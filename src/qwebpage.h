/**
 * Wrapper for QWebPage
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
		bool moveMouseTo(QWebElement elementMoveTo, int delay = OPTIMAL_MSECS_BETWEEN_CHANGE_POSITION) const;
		bool clickTo(QWebElement elementMoveTo) const;
		bool setCursorTo(QWebElement elementMoveTo) const;
	};
}

#endif // WRAPPER_QWEBPAGE_H
