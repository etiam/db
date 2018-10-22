/*
 * coloredOutput.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ui/coloredOutput.h>

namespace Ui
{

ColoredOutput::ColoredOutput(QWidget *parent) :
    Output(parent)
{
}

ColoredOutput::~ColoredOutput()
{
}

void
ColoredOutput::setTabFocus()
{
    setFocus();
}

void
ColoredOutput::appendText(const QString &text, const QColor &color)
{
    setTextColor(color);
    Output::appendText(text);
}

} // namespace Ui
