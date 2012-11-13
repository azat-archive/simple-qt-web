/**
 * Lambda wrapper for Qt connect
 *
 * File: lambda.h
 * Date: 4/15/2012
 *
 * @link http://www.silmor.de/qtstuff.lambda.php
 *
 * @package ad-reger 
 * @author Azat Khuzhin <a3at.mail@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef WRAPPER_LAMBDA_H
#define WRAPPER_LAMBDA_H

#include <functional>
#include <QObject>

typedef std::function<void()> lambdaCallback;

class Lambda : public QObject {
Q_OBJECT

public:
	Lambda(lambdaCallback callback, QObject* parent = NULL);

public slots:
	void call();

private:
	lambdaCallback _callback;
};

#endif // WRAPPER_LAMBDA_H
