db_SOURCES               +=                                                     \
                            src/gdb/gdbController.cpp                           \
                            src/gdb/gdbUtil.cpp                                 \
                            src/gdb/gdbResult.cpp

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
