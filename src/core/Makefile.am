db_SOURCES               +=                                             \
                            src/core/global.cpp                         \
                            src/core/breakpoints.cpp                    \
                            src/core/state.cpp                          \
                            src/core/util.cpp                           \
                            src/core/signal.cpp

db_CPPFLAGS              +=                                             \
                            @LIBUT_CFLAGS@

db_LDADD                 +=                                             \
                            @LIBUT_LIBS@                                \
                            -lIex -lboost_iostreams

if HAVE_BFD_H
db_LDADD                 +=                                             \
                            -lbfd
endif
