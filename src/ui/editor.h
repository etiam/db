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

#include "core/types.h"

class QMainWindow;

namespace Ui
{

class EditorImpl;

class Editor: public QWidget
{
Q_OBJECT

public:
    explicit Editor(QMainWindow *parent = 0);
    ~Editor();

    void setGutterWidth(int width);

    void setText(const QString &newText);
    QString getText();

    void setTheme(const QString &name);

    void setHighlightMode(const QString &name);

    void setKeyboardHandler(const QString &name);

    void zoomResetText();
    void zoomInText();
    void zoomOutText();

public Q_SLOTS:
    void onGutterClicked(int row);
    void onCursorMoved(int x, int y);
    void onMouseMoved(int index);

private:
    QString getLineText(int row) const;
    int getNumLines() const;
    int getLineLength(int row) const;
    int getCharacterHeight() const;
    void clearGutterMarkers();
    void updateGutterMarkers(const QString &filename);

    void showGutter();
    void hideGutter();

private Q_SLOTS:
    void loadFile(const QString &filename);
    void highlightLocation(const Core::Location &location);
    void setCursorPosition(int col, int row);

    // update the gutter marker at the current row
    void updateGutterMarker(const Core::Location &location);

private:
    std::unique_ptr<EditorImpl> m_impl;
    Core::Location m_currentLocation;
};

}

#endif // UI_EDITOR_H_
