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

#include "core/location.h"

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

    void    setTheme(const QString &name);

    void    setHighlightMode(const QString &name);

    void    setKeyboardHandler(const QString &name);

    void    zoomResetText();
    void    zoomInText();
    void    zoomOutText();

  public Q_SLOTS:
    void    onGutterClicked(int row);
    void    onCursorMoved(int x, int y);
    void    onMouseMoved(int x, int y);

  private:
    QString getLineText(int row) const;
    int     getNumLines() const;
    int     getLineLength(int row) const;
    int     getCharacterHeight() const;
    void    clearGutterMarkers();
    void    updateGutterMarkers(const QString &filename);

    // wink signal handlers
    void    onLoadFileSignal(const std::string &filename);
    void    onSetCursorPositionSignal(int row, int column);

    void    onUpdateGutterMarkerSignal(int row);

    std::unique_ptr<EditorImpl> m_impl;

  private Q_SLOTS:
    void    loadFile(const QString &filename);
    void    setCursorPosition(int row, int column);

    void    updateGutterMarker(int row);
};

}

#endif // UI_EDITOR_H_
