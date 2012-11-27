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
#include <QWebFrame>
#include <QMouseEvent>
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
			qDebug() << "Geometry bottom left" << moveTo;

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

			// We already at this element
			if ((xDiff == 0) && (yDiff == 0)) {
				return true;
			}

			if (xDiff < yDiff) {
				ratioY = xDiff ? ((float)yDiff / (float)xDiff) : 0;
			} else {
				ratioX = yDiff ? ((float)xDiff / (float)yDiff) : 0;
			}

			maxLoopIterations = ratioX ? (xDiff / ratioX) : (yDiff / ratioY);

			qDebug() << "Diff" << "x:" << xDiff << "y:" << yDiff;
			qDebug() << "Max loop interations" << maxLoopIterations;
		}
		qDebug() << "Ratio" << "x:" << ratioX << "y:" << ratioY;

		// Present moveFrom as float, for add floating ratio to it.
		struct FloatPoint {
			float x;
			float y;

			FloatPoint(const QPoint &a) {
				x = a.x();
				y = a.y();
			}

			bool operator ==(const FloatPoint &a) {
				return ((a.x == this->x) && (a.y == this->y));
			}
		} floatMoveFromPoint(moveFrom);

		// Move
		for (int i = 0; ((moveFrom != moveTo /* Just in case */) && (i < maxLoopIterations)); ++i) {
			if (moveTo.x() != moveFrom.x()) {
				moveFrom.setX((moveFrom.x() > moveTo.x()) ? (floatMoveFromPoint.x -= ratioX) : (floatMoveFromPoint.x += ratioX));
			}

			if (moveTo.y() != moveFrom.y()) {
				moveFrom.setY((moveFrom.y() > moveTo.y()) ? (floatMoveFromPoint.y -= ratioY) : (floatMoveFromPoint.y += ratioY));
			}

			QCursor::setPos(moveFrom);

			connector.waitWithoutGuiBlock(delay);
		}

		return true;
	}

	bool QWebPage::clickTo(QWebElement elementClickTo) const {
		if (elementClickTo.isNull()) {
			return false;
		}

		// TODO[place1] : drop code duplicate 
		QPoint scroll = mainFrame()->scrollPosition();
		QRect geom = elementClickTo.geometry();
		QPoint center = geom.center();

		qDebug() << "Scroll position" << scroll;
		qDebug() << "Geometry before" << geom;
		qDebug() << "Geometry center before" << center;

		qDebug() << "Click";

		int newX = center.x() - scroll.x();
		int newY = center.y() - scroll.y();

		qDebug() << "Need set cursor position to, x:" << newX << ", y:" << newY;
		QCursor::setPos(newX, newY);

		QMouseEvent press(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
		QApplication::sendEvent(view(), &press);
		QMouseEvent release(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
		QApplication::sendEvent(view(), &release);

		return true;
	}

	bool QWebPage::setCursorTo(QWebElement elementSetCursorTo) const {
		if (elementSetCursorTo.isNull()) {
			return false;
		}
		
		// TODO[place2] : drop code duplicate 
		QPoint scroll = mainFrame()->scrollPosition();
		QRect geom = elementSetCursorTo.geometry();
		QPoint center = geom.center();

		qDebug() << "Scroll position" << scroll;
		qDebug() << "Geometry before" << geom;
		qDebug() << "Geometry center before" << center;

		qDebug() << "Move";

		int newX = center.x() - scroll.x();
		int newY = center.y() - scroll.y();
		qDebug() << "Need to move, x:" << newX << ", y:" << newY;

		// move element
		center.setX(newX);
		center.setY(newY);

		qDebug() << "Geometry after" << geom;
		qDebug() << "Geometry center after" << center;

		return true;
	}
}

