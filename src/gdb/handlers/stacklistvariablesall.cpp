/*
 * stacklistframes.cpp
 *
 *  Created on: Jun 5, 2018
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

#include "gdb/result.h"
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
stacklistvariablesall(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    /*
{'message': u'done',
 'payload': {u'variables': [{u'arg': u'1', u'name': u'argc', u'value': u'2'},
                            {u'arg': u'1',
                             u'name': u'argv',
                             u'value': u'0x7fffffffd9b8'},
                            {u'name': u'infile_name',
                             u'value': u'<error reading variable: Cannot create a lazy string with address 0x0, and a non-zero length.>'},
                            {u'name': u'infile',
                             u'value': u'{<std::basic_istream<char, std::char_traits<char> >> = {<std::basic_ios<char, std::char_traits<char> >> = <invalid address>, _vptr.basic_istream = 0x7fffffffd850, _M_gcount = 140737351969059}, _M_filebuf = {<std::basic_streambuf<char, std::char_traits<char> >> = {_vptr.basic_streambuf = 0x0, _M_in_beg = 0x0, _M_in_cur = 0x0, _M_in_end = 0x0, _M_out_beg = 0x0, _M_out_cur = 0x0, _M_out_end = 0x0, _M_buf_locale = {static none = 0, static ctype = 1, static numeric = 2, static collate = 4, static time = 8, static monetary = 16, static messages = 32, static all = 63, _M_impl = 0x0, static _S_classic = 0x7ffff7dd57a0 <(anonymous namespace)::c_locale_impl>, static _S_global = 0x7ffff7dd57a0 <(anonymous namespace)::c_locale_impl>, static _S_categories = 0x7ffff7dc81e0 <__gnu_cxx::category_names>, static _S_once = 0, static _S_twinned_facets = 0x7ffff7dc97c0 <std::locale::_S_twinned_facets>}}, _M_lock = {__data = {__lock = 0, __count = 0, __owner = 0, __nusers = 0, __kind = 0, __spins = 0, __elision = 0, __list = {__prev = 0x0, __next = 0x0}}, __size = \'\\000\' <repeats 39 times>, __align = 0}, _M_file = {_M_cfile = 0x0, _M_cfile_created = false}, _M_mode = 0, _M_state_beg = {__count = 0, __value = {__wch = 0, __wchb = "\\000\\000\\000"}}, _M_state_cur = {__count = 65280, __value = {__wch = 4278190080, __wchb = "\\000\\000\\000\\377"}}, _M_state_last = {__count = 255, __value = {__wch = 0, __wchb = "\\000\\000\\000"}}, _M_buf = 0x0, _M_buf_size = 0, _M_buf_allocated = false, _M_reading = false, _M_writing = false, _M_pback = 0 \'\\000\', _M_pback_cur_save = 0x0, _M_pback_end_save = 0x0, _M_pback_init = false, _M_codecvt = 0x0, _M_ext_buf = 0x0, _M_ext_buf_size = 0, _M_ext_next = 0x0, _M_ext_end = 0x0}}'},
                            {u'name': u'width', u'value': u'725871085'},
                            {u'name': u'height', u'value': u'0'},
                            {u'name': u'outfile_name',
                             u'value': u'<incomplete sequence \\336>'},
                            {u'name': u'scene',
                             u'value': u'{camera_ = 0x7ffff71749e8, lights_ = std::vector of length -263335, capacity -253607 = {0x7ffff716c000, 0x7ffff7fd7ea0, 0x7ffff752fba0, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x7ffff7fd79d8, 0x7ffff7fd6000, 0x0, 0x7ffff7fd6478, 0x0, 0x7ffff752fba0, 0x7ffff752fc60, 0x7ffff752fc50, 0x7ffff752fbf0, 0x7ffff752fc10, 0x7ffff752fc20, 0x7ffff752fc90, 0x7ffff752fca0, 0x7ffff752fcb0, 0x7ffff752fc30, 0x7ffff752fc40, 0x7ffff752fbc0, 0x0, 0x7ffff752fbb0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7ffff752fc70, 0x0, 0x0, 0x7ffff752fc80, 0x0, 0x7ffff752fbd0, 0x0, 0x7ffff752fbe0, 0x0, 0x0, 0x7ffff752fce0, 0x0, 0x0, 0x0, 0x7ffff752fd00, 0x7ffff752fcf0, 0x7ffff752fcd0, 0x7ffff752fcc0, 0x0, 0x0, 0x7ffff752fd20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7ffff752fd10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7ffff752fc00, 0x7ffff716c040, 0x7ffff718c950 <__libc_main>, 0x1e000a, 0x0, 0x0, 0x7ffff7fd6470, 0x0, 0x7ffff7ffe168, 0x7ffff7fd5000, 0x3f30000001c, 0xe000000ff, 0x7ffff716c2c8, 0x7ffff716cac8, 0x7ffff716da6c, 0x401d00000000, 0x0, 0x0, 0x0, 0x7ffff71829d8, 0x7ffff7fd64a0, 0x7ffff716c000, 0x7ffff75359a0, 0x7ffff732c000, 0x7ffff7ffe420, 0x0, 0x0, 0x0, 0x4, 0x7ffff7fd6358, 0x7ffff7fd62b8, 0x0, 0x801, 0x48139e, 0x0, 0x0, 0x7ffff7fd6578, 0x0, 0x100000000, 0x0, 0x10, 0x0, 0x0, 0x0, 0x7ffff7171eb0, 0x0, 0x7ffff7fd6000, 0x7ffff7171eb0, 0x7ffff752c7c0, 0x10, 0x78, 0x8, 0x0, 0x98, 0x2, 0x0, 0x3c07c0, 0x3840, 0x6, 0x0, 0x7ffff7fd6490, 0x0, 0x1, 0x2e6f732e6362696c, 0x36, 0x3638782f62696c2f, 0x756e696c2d34365f, 0x696c00756e672d78, 0x362e6f732e6362, 0x7ffff7fd7000, 0x7ffff7fd74f0, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x7ffff7fd79d8, 0x0, 0x7ffff7fd7000, 0x7ffff7fd74f0, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x7ffff7fd79d8, 0x7ffff7fd74f0, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x0, 0x7ffff7fd74f0, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x7ffff7fd79d8, 0x7ffff7fd6000, 0x0, 0x7ffff7fd79d8, 0x7ffff7fd6000, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x0, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x7ffff7ffe168, 0x7ffff7fd7000, 0x7ffff7fd74f0, 0x7ffff7fd79d8, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x0, 0x7ffff7ffe168, 0x7ffff7fd7000, 0x7ffff7fd74f0, 0x7ffff7fd79d8, 0x7ffff7fd6000, 0x7ffff7ffd9d8 <_rtld_global+2456>, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40feab...}, objects_ = std::vector of length 0, capacity 17592168982143 = {<error reading variable>'},
                            {u'name': u'image',
                             u'value': u'{buffer_ = 0xffffffff, width_ = -136495872, height_ = 32767}'},
                            {u'name': u'start', u'value': u'4224500'}]},
 'stream': 'stdout',
 'token': 9,
 'type': 'result'}
    */

    if (match)
     {
         const auto &dict = result.payload.dict;
         if (dict.find("variables") != std::end(dict))
         {
             auto &vars = Core::state()->variables();

             auto variables = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("variables"));
             for (const auto &variable : variables)
             {
                 const auto &entry = boost::any_cast<Gdb::Payload::Dict>(variable);

                 auto name = boost::any_cast<char *>(entry.at("name"));

                 if (entry.at("value").type() == typeid(Payload::Dict))
                     vars[name].value = std::string("<<NOT IMPLEMENTED YET>>");  // TODO : implement this shit!

                 else if (entry.at("value").type() == typeid(char*))
                     vars[name].value = std::string(boost::any_cast<char *>(entry.at("value")));
             }

             Core::Signals::variablesUpdated.emit();
         }
     }

    return {"stacklistvariableall", match, Controller::MatchType::TOKEN};
}

}

}
