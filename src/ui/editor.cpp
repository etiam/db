/*
 * editor.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <QVBoxLayout>
#include <QtWebKit>
#include <QtWebKitWidgets>

#include "core/global.h"
#include "core/state.h"
#include "core/signal.h"

#include "gdb/commands.h"

#include "editor.h"

namespace Ui
{

// For sending java script messages to std::cerr

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

// TODO : combine EditorImpl into Editor

// EditorImpl

class EditorImpl: public QObject
{
  public:
    EditorImpl(Editor *parent=0) :
        QObject(),
        m_parent(parent),
        m_webView(new QWebView(parent)),
        m_webPage(new MyWebPage(parent)),
        m_layout(new QVBoxLayout(parent))
    {
        QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        m_parent->setLayout(m_layout);
        m_layout->addWidget(m_webView);
        m_layout->setMargin(2);

        m_webView->setPage(m_webPage);

        // scale qwebview font sizes by dpi (https://stackoverflow.com/questions/2019716/differing-dpi-font-sizes-in-qwebview-compared-to-all-other-qwidgets)
       auto factor = QApplication::desktop()->screen()->logicalDpiX() / 96.0;
       std::cout << "setting text size multiplier to " << factor << std::endl;
       m_webView->setTextSizeMultiplier(factor);
    }

    ~EditorImpl() {};

    QVariant executeJavaScript(const QString &code)
    {
        return m_webPage->mainFrame()->evaluateJavaScript(code);
    }

    void startAceWidget()
    {
        QEventLoop loop(m_parent);

        connect(m_webView, &QWebView::loadFinished, [&](bool){ loop.quit(); });

        m_webView->load(QUrl("qrc:/ace/ace.html"));
        loop.exec();

        QWebFrame *frame = m_webView->page()->mainFrame();
        frame->addToJavaScriptWindowObject("codeview", m_parent);

        QFile custom(":/ace/custom.js");
        if (custom.open(QIODevice::ReadOnly))
            executeJavaScript(custom.readAll());
    }

    QString escape(const QString &text)
    {
        QString escaped = text;

        escaped.replace(R"(\n)", R"(\\n)");
        escaped.replace("\r\n", "\n");
        escaped.replace("\r", "\n");
        escaped.replace("\n", "\\n");
        escaped.replace("\t", "\\t");
        escaped.replace("\'", "\\'");
        escaped.replace("\"", "\\\"");

        return escaped;
    }

  public:
    Editor *        m_parent;
    QWebView *      m_webView;
    QWebPage *      m_webPage;
    QVBoxLayout *   m_layout;
};

// Editor

Editor::Editor(QMainWindow *parent) :
    QWidget(parent),
    m_impl(std::make_unique<EditorImpl>(this))
{
    setObjectName("editor");

    m_impl->startAceWidget();
    m_impl->executeJavaScript("editor.focus()");

    m_impl->executeJavaScript("editor.setShowPrintMargin(false)");
    m_impl->executeJavaScript("editor.setDisplayIndentGuides(false)");
    m_impl->executeJavaScript("editor.setReadOnly(true)");
    m_impl->executeJavaScript("editor.setShowFoldWidgets(false)");

    auto fontname = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
    m_impl->executeJavaScript(QString("editor.setOptions({ fontFamily: \"%1\" })").arg(fontname));

    setHighlightMode("c_cpp");
    setTheme("clouds_midnight");

    Core::Signal::loadFile.connect(this, &Editor::onLoadFileSignal);
    Core::Signal::setCursorPosition.connect(this, &Editor::onSetCursorPositionSignal);
    Core::Signal::updateGutterMarker.connect(this, &Editor::onUpdateGutterMarkerSignal);
    Core::Signal::clearCurrentLocation.connect(this, &Editor::onClearCurrentLocationSignal);
}

Editor::~Editor()
{
}

// public functions

void
Editor::setGutterWidth(int width)
{
    if (width > 0)
    {
        m_impl->executeJavaScript(QString("editor.session.gutterRenderer.setWidth(%1)").arg(width));
    }
}

void
Editor::setText(const QString &newText)
{
    const QString request = "editor.getSession().setValue('%1')";
    m_impl->executeJavaScript(request.arg(m_impl->escape(newText)));

    setCursorPosition(1, 1);
}

QString
Editor::getText()
{
    return m_impl->executeJavaScript(QString("editor.getSession().getValue()")).toString();
}

void
Editor::setTheme(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.setTheme('ace/theme/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/theme-"+name+".js").arg(name));
}

void
Editor::setHighlightMode(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/mode/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/mode-"+name+".js").arg(name));
}

void
Editor::setKeyboardHandler(const QString &name)
{
    const QString request = ""
            "$.getScript('%1');"
            "editor.getSession().setMode('ace/keybinding/%2');";
    m_impl->executeJavaScript(request.arg("qrc:/ace/keybinding-"+name+".js").arg(name));
}

void
Editor::zoomResetText()
{
    auto factor = QApplication::desktop()->screen()->logicalDpiX() / 96.0;
    m_impl->m_webView->setTextSizeMultiplier(factor);
}

void
Editor::zoomInText()
{
    auto factor = m_impl->m_webView->textSizeMultiplier();
    m_impl->m_webView->setTextSizeMultiplier(factor + 0.1);
}

void
Editor::zoomOutText()
{
    auto factor = m_impl->m_webView->textSizeMultiplier();
    m_impl->m_webView->setTextSizeMultiplier(factor - 0.1);
}

// public slots

void
Editor::onGutterClicked(int row)
{
    auto filename = m_currentFilename.toStdString();
    auto &breakpoints = Core::state()->breakPoints();
    auto &gdb = Core::gdb();

    if (!breakpoints.exists(filename, row))
    {
        gdb->insertBreakpoint(filename + ":" + std::to_string(row));
    }
    else
    {
        auto &breakpoint = breakpoints.find(filename, row);
        if (breakpoint.enabled)
        {
            gdb->disableBreakpoint(breakpoint.breakpointnumber);
        }
        else
        {
            gdb->enableBreakpoint(breakpoint.breakpointnumber);
        }
    }
}

void
Editor::onCursorMoved(int x, int y)
{
}

void
Editor::onMouseMoved(int x, int y)
{
}

// private functions

QString
Editor::getLineText(int row) const
{
    return m_impl->executeJavaScript(QString("editor.getSession().getLine(%1)").arg(row)).toString();
}

int
Editor::getNumLines() const
{
    return m_impl->executeJavaScript("property('lines')").toInt();
}

int
Editor::getLineLength(int row) const
{
    return getLineText(row).length();
}

int
Editor::getCharacterHeight() const
{
    return m_impl->executeJavaScript("editor.renderer.lineHeight").toInt();
}

void
Editor::clearGutterMarkers()
{
    m_impl->executeJavaScript(QString("editor.getSession().clearBreakpoints()"));
}

void
Editor::updateGutterMarkers(const QString &filename)
{
    // call updateGutterMarker on all breakpoints with filename
    for (const auto &breakpoint : Core::state()->breakPoints().getAll())
    {
        if (breakpoint.location.filename == filename.toStdString())
            updateGutterMarker(breakpoint.location.row);
    }
}

void
Editor::showGutter()
{
    m_impl->executeJavaScript(QString("editor.renderer.setShowGutter(true)"));
}

void
Editor::hideGutter()
{
    m_impl->executeJavaScript(QString("editor.renderer.setShowGutter(false)"));
}

// wink signal handlers

void
Editor::onLoadFileSignal(const std::string &filename)
{
    QMetaObject::invokeMethod(this, "loadFile", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(filename)));
}

void
Editor::onSetCursorPositionSignal(int row, int column)
{
    QMetaObject::invokeMethod(this, "setCursorPosition", Qt::QueuedConnection, Q_ARG(int, row), Q_ARG(int, column));
}

void
Editor::onUpdateGutterMarkerSignal(int row)
{
    QMetaObject::invokeMethod(this, "updateGutterMarker", Qt::QueuedConnection, Q_ARG(int, row));
}

void
Editor::onClearCurrentLocationSignal()
{
    QMetaObject::invokeMethod(this, "clearCurrentLocation", Qt::QueuedConnection);
}

// private slots

void
Editor::loadFile(const QString &filename)
{
    // read file into editor
    if (!filename.isEmpty() && filename != m_currentFilename)
    {
        QFileInfo checkfile(filename);
        if (checkfile.exists() && checkfile.isFile())
        {
            QFile file(filename);
            QTextStream stream(&file);

            file.open(QFile::ReadOnly | QFile::Text);
            auto text = stream.readAll();
            setText(text);

            auto numlines = text.count("\n");
            auto numdigits = numlines > 0 ? (int) log10((double) numlines) + 1 : 1;
            setGutterWidth(numdigits);

            m_currentFilename = filename;

            setHighlightMode("c_cpp");
            showGutter();
            clearGutterMarkers();
            updateGutterMarkers(filename);
        }
        else
        {
            std::stringstream message;
            message << filename.toStdString() << ": No such file or directory.";
            setText(QString::fromStdString(message.str()));
            setHighlightMode("xml");
            hideGutter();
            clearGutterMarkers();
            m_currentFilename = "";
        }
    }
}

void
Editor::setCursorPosition(int row, int column)
{
    if (getNumLines() > row && getLineLength(row) >= column)
    {
        m_impl->executeJavaScript(QString("editor.moveCursorTo(%1, %2)").arg(row-1).arg(column-1));
        m_impl->executeJavaScript(QString("editor.renderer.scrollCursorIntoView(null, 0.5)"));
    }
}

void
Editor::updateGutterMarker(int row)
{
    const auto &breakpoints = Core::state()->breakPoints();
    const auto &currloc = Core::state()->currentLocation();

    std::string klass = "ace";
    if (breakpoints.exists(m_currentFilename.toStdString(), row))
    {
        klass += "_breakpoint";
        if (!breakpoints.enabled(m_currentFilename.toStdString(), row))
        {
            klass += "_disabled";
        }
    }

    if (currloc.row == row)
    {
        klass += "_currentline";
    }

    m_impl->executeJavaScript(QString("editor.getSession().setBreakpoint(%1, \"%2\")").arg(row-1).arg(QString::fromStdString(klass)));
}

void
Editor::clearCurrentLocation()
{
    const auto &breakpoints = Core::state()->breakPoints();
    const auto row = Core::state()->currentLocation().row;

    std::string klass = "ace";
    if (breakpoints.exists(m_currentFilename.toStdString(), row))
    {
        klass += "_breakpoint";
        if (!breakpoints.enabled(m_currentFilename.toStdString(), row))
        {
            klass += "_disabled";
        }
    }

    m_impl->executeJavaScript(QString("editor.getSession().setBreakpoint(%1, \"%2\")").arg(row-1).arg(QString::fromStdString(klass)));
}

}
