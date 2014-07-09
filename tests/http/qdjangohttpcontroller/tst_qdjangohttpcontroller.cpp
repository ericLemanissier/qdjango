/*
 * Copyright (C) 2010-2014 Jeremy Lainé
 * Contact: https://github.com/jlaine/qdjango
 *
 * This file is part of the QDjango Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include <QDateTime>
#include <QtTest>

#include "QDjangoHttpController.h"
#include "QDjangoHttpRequest.h"
#include "QDjangoHttpRequest_p.h"
#include "QDjangoHttpResponse.h"

/** Test QDjangoHttpController class.
 */
class tst_QDjangoHttpController : public QObject
{
    Q_OBJECT

private slots:
    void testBasicAuth();
    void testDateTime();
    void testServeAuthorizationRequired();
    void testServeBadRequest();
    void testServeInternalServerError();
    void testServeNotFound();
    void testServeRedirect();
    void testServeStatic();
};

void tst_QDjangoHttpController::testBasicAuth()
{
    QDjangoHttpRequest request;

    QString username, password;
    QCOMPARE(QDjangoHttpController::getBasicAuth(request, username, password), false);
    QCOMPARE(username, QString());
    QCOMPARE(password, QString());

    request.d->meta.insert("HTTP_AUTHORIZATION", "Basic bad");
    QCOMPARE(QDjangoHttpController::getBasicAuth(request, username, password), false);
    QCOMPARE(username, QString());
    QCOMPARE(password, QString());

    request.d->meta.insert("HTTP_AUTHORIZATION", "Basic Zm9vOmJhcg==");
    QCOMPARE(QDjangoHttpController::getBasicAuth(request, username, password), true);
    QCOMPARE(username, QString("foo"));
    QCOMPARE(password, QString("bar"));
}

void tst_QDjangoHttpController::testDateTime()
{
    const QDateTime dt(QDate(2014, 7, 14), QTime(11, 22, 33), Qt::UTC);
    QCOMPARE(QDjangoHttpController::httpDateTime(dt), QString("Mon, 14 Jul 2014 11:22:33 GMT"));
    QCOMPARE(QDjangoHttpController::httpDateTime("Mon, 14 Jul 2014 11:22:33 GMT"), dt);
}

void tst_QDjangoHttpController::testServeAuthorizationRequired()
{
    QDjangoHttpRequest request;
    QDjangoHttpResponse *response = QDjangoHttpController::serveAuthorizationRequired(request);
    QCOMPARE(response->statusCode(), 401);
    QCOMPARE(response->header("www-authenticate"), QString("Basic realm=\"Secure Area\""));
    delete response;
}

void tst_QDjangoHttpController::testServeBadRequest()
{
    QDjangoHttpRequest request;
    QDjangoHttpResponse *response = QDjangoHttpController::serveBadRequest(request);
    QCOMPARE(response->statusCode(), 400);
    delete response;
}

void tst_QDjangoHttpController::testServeInternalServerError()
{
    QDjangoHttpRequest request;
    QDjangoHttpResponse *response = QDjangoHttpController::serveInternalServerError(request);
    QCOMPARE(response->statusCode(), 500);
    delete response;
}

void tst_QDjangoHttpController::testServeNotFound()
{
    QDjangoHttpRequest request;
    QDjangoHttpResponse *response = QDjangoHttpController::serveNotFound(request);
    QCOMPARE(response->statusCode(), 404);
    delete response;
}

void tst_QDjangoHttpController::testServeRedirect()
{
    QDjangoHttpRequest request;

    QDjangoHttpResponse *response = QDjangoHttpController::serveRedirect(request, QUrl("/bye"), false);
    QCOMPARE(response->statusCode(), 302);
    QCOMPARE(response->header("location"), QString("/bye"));
    delete response;

    response = QDjangoHttpController::serveRedirect(request, QUrl("/bye"), true);
    QCOMPARE(response->statusCode(), 301);
    QCOMPARE(response->header("location"), QString("/bye"));
    delete response;
}

void tst_QDjangoHttpController::testServeStatic()
{
    QDjangoHttpRequest request;
    QDjangoHttpResponse *response;

    response = QDjangoHttpController::serveStatic(request, ":/not-found");
    QCOMPARE(response->statusCode(), 404);
    delete response;
    
    response = QDjangoHttpController::serveStatic(request, ":/test.css");
    QCOMPARE(response->statusCode(), 200);
    QCOMPARE(response->header("content-type"), QString("text/css"));
    QVERIFY(!response->header("last-modified").isEmpty());
    
    response = QDjangoHttpController::serveStatic(request, ":/test.html");
    QCOMPARE(response->statusCode(), 200);
    QCOMPARE(response->header("content-type"), QString("text/html"));
    QVERIFY(!response->header("last-modified").isEmpty());
    delete response;
}

QTEST_MAIN(tst_QDjangoHttpController)
#include "tst_qdjangohttpcontroller.moc"
