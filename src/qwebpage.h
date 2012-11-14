/**
 * Wrapper for QWebPage
 *
 * @package ad-reger
 * @author Azat Khuzhin <dohardgopro@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef WRAPPER_QWEBPAGE_H
#define WRAPPER_QWEBPAGE_H

#include <QWebPage>
#include <QWebElement>

namespace Wrapper {
	class QWebPage : public ::QWebPage {
	Q_OBJECT

	public:
		static const int OPTIMAL_MSECS_BETWEEN_CHANGE_POSITION = 2;
		static QString userAgent;

		QWebPage(QObject *parent = NULL);

		virtual void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID);
		virtual void javaScriptAlert(QWebFrame *frame, const QString &msg);
		virtual bool javaScriptPrompt(QWebFrame *frame, const QString &msg, const QString &defaultValue, QString *result);
		virtual bool javaScriptConfirm(QWebFrame *frame, const QString &msg);

		virtual QString userAgentForUrl(const QUrl &url) const;

		// This method move mouse cursor to element, with delay between iterations of moving
		//
		// It has relative error, and value of this error depends on difference between diffX and diffY,
		// where diffN = moveTo.x() - moveFrom.x()
		//
		// So don't use it or FIX it for elements with small size
		bool moveMouseTo(QWebElement elementMoveTo, int delay = OPTIMAL_MSECS_BETWEEN_CHANGE_POSITION) const;
	};
}

#endif // WRAPPER_QWEBPAGE_H
