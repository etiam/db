db_SOURCES               +=                                             \
                            src/ui/mainwindow.cpp                       \
                            src/ui/editor.cpp                           \
                            src/ui/console.cpp                          \
                            src/ui/main.cpp
                            
nodist_db_SOURCES        +=                                             \
                            src/ui/moc_mainwindow.cpp                   \
                            src/ui/moc_editor.cpp                       \
                            src/ui/moc_console.cpp                      \
                                                                        \
                            src/ui/qrc_editor.cpp

db_CPPFLAGS              +=                                             \
                            -I$(top_builddir)/src/ui

src/ui/qrc_editor.cpp: src/ui/custom.js src/ui/ace.html

