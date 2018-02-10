bin_PROGRAMS             += db

db_SOURCES               +=                                             \
                            src/main/main.cpp

db_CPPFLAGS              +=                                             \
                            -fPIC                                       \
                            -I$(top_builddir)/src/main                  \
                            @QTCORE_CFLAGS@                             \
                            @QTWIDGETS_CFLAGS@                          \
                            @QTGUI_CFLAGS@                              \
                            @QTWEBKIT_CFLAGS@                           \
                            @QTWEBKITWIDGETS_CFLAGS@

db_LDADD                  =                                             \
                            $(top_builddir)/src/external/cppast/build/src/libcppast.a \
                            @QTCORE_LIBS@                               \
                            @QTWIDGETS_LIBS@                            \
                            @QTGUI_LIBS@                                \
                            @QTWEBKIT_LIBS@                             \
                            @QTWEBKITWIDGETS_LIBS@

db_LDFLAGS                =                                             \
                            -fPIC -rdynamic

if DO_PROFILER
db_LDADD                 +=                                             \
                            @GPERFPROFILER_LIBS@
endif
