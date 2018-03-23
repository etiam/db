/*
 * editor.h
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef UI_EDITOR_H_
#define UI_EDITOR_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QWidget>

class EditorImpl;
class QMainWindow;

class Editor : public QWidget
{
  Q_OBJECT

  public:
    explicit Editor(QMainWindow *parent=0);
    ~Editor() {};

    void setGutterWidth(int width);

    void setCursorPosition(int row, int column);

    void setText(const QString &newText);
    QString getText();

    void setTheme(const QString &name);

    void setHighlightMode(const QString &name);

    void setKeyboardHandler(const QString &name);

  private:
    QString getLineText(int row) const;
    int getNumLines() const;
    int getLineLength(int row) const;

  protected:
    bool eventFilter(QObject *object, QEvent *filteredEvent);

    EditorImpl* d;

  private:
    QMainWindow* m_parent;
};

#endif // UI_EDITOR_H_
