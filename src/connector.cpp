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

	void Connector::_connectAndExecFirstSync(QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) const {
		_connectAndExecSync(sender, signal, receiver, member, type, file, line);
		sender->disconnect(receiver);
	}

	void Connector::_connectAndExecFirstSync(QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) const {
		return _connectAndExecFirstSync(sender, signal, this, member, type);
	}

	void Connector::_connectAndExecSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) const {
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

	void Connector::_connectAndExecSync(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) const {
		return _connectAndExecSync(sender, signal, this, member, type);
	}

	void Connector::_connectAndExecAllSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member, Qt::ConnectionType type, const char* file, int line) {
		_connectAndExecSync(sender, signal, receiver, member, type, file, line);
		_waitWithoutGuiBlock();
	}

	void Connector::_connectAndExecAllSync(const QObject *sender, const char *signal, const char *member, Qt::ConnectionType type) {
		return _connectAndExecAllSync(sender, signal, this, member, type);
	}

	void Connector::_waitWithoutGuiBlock(long long msecs) {
		QTimer timer;
		timer.start(msecs);
		_connectAndExecSync(&timer, SIGNAL(timeout()));
	}

	void Connector::_waitForBoolWithoutGuiBlock(bool& variable, long long msecs, long long stepInMsec) {
		if (stepInMsec == msecs) {
			_waitWithoutGuiBlock(msecs);

			qDebug() << "Bool variable become true after" << msecs << "msecs";
			if (variable) {
				return;
			}
		} else {
			long timerMsec = 0;
			while (timerMsec < msecs) {
				_waitWithoutGuiBlock(stepInMsec);
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
