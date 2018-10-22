/*
 * console.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef UI_CONSOLE_H_
#define UI_CONSOLE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QFrame>

#include "tabPage.h"

namespace Ui
{

class Output;
class ConsoleInput;

class GdbConsole : public QFrame, public TabPage
{
Q_OBJECT

public:
    explicit GdbConsole(QWidget *parent = nullptr, bool editable = false);
    ~GdbConsole() = default;

    void setTabFocus() override;

public Q_SLOTS:
    void appendText(const QString &text);

protected:
    bool focusNextPrevChild(bool next) override;
    void enterEvent(QEvent *event) override;

private:
    Output * m_output;
    ConsoleInput * m_lineedit;

private Q_SLOTS:
    void onDebuggerStateUpdated();
};

}

#endif // UI_CONSOLE_H_
