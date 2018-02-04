db_SOURCES               +=                                             \
                            src/ui/mainwindow.cpp                       \
                                                                        \
                            src/external/novile/src/editor.cpp
                            
nodist_db_SOURCES        +=                                             \
                            src/ui/moc_mainwindow.cpp                   \
                            src/ui/ui_mainwindow.h                      \
                                                                        \
                            src/external/novile/src/moc_editor.cpp      \
                            src/external/novile/src/moc_editor_p.cpp

db_CPPFLAGS              +=                                             \
                            -I$(top_builddir)/src/ui                    \
                            -I$(top_srcdir)/src/external/novile/src