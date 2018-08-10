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
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "ast/scanner.h"

#include "gdb/commands.h"
#include "gdb/result.h"

namespace
{

// compare how similar two filenames are based on how many directories
// are in common between them.  common should be constant among the
// set of files compared.
int
filenameOverlap(const std::string &filename, const std::string &common)
{
    auto len = std::min(filename.size(), common.size());
    auto t = std::count(common.begin(), common.end(), '/');
    int c=0;
    for (int n=0; n < len; ++n)
    {
        if (filename[n] != common[n])
            break;
        if (filename[n] == '/')
            c++;
    }

    return (static_cast<float>(c) / t) * 100;
}

}

namespace Gdb
{

namespace Handlers
{

bool
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

        auto files = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("files"));
        for (const auto &file : files)
        {
            const auto &entry = boost::any_cast<Gdb::Payload::Dict>(file);
            const auto &fullname = boost::any_cast<char *>(entry.at("fullname"));
            const auto filename = boost::filesystem::path(fullname).filename().string();
            if (std::find(std::begin(sourcefiles), std::end(sourcefiles), fullname) == std::end(sourcefiles))
            {
                if (filenameOverlap(fullname, buildpath) > 50)
                    sourcefiles.push_back(fullname);
            }
        }

        Core::Signals::sourceListUpdated();
    }

    return match;
}

}

}
