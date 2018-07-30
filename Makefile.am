ACLOCAL_AMFLAGS           = -I m4

SUBDIRS                   = $(subdirs) $(subdirs_extra)

bin_PROGRAMS              =
nodist_db_SOURCES         =
noinst_LIBRARIES          =
BUILT_SOURCES             =
MOSTLYCLEANFILES          =
EXTRA_PROGRAMS            =

ALL_CPPFLAGS              = -I$(top_srcdir)/src -MJ $@.json

db_SOURCES                = 
db_CPPFLAGS               = $(ALL_CPPFLAGS)
db_LDFLAGS                =
db_LDADD                  =

include $(top_srcdir)/src/main/Makefile.am
include $(top_srcdir)/src/core/Makefile.am
include $(top_srcdir)/src/ui/Makefile.am
include $(top_srcdir)/src/gdb/Makefile.am
include $(top_srcdir)/src/gdb/handlers/Makefile.am
include $(top_srcdir)/src/ast/Makefile.am
include $(top_srcdir)/src/external/Makefile.am
include $(top_srcdir)/tests/Makefile.am

include $(top_srcdir)/extra.am
include $(top_srcdir)/analyze.am
include $(top_srcdir)/qt.am     

clean-local: clean-moc-extra clean-qrc-extra clean-uic-extra \
	         clean-compile_commands clean-extras clean-pyc clean-qrc
