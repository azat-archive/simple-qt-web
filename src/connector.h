/**
 * Connector
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef MODULE_CONNECTOR_H
#define MODULE_CONNECTOR_H

#include <QObject>

namespace Module {
	// default timeout for different timers
	const int DEFAULT_TIMEOUT_MSEC = 6000;

	class Connector : public QObject {
	Q_OBJECT

	public:
		Connector(QObject* parent = NULL);
		virtual ~Connector();

	protected:
		// wait signal (async exec), using QEventLoop
		// and after execute finished -> disconnect
		void _connectAndExecFirstSync(QObject *sender, const char *signal, const QObject *receiver, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection, const char* file = NULL, int line = 0) const;
		void _connectAndExecFirstSync(QObject *sender, const char *signal, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection) const;

		// wait signal (async exec), using QEventLoop
		void _connectAndExecSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection, const char* file = NULL, int line = 0) const;
		void _connectAndExecSync(const QObject *sender, const char *signal, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection) const;

		// wait ALL signal (async exec), using QEventLoop
		void _connectAndExecAllSync(const QObject *sender, const char *signal, const QObject *receiver, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection, const char* file = NULL, int line = 0);
		void _connectAndExecAllSync(const QObject *sender, const char *signal, const char *member = NULL, Qt::ConnectionType type = Qt::AutoConnection);

		// wait using timer, for non-blocking GUI
		void _waitWithoutGuiBlock(long long msecs = 1000);
		// wait for bool variable become true using timer, for non-blocking GUI
		void _waitForBoolWithoutGuiBlock(bool& variable, long long msecs = DEFAULT_TIMEOUT_MSEC, long long stepInMsec = 1000);
	};
}

#endif // MODULE_CONNECTOR_H

// pass __FILE__::__LINE__
#define _connectAndExecFirstSyncFile(sender, signal, receiver, member) _connectAndExecFirstSync(sender, signal, receiver, member, Qt::AutoConnection, __FILE__, __LINE__)
#define _connectAndExecSyncFile(sender, signal, receiver, member) _connectAndExecSync(sender, signal, receiver, member, Qt::AutoConnection, __FILE__, __LINE__)
#define _connectAndExecAllSyncFile(sender, signal, receiver, member) _connectAndExecAllSync(sender, signal, receiver, member, Qt::AutoConnection, __FILE__, __LINE__)

