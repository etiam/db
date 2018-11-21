/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <regex>
#include <set>
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "ast/scanner.h"

#include "gdb/result.h"
#include "gdb/controller.h"

namespace
{

// return the number of identical directories that are present between
// pathnamea and pathnameb
int
pathnameOverlap(const std::string &pathnamea, const std::string &pathnameb)
{
    auto len = std::min(pathnamea.size(), pathnameb.size());
    int c=0;
    for (int n=0; n < len; ++n)
    {
        if (pathnamea[n] != pathnameb[n])
            break;
        if (pathnamea[n] == '/')
            c++;
    }

    return c;
}

}

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
listsourcefiles(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    /*
    {'message': u'done',
     'payload': {u'files': [{u'file': u'/home/jasonr/workspace/db/tests/cli_args.cpp',
                             u'fullname': u'/home/jasonr/workspace/db/tests/cli_args.cpp'},
                            {u'file': u'/usr/bin/../lib/gcc/x86_64-linux-gnu/5.4.0/../../../../include/x86_64-linux-gnu/c++/5.4.0/bits/atomic_word.h',
                             u'fullname': u'/usr/include/x86_64-linux-gnu/c++/5/bits/atomic_word.h'},
                            {u'file': u'/usr/bin/../lib/gcc/x86_64-linux-gnu/5.4.0/../../../../include/c++/5.4.0/iostream',
                             u'fullname': u'/usr/include/c++/5/iostream'}]},
     'stream': 'stdout',
     'token': 3,
     'type': 'result'}
    */

    if (match)
    {
        auto buildpath = Core::state()->vars().get<std::string>("buildpath");
        auto &sourcefiles = Core::state()->sourceFiles();

        // transform gdb's list of source files in the form of [(file, fullname), (file, fullname), ...]
        // into a map of form {file: fullname}.  filter down the list by including files closest to the
        // location of the debugged program.

        // create list of fullnames
        std::vector<std::string> fullnames;
        std::map<std::string, std::string> rawresults;
        auto files = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("files"));
        for (const auto &file : files)
        {
            const auto &entry = boost::any_cast<Gdb::Payload::Dict>(file);
            auto fullname = boost::any_cast<char *>(entry.at("fullname"));

            fullnames.push_back(fullname);
            rawresults[fullname] = boost::any_cast<char *>(entry.at("file"));
        }

        // sort fullnames into buckets based on how much they overlap with the buildpath
        std::map<int, std::set<std::string>> overlapmap;
        for (const auto &fullname : fullnames)
        {
            auto overlap = pathnameOverlap(fullname, buildpath);
            overlapmap[overlap].insert(fullname);
        }

        // add files from the bucket with the largest overlap
        std::vector<int> keys;
        transform(std::begin(overlapmap), std::end(overlapmap), std::back_inserter(keys),
                  [](decltype(overlapmap)::value_type p) { return p.first;} );
        std::sort(std::begin(keys), std::end(keys), std::greater<int>());

        if (keys.size() > 0)
        {
            // add the sourcefiles from the largest overlap bucket
            for (const auto &fullname : overlapmap[keys[0]])
            {
                sourcefiles[rawresults[fullname]] = fullname;
            }

            Core::Signals::sourceListUpdated.emit();
        }
    }

    return {"listsourcefiles", match, Controller::MatchType::TOKEN};
}

}

}
