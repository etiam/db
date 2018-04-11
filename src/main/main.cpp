/*
 * main.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <memory>
#include <thread>
#include <regex>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "ast/astBuilder.h"
#include "gdb/gdbController.h"
#include "ui/main.h"

int main(int argc, char *argv[])
{
    using namespace boost::program_options;

    pthread_setname_np(pthread_self(), "main");

    // Declare a group of options that will be on command line
    options_description generic("Command line options");
    generic.add_options()
        ("version", "print version string.")
        ("help,h", "produce help message.");

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    options_description hidden("Hidden options");
    hidden.add_options()
        ("prog", value<std::string>(), "prog");

    options_description cmdline_options;
    cmdline_options.add(generic);
    cmdline_options.add(hidden);

    positional_options_description p;
    p.add("prog", -1);

    variables_map vm;
    store(command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    notify(vm);

    // --help
    if (vm.count("help"))
    {
        std::cout << "Usage: db [options] [input file]" << std::endl << std::endl;
        std::cout << generic << std::endl;
        std::exit(0);
    }

    // --version
    if (vm.count("version"))
    {
        std::cout << "version " << PACKAGE_VERSION << std::endl;
        std::exit(0);
    }

    auto &gdb = Core::gdbController();
    auto &ast = Core::astBuilder();

    // load prog
    if (vm.count("prog"))
    {
        auto filename = vm["prog"].as<std::string>();

        gdb->executeCommand("file-exec-and-symbols " + filename);

//        gdb->jumpToProgramStart();

        {

        auto filter1 = [](const Gdb::GdbResult &result, int token)
        {
            static std::regex infoaddress(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
            return (result.token.value == -1 &&
                    result.message.type == Gdb::Message::Type::NONE &&
                    result.payload.type == Gdb::Payload::Type::STRING &&
                    std::regex_match(result.payload.string.string, infoaddress));
        };

        auto response1 = [](const Gdb::GdbResult &result, int token)
        {
            static std::regex infoaddress(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
            std::smatch match;
            if (std::regex_match(result.payload.string.string, match, infoaddress))
            {
                auto filter2 = [](const Gdb::GdbResult &result, int token)
                {
                    static std::regex infoaddress(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
                    return (result.token.value == -1 &&
                            result.message.type == Gdb::Message::Type::NONE &&
                            result.payload.type == Gdb::Payload::Type::STRING &&
                            std::regex_match(result.payload.string.string, infoaddress));
                };

                auto response2 = [](const Gdb::GdbResult &result, int token)
                {
                    static std::regex infoaddress(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
                    std::smatch match;
                    if (std::regex_match(result.payload.string.string, match, infoaddress))
                    {
                       std::cout << "*first group " << match[1] << std::endl;
                       std::cout << "*second group " << match[2] << std::endl;
                    }
                };

                std::stringstream stream;
                stream << "interpreter-exec console \"info line *" << match[2] << "\"";
                Core::gdbController()->executeCommand(stream.str(), filter2, response2);
            }
        };

        gdb->executeCommand("interpreter-exec console \"info address main\"", filter1, response1);
        }

//        gdb->executeCommand("interpreter-exec console \"info address main\"");
//        gdb->executeCommand("interpreter-exec console \"info line *0x475080\"");

//        gdb->executeCommand("file-list-exec-source-files");
//        gdb->executeCommand("break-insert main");

        auto buildpath = boost::filesystem::absolute(boost::filesystem::path(filename).parent_path()).string();
        ast->setBuildPath(buildpath);
    }

    auto main = std::make_unique<Ui::Main>(argc, argv);
    main->run();
}
