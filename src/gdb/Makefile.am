db_SOURCES               +=                                                     \
                            src/gdb/commands.cpp                                \
                            src/gdb/controller.cpp                              \
                            src/gdb/result.cpp                                  \
                                                                                \
                            src/gdb/handlers/console.cpp                        \
                                                                                \
                            src/gdb/responses/file-exec.cpp                     \
                            src/gdb/responses/break-insert.cpp                  \
                            src/gdb/responses/break-disable.cpp                 \
                            src/gdb/responses/break-delete.cpp                  \
                            src/gdb/responses/info-line.cpp                     \
                            src/gdb/responses/info-address.cpp

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
