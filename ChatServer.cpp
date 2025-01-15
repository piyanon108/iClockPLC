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
#include "ChatServer.h"
#include <QDateTime>

QT_USE_NAMESPACE

ChatServer::ChatServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(Q_NULLPTR),
    m_clients()
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Chat Server"),
                                              QWebSocketServer::NonSecureMode,
                                              this);
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "Chat Server listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &ChatServer::onNewConnection);
    }
}

ChatServer::~ChatServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void ChatServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &ChatServer::processMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ChatServer::socketDisconnected);
    m_clients << pSocket;
    qDebug() << "On New Connection from address : " << pSocket->peerName();
    if (clientNum <= 0)
    {
        clientNum = m_clients.length();
        emit onNumClientChanged(clientNum);
    }
    else {
        clientNum = m_clients.length();
    }

}
//var obj = JSON.parse('{"menuID":"input", "name":"S4_1", "inputgain":1, "outputgain":10}');
//var obj = JSON.parse('{"menuID":"nodeCfg", "nodeID":2, "nodeType":0, "nodeName":"t6tr1", "ipAddress":"10.45.110.11", "sipPort":"5060", "active":1}');
//var obj = JSON.parse('{"menuID":"connState", "nodeID":2, "connStatus":"Disconnected", "connDuration":"0", "trxStatus":"--"}');
void ChatServer::updateHomeInput(QString localname, uint8_t inputLevel, uint8_t outputLevel, QWebSocket *newClient, uint16_t keepAlivePeroid, uint16_t sipPort, uint8_t portInterface ,uint8_t txScheduler,uint8_t invitemode,float siteTone, int defaultEthernet, int testModeEnable)
{
    QString message = QString("{\"menuID\":\"input\", \"name\":\"%1\", \"sipPort\":\"%2\", \"keepAlivePeroid\":\"%3\", \"inputgain\":%4, \"outputgain\":%5, \"portInterface\":%6, \"pttScheduler\":%7, \"invitemode\":%8, \"siteTone\":%9, \"defaultEthernet\":%10, \"testModeEnable\":%11}")
            .arg(localname).arg(sipPort).arg(keepAlivePeroid).arg(31-inputLevel).arg(outputLevel).arg(portInterface).arg(txScheduler).arg(invitemode).arg(int(siteTone*10)).arg(defaultEthernet).arg(testModeEnable);
    newClient->sendTextMessage(message);
}

void ChatServer::updateCATIS(bool enableRecording, int warningAudioLevelTime, int warningPercentFault, int warningPTTMinute, int backupAudioMin, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"updateCATIS\", \"enableRecording\":%1, \"warningAudioLevelTime\":%2, \"warningPercentFault\":%3, \"warningPTTMinute\":%4, \"backupAudioMin\":%5}")
            .arg(enableRecording).arg(warningAudioLevelTime).arg(warningPercentFault).arg(warningPTTMinute).arg(backupAudioMin);
    newClient->sendTextMessage(message);
}

void ChatServer::updatefrequency(uint8_t nodeID, QString frequency, QString sqlLevel,QString frequencyRx, int rfPower)
{
    QString message = QString("{\"menuID\":\"updatefrequency\", \"nodeID\":%1, \"frequency\":\"%2\", \"sqlLevel\":\"%3\", \"frequencyRx\":\"%4\", \"rfPower\":%5}").arg(nodeID).arg(frequency).arg(sqlLevel).arg(frequencyRx).arg(rfPower);
    broadcastMessage(message);
}

void ChatServer::updateHomeED137Cfg(uint8_t nodeID, uint8_t nodeType, QString nodeName, QString ipAddress, QString sipPort, uint8_t active, QWebSocket *newClient, QString frequency, QString sqlLevel,
                                    QString nodeNameRx, QString ipAddressRx, QString sipPortRx, QString frequencyRx, QString sqlLevelRx, QString channelName, int rfPower)
{
    QString message = QString("{\"menuID\":\"nodeCfg\", \"nodeID\":%1, \"nodeType\":%2, \"nodeName\":\"%3\", \"ipAddress\":\"%4\", \"sipPort\":\"%5\", \"active\":%6, \"frequency\":\"%7\", \"sqlLevel\":\"%8\""
                              ", \"nodeNameRx\":\"%9\", \"ipAddressRx\":\"%10\", \"sipPortRx\":\"%11\", \"frequencyRx\":\"%12\", \"sqlLevelRx\":\"%13\", \"channelName\":\"%14\", \"rfPower\":%15}"
                              ).arg(nodeID).arg(nodeType).arg(nodeName).arg(ipAddress).arg(sipPort).arg(active).arg(frequency).arg(sqlLevel).arg(nodeNameRx).arg(ipAddressRx).arg(sipPortRx).arg(frequencyRx).arg(sqlLevelRx).arg(channelName).arg(rfPower);
    newClient->sendTextMessage(message);
}

void ChatServer::updateHomeNodeState(uint8_t nodeID, QString conn, QString duration, QString durationRx, QString trxStatus, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"connState\", \"nodeID\":%1, \"connStatus\":\"%2\", \"connDuration\":\"%3\", \"connDuration\":\"%4\", \"trxStatus\":\"%5\"}")
            .arg(nodeID).arg(conn).arg(duration).arg(durationRx).arg(trxStatus);
    newClient->sendTextMessage(message);
}
void ChatServer::updateAllowedUri(uint8_t numConn, QString uri1, QString uri2, QString uri3, QString uri4, QString uri5, QString uri6, QString uri7, QString uri8, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"uriAllowedList\", \"numConn\":%1, \"uri1\":\"%2\", \"uri2\":\"%3\", \"uri3\":\"%4\", \"uri4\":\"%5\", \"uri5\":\"%6\", \"uri6\":\"%7\", \"uri7\":\"%8\", \"uri8\":\"%9\"}").arg(numConn).arg(uri1).arg(uri2).arg(uri3).arg(uri4).arg(uri5).arg(uri6).arg(uri7).arg(uri8);
    newClient->sendTextMessage(message);
}
void ChatServer::updateClientConnStatus(uint8_t connNum, QString TxRxStatus, QString lastPtt, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"connStatus\", \"connNum\":%1, \"TxRx\":\"%2\", \"pttURI\":\"%3\"}").arg(connNum).arg(TxRxStatus).arg(lastPtt);
    newClient->sendTextMessage(message);
}
void ChatServer::updateNetworkInfo(uint8_t dhcpmethod, QString ipaddress, QString subnet, QString gateway, QString pridns, QString secdns, QString ntpServer, QWebSocket *newClient, QString phyNetworkName)
{
    QString message = QString("{\"menuID\":\"network\", \"dhcpmethod\":%1, \"ipaddress\":\"%2\", \"subnet\":\"%3\", \"gateway\":\"%4\", \"pridns\":\"%5\", \"secdns\":\"%6\", \"ntpServer\":\"%7\", \"phyNetworkName\":\"%8\"}")
            .arg(dhcpmethod).arg(ipaddress).arg(subnet).arg(gateway).arg(pridns).arg(secdns).arg(ntpServer).arg(phyNetworkName);
    newClient->sendTextMessage(message);
}
void ChatServer::updateSystemInfo(QString SwVersion, QString HwVersion,bool ntp,QString ntpServer,QString timeLocation,QWebSocket *newClient)
{
    int dateTimeMethod;
    if (ntp){
        dateTimeMethod = 1;
    }else{
        dateTimeMethod = 2;
    }

    QString message = QString("{\"menuID\":\"system\", \"SwVersion\":\"%1\", \"HwVersion\":\"%2\", \"dateTimeMethod\":\"%3\", \"ntpServer\":\"%4\", \"location\":\"%5\"}")
            .arg(SwVersion).arg(HwVersion).arg(dateTimeMethod).arg(ntpServer).arg(timeLocation);
    newClient->sendTextMessage(message);
}

void ChatServer::broadcastMessageNodeState(uint8_t nodeID, QString conn, QString duration, QString trxStatus, QString radioStatus, QString vswr, QString durationRx, QString connRx, QString radioStatusRx){
    QString message = QString("{\"menuID\":\"connState\", \"nodeID\":%1, \"connStatus\":\"%2\", \"connDuration\":\"%3\", \"trxStatus\":\"%4\", \"radioStatus\":\"%5\", \"vswr\":\"%6\", \"connDurationRx\":\"%7\", \"connStatusRx\":\"%8\", \"radioStatusRx\":\"%9\"}").arg(nodeID).arg(conn).arg(duration).arg(trxStatus).arg(radioStatus).arg(vswr).arg(durationRx).arg(connRx).arg(radioStatusRx);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastMessageStatus(uint8_t connNum, QString TxRxStatus, QString lastPtt)
{
    QString message = QString("{\"menuID\":\"connStatus\", \"connNum\":%1, \"TxRx\":\"%2\", \"pttURI\":\"%3\"}").arg(connNum).arg(TxRxStatus).arg(lastPtt);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::updateGpioInStatus(uint8_t gpio1Val, uint8_t gpio2Val, uint8_t gpio3Val, uint8_t gpio4Val, uint8_t gpio1OutVal, uint8_t gpio2OutVal, uint8_t gpio3OutVal, uint8_t gpio4OutVal, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"GpioInStatus\", \"gpio1Val\":%1, \"gpio2Val\":%2, \"gpio3Val\":%3, \"gpio4Val\":%4, \"gpio1OutVal\":%5, \"gpio2OutVal\":%6, \"gpio3OutVal\":%7, \"gpio4OutVal\":%8}")
            .arg(gpio1Val).arg(gpio2Val).arg(gpio3Val).arg(gpio4Val).arg(gpio1OutVal).arg(gpio2OutVal).arg(gpio3OutVal).arg(gpio4OutVal);
    newClient->sendTextMessage(message);
}
void ChatServer::broadcastGpioInStatus(uint8_t gpio1Val,uint8_t gpio2Val, uint8_t gpio3Val, uint8_t gpio4Val,uint8_t gpio1OutVal,uint8_t gpio2OutVal, uint8_t gpio3OutVal, uint8_t gpio4OutVal)
{
    QString message = QString("{\"menuID\":\"GpioInStatus\", \"gpio1Val\":%1, \"gpio2Val\":%2, \"gpio3Val\":%3, \"gpio4Val\":%4, \"gpio1OutVal\":%5, \"gpio2OutVal\":%6, \"gpio3OutVal\":%7, \"gpio4OutVal\":%8}")
            .arg(gpio1Val).arg(gpio2Val).arg(gpio3Val).arg(gpio4Val).arg(gpio1OutVal).arg(gpio2OutVal).arg(gpio3OutVal).arg(gpio4OutVal);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::updateSqlDefeat(int sqlDefeat, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"sqlDefeat\", \"sqlDefeat\":\"%2\"}").arg(sqlDefeat);
    newClient->sendTextMessage(message);
}
void ChatServer::updateURIConnList(QString uriConnList, QWebSocket *newClient)
{
    QString message = QString("{\"menuID\":\"uriConnList\", \"listURI\":\"%1\"}").arg(uriConnList);
    newClient->sendTextMessage(message);
}
void ChatServer::broadcastURIConnList(QString uriConnList)
{
    QString message = QString("{\"menuID\":\"uriConnList\", \"listURI\":\"%1\"}").arg(uriConnList);
    qDebug() << message;
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastConnDuration(QString uri, QString duration){
    QString message = QString("{\"menuID\":\"ConnDuration\", \"uri\":\"%1\", \"duration\":\"%2\"}").arg(uri).arg(duration);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastSystemInfo(QString memmory, QString storage, QString cupusage, QString currentTime, QString currentDate)
{
    QString message = QString("{\"menuID\":\"SystemInfo\", \"memmory\":%1, \"storage\":%2, \"cupusage\":%3, \"currentTime\":\"%4\", \"currentDate\":\"%5\"}")
            .arg(memmory).arg(storage).arg(cupusage).arg(currentTime).arg(currentDate);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastAudioInfo(qreal audioInLevel, qreal audioOutLevel)
{
    QString message = QString("{\"menuID\":\"AudioInfo\", \"audioInLevel\":%1, \"audioOutLevel\":%2}")
            .arg(audioInLevel).arg(audioOutLevel);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastTestmodeEnable(int testModeIndex)
{
    QString message = QString("{\"menuID\":\"testModeEnable\", \"testModeIndex\":%1}")
            .arg(testModeIndex);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::broadcastSystemMessage(QString nodeSelected)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTime = currentDateTime.time().toString("hh:mm:ss");
    QString currentDate = currentDateTime.date().toString("dd MMM yyyy");
    int hrs = currentDateTime.time().hour();
    int min = currentDateTime.time().minute();
    int sec = currentDateTime.time().second();
    QString message = QString("{\"menuID\":\"broadcastLocalTime\", \"currentTime\":\"%1\", \"currentDate\":\"%2\", \"nodeSelected\":\"%3\", \"hrs\":%4, \"min\":%5, \"sec\":%6}")
            .arg(currentTime).arg(currentDate).arg(nodeSelected).arg(hrs).arg(min).arg(sec);
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}

void ChatServer::broadcastMessage(QString message){
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
}
//snmpEnable=false
//defaultMainRadio=true
//mainVCS=false
//radioModeTxRx=true
//radioA_IPAddr=192.168.10.80
//radioB_IPAddr=192.168.10.18

void ChatServer::commandProcess(QString message, QWebSocket *pSender){
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
        emit onGPSDataChenged(GPSD_Port, "", "", 0, 0, 0, 0, 0,false, "");
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

void ChatServer::processMessage(QString message)
{
//    qDebug() << message;
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    commandProcess(message, pSender);

}

void ChatServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        m_clients.removeAll(pClient);
        m_WebSocketClients.removeAll(pClient);
        pClient->deleteLater();
        qDebug() << pClient->localAddress().toString() << "has disconect";
    }
    clientNum = m_clients.length();
    if (clientNum <= 0)
    {
        emit onNumClientChanged(clientNum);
    }
}
