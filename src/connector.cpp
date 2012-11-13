/**
 * Connector
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "connector.h"

#include <QWebFrame>
#include <QPoint>
#include <QMouseEvent>
#include <QWebElementCollection>
#include <QApplication>
#include <QTimer>
#include <QDebug>

namespace Module {
	Connector::Connector(QObject* parent)  : QObject(parent)  {
	}

	Connector::~Connector() {
	}

	void Connector::connectAndExecFirstSync(QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) const {
		connectAndExecSync(sender, signal, receiver, member, type, file, line);
		sender->disconnect(receiver);
	}

	void Connector::connectAndExecFirstSync(QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) const {
		return connectAndExecFirstSync(sender, signal, this, member, type);
	}

	void Connector::connectAndExecSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) const {
		QDebug debugLogger = qDebug() << "connectAndExecSync";
		if (file || line) {
			debugLogger << file << ":" << line;
		}
		debugLogger << sender << "::" << signal << " => " << receiver << "::" << member << " [" << type << "]";

		QEventLoop loop;
		if (member && receiver) {
			loop.connect(sender, signal, receiver, member, type);
		}
		loop.connect(sender, signal, SLOT(quit()), type);
		loop.exec();
	}

	void Connector::connectAndExecSync(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) const {
		return connectAndExecSync(sender, signal, this, member, type);
	}

	void Connector::connectAndExecAllSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) {
		connectAndExecSync(sender, signal, receiver, member, type, file, line);
		waitWithoutGuiBlock();
	}

	void Connector::connectAndExecAllSync(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) {
		return connectAndExecAllSync(sender, signal, this, member, type);
	}

	void Connector::waitWithoutGuiBlock(long long msecs) {
		QTimer timer;
		timer.start(msecs);
		connectAndExecSync(&timer, SIGNAL(timeout()));
	}

	void Connector::waitForBoolWithoutGuiBlock(bool& variable, long long msecs, long long stepInMsec) {
		if (stepInMsec == msecs) {
			waitWithoutGuiBlock(msecs);

			qDebug() << "Bool variable become true after" << msecs << "msecs";
			if (variable) {
				return;
			}
		} else {
			long timerMsec = 0;
			while (timerMsec < msecs) {
				waitWithoutGuiBlock(stepInMsec);
				timerMsec += stepInMsec;

				if (variable) {
					break;
				}

				qDebug() << "Still false, after" << timerMsec << "msecs";
			}

			qDebug() << "Bool variable become true in" << timerMsec << "msecs";
			if (variable) {
				return;
			}
		}

		throw QString("Can't wait for bool in " + QString::number(msecs) + " seconds");
	}
}
