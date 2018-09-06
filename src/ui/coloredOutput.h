/*
 * coloredOutput.h
 *
 *  Created on: Sep 5, 2018
 *      Author: jasonr
 */

#ifndef UI_COLOREDOUTPUT_H_
#define UI_COLOREDOUTPUT_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "output.h"

namespace Ui
{

class ColoredOutput : public Output
{
Q_OBJECT

public:
    explicit ColoredOutput(QWidget *parent = nullptr);
    virtual ~ColoredOutput();

public Q_SLOTS:
    void appendText(const QString &text, const QColor &color);
};

} // namespace Ui

#endif // UI_COLOREDOUTPUT_H_
