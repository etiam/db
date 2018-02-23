db_SOURCES               +=                                                     \
                            src/gdb/pyGdbMiInterface.cpp

db_CPPFLAGS              +=                                                     \
                            @PYTHON_CFLAGS@

db_LDADD                 +=                                                     \
                            @PYTHON_LIBS@

nodist_db_SOURCES        +=                                                     \
                            src/gdb/qrc_pygdbmi.cpp

BUILT_SOURCES            +=                                                     \
                            src/gdb/pygdbmi.qrc                                 \
                            src/external/pygdbmi/pygdbmi/gdbcontroller.pyc
