/*
 * output.h
 *
 *  Created on: Jun 17, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef UI_OUTPUT_H_
#define UI_OUTPUT_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QPlainTextEdit>

#include "core/timer.h"

namespace Ui
{

class Output: public QPlainTextEdit
{
  Q_OBJECT

  public:
    explicit Output(QWidget *parent = nullptr);
    ~Output() = default;

  public Q_SLOTS:
    void        appendText(const QString &text);
};

}

#endif // UI_OUTPUT_H_
