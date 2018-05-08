db_SOURCES               +=                                             \
                            src/core/global.cpp                         \
                            src/core/breakpoints.cpp                    \
                            src/core/state.cpp                          \
                            src/core/util.cpp                           \
                            src/core/signal.cpp

db_CPPFLAGS              +=                                             \
                            -I$(top_srcdir)/src/external/libut/src

db_LDADD                 +=                                             \
                            -L$(top_builddir)/src/external/libut/.libs  \
							-lut -lIex -lboost_iostreams

db_LDFLAGS               +=                                             \
                            -Wl,--rpath -Wl,$(top_builddir)/src/external/libut/.libs


if HAVE_BFD_H
db_LDADD                 +=                                             \
                            -lbfd
endif
