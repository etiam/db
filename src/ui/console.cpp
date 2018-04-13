/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "console.h"

namespace Ui
{

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setFont(QFont("hack", 12));
}

Console::~Console()
{
}

}
