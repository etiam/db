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
        std::cerr << "\"" << sourceID.toStdString() << "\":"
                  << lineNumber << ": " << message.toStdString() << std::endl;
    }

};

class EditorImpl: public QObject
{
  Q_OBJECT

  public:
    EditorImpl(Editor *parent=0) :
        QObject(),
        m_parent(parent),
        m_webView(new QWebView(parent)),
        m_webPage(new MyWebPage(parent)),
        m_layout(new QVBoxLayout(parent))
    {
        QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        m_webView->installEventFilter(parent);

        m_parent->setLayout(m_layout);
        m_layout->addWidget(m_webView);
        m_layout->setMargin(0);

        m_webView->setPage(m_webPage);
    }

    ~EditorImpl() {};

    QVariant executeJavaScript(const QString &code)
    {
        return m_webPage->mainFrame()->evaluateJavaScript(code);
    }

    void startAceWidget()
    {
        QEventLoop loop(m_parent);

        QObject::connect(m_webView, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));

        m_webView->load(QUrl("qrc:/html/ace.html"));
        loop.exec();

        QWebFrame *frame = m_webView->page()->mainFrame();
        frame->addToJavaScriptWindowObject("codeview", this);

        QFile custom(":/ace/custom.js");
        if (custom.open(QIODevice::ReadOnly))
            executeJavaScript(custom.readAll());
    }

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

  public slots:
      void onMouseMove(int x, int y) { std::cout << x << ", " << y << std::endl; };

  public:
    Editor *        m_parent;
    QWebView *      m_webView;
    QWebPage *      m_webPage;
    QVBoxLayout *   m_layout;
};
