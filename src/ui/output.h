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

#include <QTextEdit>
#include <QString>

#include "tabPage.h"

class QTimer;

namespace Ui
{

class Output : public QTextEdit, public TabPage
{
Q_OBJECT

public:
    explicit Output(QWidget *parent = nullptr);
    ~Output() = default;

    void setTabFocus() override;

public Q_SLOTS:
    void appendText(const QString &text);
    void flush();

private:
    void trim();
    void onDebuggerStateUpdated();

    // TODO : get these from config file
    unsigned int m_maxScrollbackSize = 10000;
    unsigned int m_bufferSize = 4096;
    QString m_buffer;
    QTimer * m_flushTimer;
    QTimer * m_trimTimer;
};

}

#endif // UI_OUTPUT_H_
