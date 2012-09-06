#include "mainwindow.h"

#include "locationedit.h"
#include "tvremotebridge.h"
#include "browsersettings.h"
#include "utils.h"
#include "ruiwebpage.h"
#include <QMenuBar>
#include <QKeyEvent>
#include <QAction>
#include <QSplitter>
#include <QWebView>
#include <QWebPage>
#include <QCompleter>

#define TV_REMOTE_SIMULATOR 1

#define RUI_WIDTH 880
#define REMOTE_WIDTH 246
#define RUI_HEIGHT 690

MainWindow::MainWindow()
    : m_page(0)
    , m_pageRemote(0)
    , m_navigationBar(0)
    , m_urlEdit(0)
    , m_browserSettings(BrowserSettings::Instance())
{
    init();
}

void MainWindow::init()
{
    int height = RUI_HEIGHT;
    int width = RUI_WIDTH;

    // We house the RUI webview and the TV Remote webview in a splitter.
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);
    width += REMOTE_WIDTH;

    // RUI webview
    m_page = new RUIWebPage(this);
    m_view = new QWebView(splitter);
    m_view->setPage(m_page);
    m_view->installEventFilter(this);
    m_view->resize(width,height);

    // TV Remote webview. Standard QWebPage (no useragent string modification)
    m_pageRemote = new QWebPage(this);
    m_tvRemoteView = new QWebView(splitter);
    m_tvRemoteView->setPage(m_pageRemote);

    QUrl qurl = urlFromUserInput(m_browserSettings->tvRemoteURL);
    m_pageRemote->mainFrame()->load(qurl);

    // Configure splitter
    QList<int> paneWidths;

    paneWidths.append(RUI_WIDTH);
    paneWidths.append(REMOTE_WIDTH);

    splitter->setSizes(paneWidths);

    splitter->setHandleWidth(1);
    QSplitterHandle* handle = splitter->handle(1);
    handle->setEnabled(false);

    // Handle key presses from the remote
    m_tvRemoteBridge = new TVRemoteBridge(this, m_page, m_pageRemote);

    if (!m_browserSettings->hasTVRemote) {
        m_tvRemoteView->hide();
    }

    buildUI();

    // Process window settings
    Qt::WindowFlags flags = this->windowFlags();

    if (!m_browserSettings->hasTitleBar) {
        flags |= Qt::FramelessWindowHint;
        flags &= ~Qt::WindowMinMaxButtonsHint;  // These buttons force the title bar.
    }

    if (m_browserSettings->staysOnTop) {
        flags |= Qt::WindowStaysOnTopHint;
    }

    if (flags != this->windowFlags()) {
        setWindowFlags(flags);
    }

    if (m_browserSettings->startMaximized)
        setWindowState(windowState() | Qt::WindowMaximized);
    else
        resize(width, height);
}

void MainWindow::buildUI()
{
    delete m_navigationBar;
    m_navigationBar = 0;

    if (m_browserSettings->hasMenuBar) {
        createMenuBar();
    }

    m_navigationBar = addToolBar("Navigation");

    if (m_browserSettings->hasBackButton) {
        m_navigationBar->addAction(page()->action(QWebPage::Back));
    }

    if (m_browserSettings->hasStopButton) {
        m_navigationBar->addAction(page()->action(QWebPage::Stop));
    }

    if (m_browserSettings->hasForwardButton) {
        m_navigationBar->addAction(page()->action(QWebPage::Forward));
    }

    if (m_browserSettings->hasReloadButton) {
        QAction* reloadAction = page()->action(QWebPage::Reload);
        connect(reloadAction, SIGNAL(triggered()), this, SLOT(changeLocation()));
        m_navigationBar->addAction(reloadAction);
    }

    if (!m_browserSettings->hasHomeButton) {
        // Not implemented yet. Have at it.
    }

    if (m_browserSettings->hasUrlEdit) {
        m_urlEdit = new LocationEdit(m_navigationBar);
        m_urlEdit->setSizePolicy(QSizePolicy::Expanding, m_urlEdit->sizePolicy().verticalPolicy());
        connect(m_urlEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
        QCompleter* completer = new QCompleter(m_navigationBar);
        m_urlEdit->setCompleter(completer);
        completer->setModel(&m_urlModel);
        m_navigationBar->addWidget(m_urlEdit);

        connect(page()->mainFrame(), SIGNAL(urlChanged(QUrl)), this, SLOT(setAddressUrl(QUrl)));
        connect(page(), SIGNAL(loadProgress(int)), m_urlEdit, SLOT(setProgress(int)));
        connect(page()->mainFrame(), SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
        connect(page()->mainFrame(), SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
    }

    if (!m_browserSettings->hasNavigationBar) {
        m_navigationBar->hide();
    }


    if (m_browserSettings->hasTitleBar) {
        connect(page()->mainFrame(), SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
    }

    // Not sure what this does
    connect(page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));

#ifndef QT_NO_SHORTCUT
    // short-cuts
    page()->action(QWebPage::Back)->setShortcut(QKeySequence::Back);
    page()->action(QWebPage::Stop)->setShortcut(Qt::Key_Escape);
    page()->action(QWebPage::Forward)->setShortcut(QKeySequence::Forward);
    page()->action(QWebPage::Reload)->setShortcut(QKeySequence::Refresh);

    // TODO: Home key shortcut
#endif
}

void MainWindow::createMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("New Window", this, SLOT(newWindow()), QKeySequence::New);
    fileMenu->addAction(tr("Open File..."), this, SLOT(openFile()), QKeySequence::Open);
    fileMenu->addAction(tr("Open Location..."), this, SLOT(openLocation()), QKeySequence(Qt::CTRL | Qt::Key_L));
    fileMenu->addAction("Close Window", this, SLOT(close()), QKeySequence::Close);
    fileMenu->addSeparator();
    fileMenu->addAction("Take Screen Shot...", this, SLOT(screenshot()));
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", QApplication::instance(), SLOT(closeAllWindows()), QKeySequence(Qt::CTRL | Qt::Key_Q));

    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(page()->action(QWebPage::Stop));
    viewMenu->addAction(page()->action(QWebPage::Reload));
    viewMenu->addSeparator();
    QAction* showTVRemote = viewMenu->addAction("TVRemote", this, SLOT(toggleTVRemote(bool)));
    showTVRemote->setCheckable(true);
    showTVRemote->setChecked(m_browserSettings->hasTVRemote);
    QAction* showNavigationBar = viewMenu->addAction("Navigation Bar", this, SLOT(toggleNavigationBar(bool)));
    showNavigationBar->setCheckable(true);
    showNavigationBar->setChecked(m_browserSettings->hasNavigationBar);
}

void MainWindow::home()
{
    // TODO: if RUI list > 1, display list, otherwise display default.

    // Default RUI. What if there is no default RUI and nothing is discovered?

    QString url = m_browserSettings->defaultRUIUrl;
    if (url.size() > 0 ) {
        load(m_browserSettings->defaultRUIUrl);
    }
}

void MainWindow::setPage(RUIWebPage* page)
{
   // if (page && m_page)
   //     page->setUserAgent(m_page->userAgentForUrl(QUrl()));

    delete m_page;
    m_page = page;

    buildUI();
}

RUIWebPage* MainWindow::page() const
{
    return m_page;
}

void MainWindow::setAddressUrl(const QUrl& url)
{
    setAddressUrl(url.toString(QUrl::RemoveUserInfo));
}

void MainWindow::setAddressUrl(const QString& url)
{
    if (m_browserSettings->hasUrlEdit) {
    if (!url.contains("about:"))
        m_urlEdit->setText(url);
    }
}

void MainWindow::addCompleterEntry(const QUrl& url)
{
    QUrl::FormattingOptions opts;
    opts |= QUrl::RemoveScheme;
    opts |= QUrl::RemoveUserInfo;
    opts |= QUrl::StripTrailingSlash;
    QString s = url.toString(opts);
    s = s.mid(2);
    if (s.isEmpty())
        return;

    if (!m_urlList.contains(s))
        m_urlList += s;
    m_urlModel.setStringList(m_urlList);
}

void MainWindow::load(const QString& url)
{
    QUrl qurl = urlFromUserInput(url);
    if (qurl.scheme().isEmpty())
        qurl = QUrl("http://" + url + "/");
    load(qurl);
}

void MainWindow::load(const QUrl& url)
{
    if (!url.isValid())
        return;

    setAddressUrl(url.toString());
    page()->mainFrame()->load(url);
}

QString MainWindow::addressUrl() const
{
    if (m_browserSettings->hasUrlEdit) {
        return m_urlEdit->text();
    } else {
        return QString();
    }
}

void MainWindow::changeLocation()
{
    if (m_browserSettings->hasUrlEdit) {
        QString string = m_urlEdit->text();
        QUrl mainFrameURL = page()->mainFrame()->url();

        if (mainFrameURL.isValid() && string == mainFrameURL.toString()) {
            page()->triggerAction(QWebPage::Reload);
            return;
        }

        load(string);
    }
}

void MainWindow::toggleTVRemote(bool b)
{
    m_browserSettings->hasTVRemote = b;
    m_browserSettings->save();
    b ? m_tvRemoteView->show() : m_tvRemoteView->hide();
}

void MainWindow::toggleNavigationBar(bool b)
{
    m_browserSettings->hasNavigationBar = b;
    m_browserSettings->save();
    b ? m_navigationBar->show() : m_navigationBar->hide();
}

// Don't think we need this
void MainWindow::openLocation()
{
    if (m_browserSettings->hasUrlEdit) {
        m_urlEdit->selectAll();
        m_urlEdit->setFocus();
    }
}

void MainWindow::onIconChanged()
{
    if (m_browserSettings->hasUrlEdit) {
        m_urlEdit->setPageIcon(page()->mainFrame()->icon());
    }
}

void MainWindow::onLoadStarted()
{
#ifndef QT_NO_INPUTDIALOG
    m_urlEdit->setPageIcon(QIcon());
#endif
}

void MainWindow::onTitleChanged(const QString& title)
{
    if (title.isEmpty())
        setWindowTitle(QCoreApplication::applicationName());
    else
        setWindowTitle(QString::fromLatin1("%1 - %2").arg(title).arg(QCoreApplication::applicationName()));
}

/*
bool
MainWindow::eventFilter(QObject* object, QEvent* event)
{
   // the text edit box filters its events through here
  // if (object == m_text_edit_box)
  // {
      if (event->type() == QEvent::KeyPress)

{
         QKeyEvent *key_event = static_cast<QKeyEvent*>(event);

         //if (key_event->matches(QKeySequence::Copy))
        if (key_event->key() == Qt::Key_Space)
        // if (key_event->matches(QKeySequence::Copy))
         {
             ::exit(0);
            // don't do anything and don't pass along event
            return true;
         }
      }
      *//*
      else if (event->type() == QEvent::KeyRelease)
      {
         QKeyEvent *key_event = static_cast<QKeyEvent*>(event);

         if (key_event->matches(QKeySequence::Copy))
         {
            // we only get in here if 'c' is released before ctrl
            callCustomCopy();
            return true;
         }
      }
      *//*
  // }

   // pass along event
   return false;
}
*/


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        ::exit(0);
    }
}
