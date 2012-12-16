// 
// Simple QtWeb
//

#include <QtGui/QApplication>
#include <QObject>

#include <QDebug>

#include <QWebView>
#include <QWebFrame>
#include <QUrl>
#include <QWebElement>
#include <QPoint>
#include <QRect>
#include <QMouseEvent>
#include <QCursor>
#include <QStringList>
#include <QNetworkProxy>
#include <QSocketNotifier>
#include <QTextStream>

#include <memory>

#include <signal.h>
#include <unistd.h> // STDIN_FILENO

#include "lambda.h"
#include "qwebpage.h"

std::unique_ptr<QWebView> view;
std::unique_ptr<QSocketNotifier> stdinNotifier;

struct Options {
	bool maximize;
	QString url; // default url is "google.com"
	QString userAgent; // default is that using in ga-kw
	QString socks;
	QString moveToElementPrefix;
	QString clickToElementPrefix;
	QString cursorSetToElementPrefix;
	bool socksResolver;
};
Options options = {
	false,
	QString("google.com"),
	QString("Mozilla/5.0 (X11; U; Linux i686;) Gecko/20110101 Firefox/3.6.13"),
	QString(""),
	QString("Move to "),
	QString("Click to "),
	QString("Cursor to "),
	false,
};

QStringList getCliArguments(int argc, char** argv) {
	QStringList cliArguments;

	for (int i = 0; i < argc; ++i) {
		cliArguments << QString::fromLocal8Bit(argv[i]);
	}

	return cliArguments;
}

int main(int argc, char** argv) {
	// Parse options
	// TODO: http://api.kde.org/4.x-api/kdelibs-apidocs/kdecore/html/classKCmdLineArgs.html
	{
		QStringList arguments = getCliArguments(argc, argv);
		if ((arguments.indexOf("-h") != -1) || (arguments.indexOf("--help") != -1)) {
			qDebug() << "QtWeb options:";
			qDebug() << "";
			qDebug() << "--maximize";
			qDebug() << "";
			qDebug() << "--url";
			qDebug() << "--user-agent";
			qDebug() << "--socks host:port";
			qDebug() << "--socks-resolver";

			return EXIT_SUCCESS;
		}

		int optionIndex = 0;

		// maximize 
		if ((optionIndex = arguments.indexOf("--maximize")) != -1) {
			options.maximize = true;
		}
		// url
		if ((optionIndex = arguments.indexOf("--url")) != -1) {
			options.url = arguments.at(optionIndex + 1);
		}
		// user agent
		if ((optionIndex = arguments.indexOf("--user-agent")) != -1) {
			options.userAgent = arguments.at(optionIndex + 1);
			Wrapper::QWebPage::userAgent = options.userAgent;
		}
		// socks
		if ((optionIndex = arguments.indexOf("--socks")) != -1) {
			options.socks = arguments.at(optionIndex + 1);
		}
		// socks resolver
		if ((optionIndex = arguments.indexOf("--socks-resolver")) != -1) {
			options.socksResolver = true; 
		}

		qDebug() << "Maximize" << options.maximize;
		qDebug() << "Load" << options.url;
		qDebug() << "User-agent" << options.userAgent;
		qDebug() << "Socks" << options.socks;
		qDebug() << "Socks resolver" << options.socksResolver;
	}

	QApplication a(argc, argv);

	// release view
	Lambda qHandler([&]() {
		view.reset();
	});
	QObject::connect(&a, SIGNAL(aboutToQuit()), &qHandler, SLOT(call()));
	
	view.reset(new QWebView);
	view->setPage(new Wrapper::QWebPage);

	// add "http://"
	if (options.url.indexOf("://") == -1) {
		options.url = QLatin1String("http://") + options.url;
	}

	// socks
	if (options.socks.length()) {
		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::Socks5Proxy);

		int colonPosition = options.socks.lastIndexOf(":");
		if (colonPosition == -1) {
			qDebug() << "Malformed proxy (HOST:PORT is needed)";
			return EXIT_FAILURE;
		}

		proxy.setHostName(options.socks.left(colonPosition));
		proxy.setPort(options.socks.right(options.socks.length() - 1 - colonPosition).toInt());

		if (options.socksResolver) {
			proxy.setCapabilities(proxy.capabilities() | QNetworkProxy::HostNameLookupCapability);
		} else {
			proxy.setCapabilities(proxy.capabilities() & ~QNetworkProxy::HostNameLookupCapability);
		}

		view->page()->networkAccessManager()->setProxy(proxy);
		
		qDebug() << "Proxy installed";
	}

	view->load(QUrl(options.url));
	if (options.maximize) {
		view->showMaximized();
	} else {
		view->show();
	}

	// execute JS
	{
		// Reinterpret pointer to Wrapper class
		Wrapper::QWebPage *page = reinterpret_cast<Wrapper::QWebPage *>(view->page());

		// async read from STDIN
		// and eval this as JS code
		stdinNotifier.reset(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read));
		QObject::connect(&(*stdinNotifier), SIGNAL(activated(int)), new Lambda([&] {
			qDebug() << Q_FUNC_INFO;

			QTextStream stream(stdin, QIODevice::ReadOnly);
			QString jsToExecute;

			forever {
				fd_set stdinfd;
				FD_ZERO( &stdinfd );
				FD_SET( STDIN_FILENO, &stdinfd );
				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				int ready = select(1, &stdinfd, NULL, NULL, &tv);

				if (ready > 0) {
					jsToExecute += stream.readLine();
				} else {
					break;
				}
			}

			// TODO : drop code duplicating
			if (jsToExecute.startsWith(options.moveToElementPrefix)) {
				QString moveMouseToElementSelector = jsToExecute.remove(0, options.moveToElementPrefix.length());

				page->moveMouseTo(page->mainFrame()->findFirstElement(moveMouseToElementSelector));
			} else if (jsToExecute.startsWith(options.clickToElementPrefix)) {
				QString clickMouseToElementSelector = jsToExecute.remove(0, options.clickToElementPrefix.length());

				page->clickTo(page->mainFrame()->findFirstElement(clickMouseToElementSelector));
			} else if (jsToExecute.startsWith(options.cursorSetToElementPrefix)) {
				QString setCursorToElementSelector = jsToExecute.remove(0, options.cursorSetToElementPrefix.length());

				page->setCursorTo(page->mainFrame()->findFirstElement(setCursorToElementSelector));
			} else {
				view->page()->mainFrame()->evaluateJavaScript(jsToExecute);	
			}
		}), SLOT(call()));
		stdinNotifier->setEnabled(true);
	}

	return a.exec();
}

