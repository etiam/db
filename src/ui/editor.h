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

#include <memory>
#include <QWidget>

class QMainWindow;

namespace Ui
{

class EditorImpl;

class Editor : public QWidget
{
  Q_OBJECT

  public:
    explicit Editor(QMainWindow *parent=0);
    ~Editor();

    void    setGutterWidth(int width);

    void    setText(const QString &newText);
    QString getText();

    void    setBreakpointMarker(int row);

    void    setTheme(const QString &name);

    void    setHighlightMode(const QString &name);

    void    setKeyboardHandler(const QString &name);

  private:
    QString getLineText(int row) const;
    int     getNumLines() const;
    int     getLineLength(int row) const;

    // wink signal handlers
    void    onLoadFileSignal(const std::string &filename);
    void    onSetCursorPositionSignal(int row, int column);

    std::unique_ptr<EditorImpl> m_impl;

  private Q_SLOTS:
    void    loadFile(const QString &filename);
    void    setCursorPosition(int row, int column);
};

}

#endif // UI_EDITOR_H_
