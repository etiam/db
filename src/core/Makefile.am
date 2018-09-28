db_SOURCES               +=                                             \
                            src/core/global.cpp                         \
                            src/core/breakpoints.cpp                    \
                            src/core/callStack.cpp                      \
                            src/core/state.cpp                          \
                            src/core/utils.cpp                          \
                            src/core/signals.cpp                        \
                            src/core/redirector.cpp                     \
                                                                        \
                            src/core/workerThread.cpp                   \
                                                                        \
                            src/core/archive.cpp                        \
                            src/core/serializers.cpp

db_CPPFLAGS              +=                                             \
                            @LIBUT_CFLAGS@

db_LDADD                 +=                                             \
                            @LIBUT_LIBS@                                \
                            -lIex -lboost_iostreams

if HAVE_BFD_H
db_LDADD                 +=                                             \
                            -lbfd
endif
