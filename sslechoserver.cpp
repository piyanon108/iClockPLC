/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "sslechoserver.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

QT_USE_NAMESPACE

//! [constructor]
SslEchoServer::SslEchoServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(nullptr)
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("SSL Echo Server"),
                                              QWebSocketServer::SecureMode,
                                              this);
    QSslConfiguration sslConfiguration;
    QFile certFile(QStringLiteral("/etc/cert/cert.pem"));
    QFile keyFile(QStringLiteral("/etc/cert/key.pem"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    m_pWebSocketServer->setSslConfiguration(sslConfiguration);

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "SSL Echo Server listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &SslEchoServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::sslErrors,
                this, &SslEchoServer::onSslErrors);
    }
}
//! [constructor]

SslEchoServer::~SslEchoServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

//! [onNewConnection]
void SslEchoServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &SslEchoServer::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &SslEchoServer::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &SslEchoServer::socketDisconnected);

    m_clients << pSocket;

    if (clientNum <= 0)
    {
        clientNum = m_clients.length();
        emit onNumClientChanged(clientNum);
    }
    else {
        clientNum = m_clients.length();
    }
}
//! [onNewConnection]

//! [processTextMessage]
void SslEchoServer::processTextMessage(QString message)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    commandProcess(message, pSender);
}
//! [processTextMessage]

void SslEchoServer::commandProcess(QString message, QWebSocket *pSender){
//    message = message.replace(" ","");
    QJsonDocument d = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject command = d.object();
    QString getCommand =  QJsonValue(command["menuID"]).toString();
//    qDebug() << "getCommand" << getCommand << message;

    if (message.contains("web:channel"))
    {
        emit getChannelList(pSender);
    }
    else if (getCommand.contains("getCtrlinfoPage"))
    {
        int webchannelID = QJsonValue(command["channelID"]).toInt();
        emit getCtrlinfoPage(pSender, webchannelID);
    }
    else if (getCommand.contains("channelConfig"))
    {
        int webchannelID = QJsonValue(command["channelID"]).toInt();
        QString radioA_IPAddr   = QJsonValue(command["radioA_IPAddr"]).toString();
        QString radioB_IPAddr   = QJsonValue(command["radioB_IPAddr"]).toString();
        bool defaultMainRadio   = QJsonValue(command["defaultMainRadio"]).toInt() != 0;
        bool mainVCS            = QJsonValue(command["mainVCS"]).toInt() != 0;
        bool snmpEnable         = QJsonValue(command["snmpEnable"]).toInt() != 0;
        bool radioModeTxRx      = QJsonValue(command["radioModeTxRx"]).toInt() != 0;
        int schedulerIndex      = QJsonValue(command["schedulerIndex"]).toInt();
        QString channelName     = QJsonValue(command["channelName"]).toString();
        emit updateChannelConf(radioA_IPAddr,radioB_IPAddr,radioModeTxRx,defaultMainRadio,mainVCS,snmpEnable,schedulerIndex, webchannelID, channelName);
    }
    else if (getCommand.contains("updateScheduler"))
    {
        int value = QJsonValue(command["schedulerIndex"]).toInt();
        int channelID = QJsonValue(command["channelID"]).toInt();
        emit updateScheduler(value,channelID);
    }

    else if (getCommand.contains("updateNTPServer"))
    {
        QString value = QJsonValue(command["ntpServer"]).toString();
        emit updateNTPServer(value);
    }
    else if (getCommand.contains("updateTime"))
    {
        QString value = QJsonValue(command["dateTime"]).toString();
        emit setcurrentDatetime(value);
    }
    else if (getCommand.contains("setLocation"))
    {
        QString value = QJsonValue(command["location"]).toString();
        emit setLocation(value);
    }
    else if (getCommand.contains("updateFirmware"))
    {
        emit updateFirmware();
    }
    else if (getCommand.contains("toggleGpioOut"))
    {
        int gpioNum = QJsonValue(command["gpioNum"]).toInt();
        int gpioVal = QJsonValue(command["gpioVal"]).toInt();
        emit toggleGpioOut(gpioNum, gpioVal);
    }
    else if (getCommand.contains("updateLocalNetwork"))
    {
        uint8_t dhcpmethod =  QJsonValue(command["dhcpmethod"]).toString() == "on";
        QString ipaddress =  QJsonValue(command["ipaddress"]).toString();
        QString subnet =  QJsonValue(command["subnet"]).toString();
        QString gateway =  QJsonValue(command["gateway"]).toString();
        QString pridns =  QJsonValue(command["pridns"]).toString();
        QString secdns =  QJsonValue(command["secdns"]).toString();
        QString phyNetworkName = QJsonValue(command["phyNetworkName"]).toString();
        qDebug() << "updateLocalNetwork" << phyNetworkName << dhcpmethod << ipaddress << subnet << gateway << pridns << secdns;
        emit updateNetwork(dhcpmethod, ipaddress, subnet, gateway, pridns, secdns, phyNetworkName);
    }
    else if (getCommand.contains("GPS_Data"))
    {
        uint16_t GPSD_Port =  QJsonValue(command["GPSD_Port"]).toInt();
        QString GPS_Date =  QJsonValue(command["GPS_Date"]).toString();
        QString GPS_Time =  QJsonValue(command["GPS_Time"]).toString();
        double GPS_Lat =  QJsonValue(command["GPS_Lat"]).toDouble();
        double GPS_Long =  QJsonValue(command["GPS_Long"]).toDouble();
        double GPS_Alt =  QJsonValue(command["GPS_Alt"]).toDouble();
        int GPS_Sat =  QJsonValue(command["GPS_Sat"]).toInt();
        int GPS_SatUse =  QJsonValue(command["GPS_SatUse"]).toInt();
        emit onGPSDataChenged(GPSD_Port, GPS_Date, GPS_Time, GPS_Lat, GPS_Long, GPS_Alt, GPS_Sat, GPS_SatUse,true,message);
        Q_FOREACH (QWebSocket *webClient, m_WebSocketClients)
        {
            webClient->sendTextMessage(message);
        }
    }
    else if (getCommand.contains("Waitting"))
    {
        uint16_t GPSD_Port =  QJsonValue(command["GPSD_Port"]).toInt();
        emit onGPSDataChenged(GPSD_Port, "", "", 0, 0, 0, 0, 0,false,message);
        Q_FOREACH (QWebSocket *webClient, m_WebSocketClients)
        {
            webClient->sendTextMessage(message);
        }
    }
    else if (getCommand.contains("restartnetwork"))
    {
        QString phyNetworkName = QJsonValue(command["phyNetworkName"]).toString();
        emit restartnetwork(phyNetworkName);
    }
    else if (getCommand.contains("rebootSystem"))
    {
        emit rebootSystem();
    }
    else if (getCommand.contains("setgps_voltage"))
    {
        int gpsnum = QJsonValue(command["gpsnum"]).toInt();
        int voltage  = QJsonValue(command["voltage"]).toInt();
        emit setgps_voltage(gpsnum,voltage);
    }
    else if (getCommand.contains("getMonitorPage"))
    {
        m_WebSocketClients << pSender;
        emit getMonitorPage(pSender);
    }
    else if (getCommand.contains("getNetworkPage"))
    {
        m_WebSocketClients << pSender;
        emit getNetworkPage(pSender);
    }
    else if (getCommand.contains("getSystemPage"))
    {
        m_WebSocketClients << pSender;
        emit getSystemPage(pSender);
    }
    else if (getCommand.contains("getNtpPage"))
    {
        m_WebSocketClients << pSender;
        emit getNtpPage(pSender);
    }
    else if (getCommand.contains("deleteServer"))
    {
        QString address = QJsonValue(command["address"]).toString();
        emit deleteServer(address);
    }
    else if (getCommand.contains("addNewServer"))
    {
        QString address = QJsonValue(command["address"]).toString();
        QString ServerPoolPeer = QJsonValue(command["ServerPoolPeer"]).toString();
        QString option = QJsonValue(command["option"]).toString();
        emit addServerPoolPeer(ServerPoolPeer, address, option);
    }
    else if (getCommand.contains("getCtrlinfoPage"))
    {
        bool hasConnect = false;
        Q_FOREACH (QWebSocket *webClient, m_WebSocketClients)
        {
            if (webClient == pSender){
                hasConnect = true;
                break;
            }
        }
        if (!hasConnect)
            m_WebSocketClients << pSender;

        int idInRole = QJsonValue(command["idInRole"]).toInt();
        emit getCtrlInfoPage(idInRole, pSender);
    }
    else {
        emit onNewMessage(message);
        qDebug() << "getCommand" << getCommand << message;
    }
}

void SslEchoServer::processMessage(QString message)
{
//    qDebug() << message;
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    commandProcess(message, pSender);

}

//! [processBinaryMessage]
void SslEchoServer::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        pClient->sendBinaryMessage(message);
    }
}
//! [processBinaryMessage]

//! [socketDisconnected]
void SslEchoServer::socketDisconnected()
{
    qDebug() << "Client disconnected";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void SslEchoServer::onSslErrors(const QList<QSslError> &)
{
    qDebug() << "Ssl errors occurred";
}
//! [socketDisconnected]

void SslEchoServer::broadcastMessage(QString message){
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
