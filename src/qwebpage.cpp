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
}

