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
    explicit Console(QWidget *parent = nullptr, bool editable = false);
    ~Console() = default;

  public Q_SLOTS:
    void appendText(const QString &text);

  private:
    bool m_editable;

  protected:
    void keyPressEvent(QKeyEvent *e) override;
};

}

#endif // UI_CONSOLE_H_
