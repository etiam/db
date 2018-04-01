bin_PROGRAMS             += db

db_SOURCES               +=                                             \
                            src/main/main.cpp

db_CPPFLAGS              +=                                             \
                            -fPIC                                       \
                            -I$(abs_builddir)                           \
                            -I$(top_builddir)/src/main                  \
                            @BOOST_CPPFLAGS@                            \
                            @QTCORE_CFLAGS@                             \
                            @QTWIDGETS_CFLAGS@                          \
                            @QTGUI_CFLAGS@                              \
                            @QTWEBKIT_CFLAGS@                           \
                            @QTWEBKITWIDGETS_CFLAGS@

db_LDADD                 +=                                             \
                            @BOOST_LDFLAGS@                             \
                            @BOOST_SYSTEM_LIB@                          \
                            @BOOST_FILESYSTEM_LIB@                      \
                            @BOOST_PROGRAM_OPTIONS_LIB@                 \
                            @QTCORE_LIBS@                               \
                            @QTWIDGETS_LIBS@                            \
                            @QTGUI_LIBS@                                \
                            @QTWEBKIT_LIBS@                             \
                            @QTWEBKITWIDGETS_LIBS@

db_LDFLAGS               +=                                             \
                            -pthread -fPIC -rdynamic

if DO_PROFILER
db_LDADD                 +=                                             \
                            @GPERFPROFILER_LIBS@
endif
