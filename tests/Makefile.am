EXTRA_PROGRAMS             +=                                               \
                              helloworld                                    \
                              multiunit                                     \
                              infiniteloop

ALL_CPPFLAGS                =                                               \
                              -I$(top_srcdir)/src

ALL_LDADD                   =

helloworld_SOURCES          = tests/helloworld.cpp
helloworld_LDFLAGS          = $(ALL_LDFLAGS)
helloworld_CPPFLAGS         = $(ALL_CPPFLAGS)
helloworld_LDADD            = $(ALL_LDADD)

multiunit_SOURCES           = tests/multiunitA.cpp tests/multiunitB.cpp
multiunit_LDFLAGS           = $(ALL_LDFLAGS)
multiunit_CPPFLAGS          = $(ALL_CPPFLAGS)
multiunit_LDADD             = $(ALL_LDADD)

infiniteloop_SOURCES        = tests/infiniteloop.cpp
infiniteloop_LDFLAGS        = $(ALL_LDFLAGS)
infiniteloop_CPPFLAGS       = $(ALL_CPPFLAGS)
infiniteloop_LDADD          = $(ALL_LDADD)
