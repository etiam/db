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

#include <QPlainTextEdit>

namespace Ui
{

class Console: public QPlainTextEdit
{
  Q_OBJECT

  public:
    Console(QWidget *parent=0);
    virtual ~Console();

  public Q_SLOTS:
    void appendText(const QString &text, bool newline=true);

  private:

    // wink signal handlers
    void onAppendConsoleTextSignal(const std::string &text, bool newline);
};

}

#endif // UI_CONSOLE_H_
