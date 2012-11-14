/**
 * Wrapper for QWebPage
 *
 * @package ad-reger
 * @author Azat Khuzhin <dohardgopro@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "qwebpage.h"
#include "connector.h"

#include <QDebug>

namespace Wrapper {
	QString QWebPage::userAgent = "qt";

	QWebPage::QWebPage(QObject *parent) : ::QWebPage(parent) {
		// Set cache settings
		settings()->setMaximumPagesInCache(0);
		settings()->setObjectCacheCapacities(0,0,0);
	}

	void QWebPage::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID) {
		qDebug() << message << " @ no. " << lineNumber << " sourceid: " << sourceID;
	}

	void QWebPage::javaScriptAlert(QWebFrame *frame, const QString &msg) {
		Q_UNUSED(frame);

		qDebug() << msg;
	}

	bool QWebPage::javaScriptPrompt(QWebFrame *frame, const QString &msg, const QString &defaultValue, QString *result) {
		Q_UNUSED(frame);

		qDebug() << msg << " @ default is '" << defaultValue << "' : " << (result ? *result : "canceled");

		return result == NULL;
	}

	bool QWebPage::javaScriptConfirm(QWebFrame *frame, const QString &msg) {
		Q_UNUSED(frame);

		qDebug() << msg;

		return msg == "";
	}

	QString QWebPage::userAgentForUrl(const QUrl &url) const {
		Q_UNUSED(url);

		return userAgent; 
	}

	bool QWebPage::moveMouseTo(QWebElement elementMoveTo, int delay) const {
		if (elementMoveTo.isNull()) {
			return false;
		}

		qDebug() << "Cursor position" << QCursor::pos();

		// TODO : add detecting global position, not position on current monitor
		QRect geometry = elementMoveTo.geometry();
		// TODO : some randomize
		QPoint moveTo;
		// Calculate real center, not the upper border
		{
			moveTo = geometry.bottomLeft();
			moveTo.setX(moveTo.x() + (geometry.width() / 2));
			moveTo.setY(moveTo.y() + (geometry.height() / 2));
		}
		QPoint moveFrom = QCursor::pos();

		Module::Connector connector;

		// TODO : move more granular
		// Move
		while (moveFrom != moveTo) {
			if (moveTo.x() != moveFrom.x()) {
				moveFrom.setX((moveFrom.x() > moveTo.x()) ? (moveFrom.x() - 1) : (moveFrom.x() + 1));
			}

			if (moveTo.y() != moveFrom.y()) {
				moveFrom.setY((moveFrom.y() > moveTo.y()) ? (moveFrom.y() - 1) : (moveFrom.y() + 1));
			}

			QCursor::setPos(moveFrom);

			connector.waitWithoutGuiBlock(delay);
		}

		return true;
	}
}

