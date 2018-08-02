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

#include "core/timer.h"

namespace Ui
{

class Output;

class Console: public QFrame
{
  Q_OBJECT

  public:
    explicit Console(QWidget *parent = nullptr, bool editable = false);
    ~Console() = default;

  public Q_SLOTS:
    void appendText(const QString &text);

  private:
    Output * m_output;
};

}

#endif // UI_CONSOLE_H_
