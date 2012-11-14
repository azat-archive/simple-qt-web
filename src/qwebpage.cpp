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
#include <QDesktopWidget>
#include <QApplication>

#include <complex>

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

		QPoint moveFrom = QCursor::pos();
		qDebug() << "Move from (cursor position)" << moveFrom;

		QRect viewFrameGeometry = view()->frameGeometry();
		qDebug() << "View frame geometry" << viewFrameGeometry;

		QRect geometry = elementMoveTo.geometry();
		qDebug() << "Element geometry" << geometry;

		// TODO : some randomize
		QPoint moveTo;
		// Calculate real center, not the upper border
		{
			moveTo = geometry.bottomLeft();
			moveTo.setX(viewFrameGeometry.x() + moveTo.x() + (geometry.width() / 2));
			moveTo.setY(viewFrameGeometry.y() + moveTo.y() + (geometry.height() / 2));
		}
		qDebug() << "Move to" << moveTo;

		Module::Connector connector;

		// default ratio is 1
		float ratioX = 1;
		float ratioY = 1;
		int maxLoopIterations = 0;
		{
			int xDiff = std::abs(moveTo.x() - moveFrom.x());
			int yDiff = std::abs(moveTo.y() - moveFrom.y());

			if (xDiff < yDiff) {
				ratioY = int((yDiff / xDiff) + 0.5);
			} else {
				ratioX = int((xDiff / yDiff) + 0.5);
			}

			maxLoopIterations = std::max(xDiff, yDiff);

			qDebug() << "Diff" << "x:" << xDiff << "y:" << yDiff;
			qDebug() << "Max loop interations" << maxLoopIterations;
		}
		qDebug() << "Ratio" << "x:" << ratioX << "y:" << ratioY;

		// Move
		for (int i = 0; (i < maxLoopIterations) || (moveFrom != moveTo); ++i) {
			if (moveTo.x() != moveFrom.x()) {
				moveFrom.setX((moveFrom.x() > moveTo.x()) ? (moveFrom.x() - ratioX) : (moveFrom.x() + ratioX));
			}

			if (moveTo.y() != moveFrom.y()) {
				moveFrom.setY((moveFrom.y() > moveTo.y()) ? (moveFrom.y() - ratioY) : (moveFrom.y() + ratioY));
			}

			QCursor::setPos(moveFrom);

			connector.waitWithoutGuiBlock(delay);
		}

		return true;
	}
}

