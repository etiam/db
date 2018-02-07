#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QMainWindow>
//#include "novile/src/editor.h"
#include "editor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &filename, QWidget *parent = 0);
    ~MainWindow();

    // Establishers for Editor interaction
    void establishBaseConnects();
    void establishEventConnects();

    // Define additional information
    void initExampleFiles();
    void setupStartValues();

    // GUI controls reactions
    void updateDocument(int index);
    void updateMode(int index);
    void updateTheme(int index);
    void updateHighlightActive(int checked);
    void updateHighlightSelected(int checked);
    void updateShowPrintMargin(int checked);
    void updateShowInvisibles(int checked);
    void updateShowGutter(int checked);
    void updateShowIndent(int checked);
    void updateReadOnly(int checked);
    void updateFadeFold(int checked);

    void readSettings();
    void writeSettings();

private:
    Ui::MainWindow *ui;
    Editor *editor;
    QMap<QString, QString> documents;
};

#endif // MAINWINDOW_H
