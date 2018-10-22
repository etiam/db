/*
 * tabPage.h
 *
 *  Created on: Oct 22, 2018
 *      Author: jasonr
 */

#ifndef SRC_UI_TABPAGE_H_
#define SRC_UI_TABPAGE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QObject>

namespace Ui
{

// Base class for all tab widgets.  setTabFocus() needs to be implemented so that
// when TabWidget receives mouse Enter event it can trigger current tab widget to
// take focus.

class TabPage
{

public:
    TabPage() = default;
    ~TabPage() = default;

    virtual void setTabFocus() = 0;
};

} // namespace Ui

#endif // SRC_UI_TABPAGE_H_
