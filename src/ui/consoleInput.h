/*
 * consoleInput.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "external/lineedit/src/history_line_edit.hpp"

class QCompleter;

namespace Ui
{

class ConsoleInput : public HistoryLineEdit
{
  Q_OBJECT

  public:
    explicit ConsoleInput(QWidget *parent = nullptr);

  protected:
    void keyPressEvent(QKeyEvent *) override;
    void focusInEvent(QFocusEvent *event) override;

  private Q_SLOTS:
    void updateCompletionData();

  private:
    void autoComplete(bool notify);

    QCompleter *m_completer;
};

}
