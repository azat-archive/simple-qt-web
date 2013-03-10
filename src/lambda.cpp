/**
 * Lambda wrapper for Qt connect
 *
 * @link http://www.silmor.de/qtstuff.lambda.php
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "lambda.h"

#include <QDebug>

Lambda::Lambda(lambdaCallback callback, QObject* parent) : QObject(parent), _callback(callback) {
}

void Lambda::call() {
	qDebug() << "Execute lambda function : " << (void*)this;
	_callback();
}

