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

// compare how similar two filenames are based on how many directories
// are in common between them. 'common' should be constant among the
// set of files compared.
int
filenameOverlap(const std::string &filename, const std::string &common)
{
    auto len = std::min(filename.size(), common.size());
    int c=0;
    for (int n=0; n < len; ++n)
    {
        if (filename[n] != common[n])
            break;
        if (filename[n] == '/')
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

        // filter down the list of source files that gdb returns by excluding files
        // not in the same file system tree as the buildpath.

        // create list of fullnames
        std::vector<std::string> fullnames;
        auto files = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("files"));
        for (const auto &file : files)
        {
            const auto &entry = boost::any_cast<Gdb::Payload::Dict>(file);

            fullnames.push_back(boost::any_cast<char *>(entry.at("fullname")));
        }

        // sort fullnames into buckets based on how much they overlap with the buildpath
        std::map<int, std::set<std::string>> overlapmap;
        for (const auto &fullname : fullnames)
        {
            auto overlap = filenameOverlap(fullname, buildpath);
            overlapmap[overlap].insert(fullname);
        }

        // choose the bucket with the largest overlap
        std::vector<int> keys;
        transform(std::begin(overlapmap), std::end(overlapmap), back_inserter(keys),
                  [](decltype(overlapmap)::value_type p) { return p.first;} );
        sort(std::begin(keys), std::end(keys));

        if (keys.size() > 0)
        {
            auto closestgroup = keys[keys.size()-1];

            // add the filename of each fullname from the largest overlap bucket
            for (const auto &fullname : overlapmap[closestgroup])
            {
                const auto filename = boost::filesystem::path(fullname).filename().string();
                if (std::find(std::begin(sourcefiles), std::end(sourcefiles), fullname) == std::end(sourcefiles))
                {
                    sourcefiles.push_back(fullname);
                }
            }

            Core::Signals::sourceListUpdated.emit();
        }
    }

    return {"listsourcefiles", match, Controller::MatchType::TOKEN};
}

}

}
