/*
 * console.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ui/console.h>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setReadOnly(true);
}

Console::~Console()
{
}

