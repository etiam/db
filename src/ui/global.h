/*
 * global.h
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifndef UI_GLOBAL_H_
#define UI_GLOBAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

namespace Ui
{

// global init
void initialize(int &argc, char *argv[]);
void shutdown();

void run();

} // namespace Ui

#endif // UI_GLOBAL_H_
