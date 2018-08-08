db_SOURCES               +=                                             \
                            src/ui/mainWindow.cpp                       \
                            src/ui/editor.cpp                           \
                            src/ui/output.cpp                           \
                            src/ui/console.cpp                          \
                            src/ui/consoleInput.cpp                     \
                                                                        \
                            src/ui/callStack.cpp                        \
                            src/ui/breakPoints.cpp                      \
                                                                        \
                            src/ui/callStackItemModel.cpp               \
                            src/ui/breakPointsItemModel.cpp             \
                                                                        \
                            src/ui/debugControls.cpp                    \
                            src/ui/main.cpp
                            
nodist_db_SOURCES        +=                                             \
                            src/ui/moc_mainWindow.cpp                   \
                            src/ui/moc_editor.cpp                       \
                            src/ui/moc_output.cpp                       \
                            src/ui/moc_console.cpp                      \
                            src/ui/moc_consoleInput.cpp                 \
                                                                        \
                            src/ui/moc_callStack.cpp                    \
                            src/ui/moc_breakPoints.cpp                  \
                                                                        \
                            src/ui/moc_breakPointsItemModel.cpp         \
                            src/ui/moc_callStackItemModel.cpp           \
                                                                        \
                            src/ui/qrc_editor.cpp

db_CPPFLAGS              +=                                             \
                            -I$(top_builddir)/src/ui

src/ui/qrc_editor.cpp: src/ui/custom.js src/ui/custom.qss src/ui/ace.html
