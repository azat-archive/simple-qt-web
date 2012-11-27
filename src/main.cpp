// 
// Simple QtWeb
//
// Also it read STDIN and execute that as JS code
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

// settings
#define MOVE
#define CLICK // try to click by element, that we track 

std::unique_ptr<QWebView> view;
std::unique_ptr<QSocketNotifier> stdinNotifier;

struct Options {
	bool maximize;
	QString url; // default url is "google.com"
	QString element; // default element selector to find
	QString userAgent; // default is that using in ga-kw
	QString socks;
	QString moveElementPrefix;
	bool socksResolver;
};
Options options = {
	false,
	QString("google.com"),
	QString("input[type=submit]"),
	QString("Mozilla/5.0 (X11; U; Linux i686;) Gecko/20110101 Firefox/3.6.13"),
	QString(""),
	QString("Move to "),
	false,
};

// handler
void handler(const int num) {
	qDebug() << "Signal" << num;

	QPoint scroll = view->page()->mainFrame()->scrollPosition();
	QWebElement el = view->page()->mainFrame()->findFirstElement(options.element);
	QRect geom = el.geometry();
	QPoint center = geom.center();

	qDebug() << "Scroll position" << scroll;

	qDebug() << "Element position before" << geom;
	qDebug() << "Element position center before" << center;

#ifdef MOVE
	qDebug() << "Move";

	int newX = center.x() - scroll.x();
	int newY = center.y() - scroll.y();
	qDebug() << "Need to move, x:" << newX << ", y:" << newY;

	// move element
	center.setX(newX);
	center.setY(newY);

	qDebug() << "Element position after" << geom;
	qDebug() << "Element position center after" << center;
#endif

#ifdef CLICK
	qDebug() << "Click";
	QCursor::setPos(newX, newY);

	QMouseEvent press(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(view.get(), &press); 
	QMouseEvent release(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(view.get(), &release);
#endif

	qDebug() << "";
}
// \handler

int main(int argc, char** argv) {
	{
		struct sigaction action;
		action.sa_handler = handler;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		sigaction(SIGINT, &action, NULL);
	}

	QApplication a(argc, argv);

	// Parse options
	// TODO: http://api.kde.org/4.x-api/kdelibs-apidocs/kdecore/html/classKCmdLineArgs.html
	{
		auto arguments = a.arguments();
		if ((arguments.indexOf("-h") != -1) || (arguments.indexOf("--help") != -1)) {
			qDebug() << "QtWeb options:";
			qDebug() << "";
			qDebug() << "--maximize";
			qDebug() << "";
			qDebug() << "--url";
			qDebug() << "--element selector-to-track";
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
		// element to track
		if ((optionIndex = arguments.indexOf("--element")) != -1) {
			options.element = arguments.at(optionIndex + 1);
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
		qDebug() << "Element" << options.element;
		qDebug() << "Load" << options.url;
		qDebug() << "User-agent" << options.userAgent;
		qDebug() << "Socks" << options.socks;
		qDebug() << "Socks resolver" << options.socksResolver;
	}

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

			if (jsToExecute.startsWith(options.moveElementPrefix)) {
				QString moveMouseToElementSelector = jsToExecute.remove(0, options.moveElementPrefix.length());

				// Reinterpret pointer to Wrapper class
				Wrapper::QWebPage *page = reinterpret_cast<Wrapper::QWebPage *>(view->page());
				page->moveMouseTo(page->mainFrame()->findFirstElement(moveMouseToElementSelector));
			} else {
				view->page()->mainFrame()->evaluateJavaScript(jsToExecute);	
			}
		}), SLOT(call()));
		stdinNotifier->setEnabled(true);
	}

	return a.exec();
}

