/*
 * editorImpl.h
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QtWebKit>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWebKitWidgets>
#endif

#include "editor.h"

class MyWebPage : public QWebPage
{
  public:
    explicit MyWebPage(QObject *parent=0) : QWebPage(parent) {};

  protected:
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID) override
    {
        std::cout << sourceID.toStdString() << ":" << lineNumber << " " << message.toStdString() << std::endl;
    }

};

class EditorImpl: public QObject
{
  Q_OBJECT
  public:
    EditorImpl(Editor *parent=0) :
        QObject(),
        m_parent(parent),
        m_aceView(new QWebView(parent)),
        m_webPage(new MyWebPage(parent)),
        m_layout(new QVBoxLayout(parent))
    {
        QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        m_parent->setLayout(m_layout);
        m_layout->addWidget(m_aceView);
        m_layout->setMargin(0);

        m_aceView->setPage(m_webPage);
    }

    ~EditorImpl() {};

    /**
     * @brief Run some JS code to Ace
     * @param code javascript source
     * @return evaluation result
     */
    QVariant executeJavaScript(const QString &code)
    {
//        return m_aceView->page()->mainFrame()->evaluateJavaScript(code);
        return m_webPage->mainFrame()->evaluateJavaScript(code);
    }

    /**
     * @brief Start Ace web widget and load javascript low-level helpers
     */
    void startAceWidget()
    {
        QEventLoop loop(m_parent);

        QObject::connect(m_aceView, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));

        m_aceView->load(QUrl("qrc:/html/ace.html"));
        loop.exec();

        // Wrapper (data/wrapper.js)
        QWebFrame *frame = m_aceView->page()->mainFrame();
        frame->addToJavaScriptWindowObject("Novile", this);

        {
        QFile listeners(":/html/wrapper.js");
        if (listeners.open(QIODevice::ReadOnly))
            executeJavaScript(listeners.readAll());
        }

        {
        QFile listeners(":/ace/custom.js");
        if (listeners.open(QIODevice::ReadOnly))
            executeJavaScript(listeners.readAll());
        }

    }


    /**
     * @brief Escape symbols for JavaScript calls
     * @param text non-escaped code
     * @return escaped code (ready for js call)
     */
    QString escape(const QString &text)
    {
        QString escaped = text;
        escaped.replace("\r\n", "\n");
        escaped.replace("\r", "\n");
        escaped.replace("\n", "\\n");
        escaped.replace("\t", "\\t");
        escaped.replace("\'", "\\'");
        escaped.replace("\"", "\\\"");

        return escaped;
    }

  public:
    Editor *m_parent;
    QWebView *m_aceView;
    QWebPage *m_webPage;
    QVBoxLayout *m_layout;
};
