ACLOCAL_AMFLAGS           = -I m4

bin_PROGRAMS              =
nodist_db_SOURCES         =
noinst_LIBRARIES          =
BUILT_SOURCES             =
MOSTLYCLEANFILES          =

db_SOURCES                = 
db_CPPFLAGS               =                                             \
                            -I$(top_srcdir)/src

include $(top_srcdir)/src/main/Makefile.am
include $(top_srcdir)/src/ui/Makefile.am

EXTRA_DIST                = $(nodist_db_la_SOURCES:moc_%.cpp=%.h)
CLEANFILES                = $(filter moc_%.cpp,$(db_la_SOURCES))

include $(top_srcdir)/extra.am
include $(top_srcdir)/analyze.am
include $(top_srcdir)/qt.am     
