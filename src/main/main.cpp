/*
 * main.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

#include "ui/main.h"

int main(int argc, char *argv[])
{
    auto mainwindow = std::make_unique<Ui::Main>(argc, argv);
    mainwindow->run();
}
