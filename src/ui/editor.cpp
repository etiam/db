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
#include "core/signals.h"

#include "gdb/global.h"
#include "gdb/commands.h"

#include "editor.h"

Q_DECLARE_METATYPE(Core::Location);

namespace Ui
{

// For sending java-script messages to std::cerr

class MyWebPage : public QWebPage
{
  public:
    explicit MyWebPage(QObject *parent=0) : QWebPage(parent) { };

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

    // set some editor ui options
    m_impl->executeJavaScript("editor.setShowPrintMargin(false)");
    m_impl->executeJavaScript("editor.setDisplayIndentGuides(false)");
    m_impl->executeJavaScript("editor.setReadOnly(true)");
    m_impl->executeJavaScript("editor.setShowFoldWidgets(false)");

    // set editor font to system default fixed font
    auto fontname = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
    m_impl->executeJavaScript(QString("editor.setOptions({ fontFamily: \"%1\" })").arg(fontname));

    // don't display the cursor
    m_impl->executeJavaScript("editor.renderer.$cursorLayer.element.style.display = \"none\"");

    // don't highlight active line/gutter
    m_impl->executeJavaScript("editor.setOptions({ highlightActiveLine: false })");
    m_impl->executeJavaScript("editor.setOptions({ highlightGutterLine: false })");

    // syntax highlighting
    setHighlightMode("c_cpp");

    // color theme
    setTheme("clouds_midnight");

    qRegisterMetaType<Core::Location>("Location");

    // connect signal handlers
    Core::Signals::loadEditorSource.connect([this](const std::string &f)
    {
        QMetaObject::invokeMethod(this, "loadFile", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(f)));
    });

    Core::Signals::updateGutterMarker.connect([this](const Core::Location &l)
    {
        QMetaObject::invokeMethod(this, "updateGutterMarker", Qt::QueuedConnection, Q_ARG(Core::Location, l));
    });

    Core::Signals::highlightLocation.connect([this](const Core::Location &l)
    {
        QMetaObject::invokeMethod(this, "highlightLocation", Qt::QueuedConnection, Q_ARG(Core::Location, l));
    });

    Core::Signals::setCursorLocation.connect([this](const Core::Location &l)
    {
        QMetaObject::invokeMethod(this, "setCursorPosition", Qt::QueuedConnection, Q_ARG(int, 1), Q_ARG(int, l.row));
    });

    Core::Signals::debuggerStateUpdated.connect(this, &Editor::onDebuggerStateUpdated);
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
    const auto &location = Core::Location({"", m_currentLocation.filename, row});
    auto &breakpoints = Core::state()->breakPoints();
    auto &gdb = Gdb::commands();

    if (!breakpoints.exists(location))
    {
        assert(m_currentLocation.filename.size() != 0);
        gdb->insertBreakpoint(m_currentLocation.filename + ":" + std::to_string(row));
    }
    else
    {
        auto &breakpoint = breakpoints.find(location);
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
Editor::onMouseMoved(int index)
{
//    std::cout << index << std::endl;
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
            updateGutterMarker(breakpoint.location);
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

void
Editor::onDebuggerStateUpdated()
{
    const auto state = Core::state()->debuggerState();

    switch(state)
    {
        case Core::State::Debugger::PAUSED:
        case Core::State::Debugger::LOADED:
            setDisabled(false);
            m_impl->executeJavaScript("editor.container.style.opacity=1.0");
            break;

        case Core::State::Debugger::RUNNING:
            setDisabled(true);
            m_impl->executeJavaScript("editor.container.style.opacity=0.8");
            break;

        default:
            break;
    }
}

// private slots

void
Editor::loadFile(const QString &filename)
{
    // read file into editor
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

        setHighlightMode("c_cpp");
        showGutter();
        clearGutterMarkers();
        updateGutterMarkers(filename);

        m_currentLocation.filename = filename.toStdString();
    }
    else
    {
        std::stringstream message;
        message << filename.toStdString() << ": No such file or directory.";
        setText(QString::fromStdString(message.str()));
        setHighlightMode("xml");
        hideGutter();
        clearGutterMarkers();
    }
}

void
Editor::highlightLocation(const Core::Location &location)
{
    if (getNumLines() > location.row)
    {
        m_impl->executeJavaScript(QString("unhighlightcurrentline()"));
        m_impl->executeJavaScript(QString("highlightline(%1)").arg(location.row-1));
    }

    m_currentLocation = location;
}

void
Editor::setCursorPosition(int col, int row)
{
    if (getNumLines() > row)
    {
        m_impl->executeJavaScript(QString("editor.moveCursorTo(%1, %2)").arg(row-1).arg(col-1));
        m_impl->executeJavaScript(QString("editor.renderer.scrollCursorIntoView(null, 0.5)"));
    }
}

void
Editor::updateGutterMarker(const Core::Location &location)
{
    const auto &breakpoints = Core::state()->breakPoints();

    std::string klass = "ace";
    if (breakpoints.exists(location))
    {
        klass += "_breakpoint";
        if (!breakpoints.enabled(location))
        {
            klass += "_disabled";
        }
    }

    m_impl->executeJavaScript(QString("editor.getSession().setBreakpoint(%1, \"%2\")").arg(location.row-1).arg(QString::fromStdString(klass)));
}

}
