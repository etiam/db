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

class Editor : public QWidget
{
  Q_OBJECT

  public:
    explicit Editor(QWidget *parent = 0);
    ~Editor() {};

    QString line(int row) const;
    int lines() const;
    int lineLength(int row) const;

    void setCursorPosition(int row, int column);
    void setText(const QString &newText);

    void setTheme(const QString &name, const QUrl &url);
    void setTheme(const QString &name);

    void setHighlightMode(const QString &name, const QUrl &url);
    void setHighlightMode(const QString &name);

  protected:

    EditorImpl* d;
};

#endif // UI_EDITOR_H_
