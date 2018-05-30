db_SOURCES               +=                                                     \
                            src/gdb/commands.cpp                                \
                            src/gdb/controller.cpp                              \
                            src/gdb/result.cpp                                  \
                                                                                \
                            src/gdb/handlers/console.cpp                        \
                            src/gdb/handlers/logging.cpp                        \
                            src/gdb/handlers/notify.cpp                         \
                            src/gdb/handlers/output.cpp                         \
                                                                                \
                            src/gdb/handlers/stopped.cpp                        \
                            src/gdb/handlers/threadgroupstarted.cpp             \
                            src/gdb/handlers/execrun.cpp                        \
                            src/gdb/handlers/execnext.cpp                       \
                                                                                \
                            src/gdb/handlers/interpreterexec.cpp                \
                            src/gdb/handlers/processexited.cpp                  \
                            src/gdb/handlers/killprog.cpp                       \
                                                                                \
                            src/gdb/handlers/fileexec.cpp                       \
                            src/gdb/handlers/breakinsert.cpp                    \
                            src/gdb/handlers/breakdisable.cpp                   \
                            src/gdb/handlers/breakdelete.cpp                    \
                            src/gdb/handlers/infoline.cpp                       \
                            src/gdb/handlers/infoaddress.cpp

db_CPPFLAGS              +=                                                     \
                            @PYTHON_CPPFLAGS@                                   \
                            @BOOST_CPPFLAGS@

db_LDADD                 +=                                                     \
                            @PYTHON_LIBS@                                       \
                            -l@BOOST_PYTHON_LIB@
                            
db_LDFLAGS               +=                                                     \
                            @BOOST_LDFLAGS@

nodist_db_SOURCES        +=                                                     \
                            src/gdb/qrc_pygdbmi.cpp

BUILT_SOURCES            +=                                                     \
                            src/gdb/pygdbmi.qrc                                 \
                            src/external/pygdbmi/pygdbmi/printcolor.pyc         \
                            src/external/pygdbmi/pygdbmi/StringStream.pyc       \
                            src/external/pygdbmi/pygdbmi/gdbcontroller.pyc      \
                            src/external/pygdbmi/pygdbmi/gdbmiparser.pyc
