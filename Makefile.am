ACLOCAL_AMFLAGS           = -I m4

SUBDIRS                   = $(subdirs) $(subdirs_extra)

bin_PROGRAMS              =
nodist_db_SOURCES         =
noinst_LIBRARIES          =
BUILT_SOURCES             =
MOSTLYCLEANFILES          =
EXTRA_PROGRAMS            =

db_SOURCES                = 
db_CPPFLAGS               = -I$(top_srcdir)/src -MJ $@.json
db_LDFLAGS                =
db_LDADD                  =

include $(top_srcdir)/src/main/Makefile.am
include $(top_srcdir)/src/core/Makefile.am
include $(top_srcdir)/src/ui/Makefile.am
include $(top_srcdir)/src/gdb/Makefile.am
include $(top_srcdir)/src/ast/Makefile.am
include $(top_srcdir)/src/external/Makefile.am
include $(top_srcdir)/tests/Makefile.am

EXTRA_DIST                = $(nodist_db_la_SOURCES:moc_%.cpp=%.h)
CLEANFILES                = $(filter moc_%.cpp,$(db_la_SOURCES))

include $(top_srcdir)/extra.am
include $(top_srcdir)/analyze.am
include $(top_srcdir)/qt.am     
