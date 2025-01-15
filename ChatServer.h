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
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "QWebSocketServer"
#include "QWebSocket"
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class ChatServer : public QObject
{
    Q_OBJECT
public:
    explicit ChatServer(quint16 port, QObject *parent = Q_NULLPTR);
    virtual ~ChatServer();
    void broadcastMessage(QString message);
    void updateHomeInput(QString localname, uint8_t inputLevel, uint8_t outputLevel, QWebSocket *newClient, uint16_t keepAlivePeroid, uint16_t sipPort, uint8_t portInterface, uint8_t txScheduler, uint8_t invitemode, float siteTone, int defaultEthernet, int testModeEnable);
    void updateHomeED137Cfg(uint8_t nodeID, uint8_t nodeType, QString nodeName, QString ipAddress, QString sipPort, uint8_t active, QWebSocket *newClient, QString frequency, QString sqlLevel, QString nodeNameRx, QString ipAddressRx, QString sipPortRx, QString frequencyRx, QString sqlLevelRx, QString channelName, int rfPower);
    void updatefrequency(uint8_t nodeID, QString frequency, QString sqlLevel, QString frequencyRx, int rfPower);
    void updateNetworkInfo(uint8_t dhcpmethod, QString ipaddress, QString subnet, QString gateway, QString pridns, QString secdns, QString ntpServer, QWebSocket *newClient, QString phyNetworkName);
    void updateSystemInfo(QString SwVersion, QString HwVersion, bool ntp, QString ntpServer, QString timeLocation, QWebSocket *newClient);
    void updateHomeNodeState(uint8_t nodeID, QString conn, QString duration, QString durationRx, QString trxStatus, QWebSocket *newClient);
    void updateAllowedUri(uint8_t numConn, QString uri1, QString uri2, QString uri3, QString uri4, QString uri5, QString uri6, QString uri7, QString uri8, QWebSocket *newClient);
    void updateClientConnStatus(uint8_t connNum, QString TxRxStatus, QString lastPtt, QWebSocket *newClient);
    void broadcastMessageNodeState(uint8_t nodeID, QString conn, QString duration, QString trxStatus, QString radioStatus, QString vswr, QString durationRx, QString connRx, QString radioStatusRx);
    void broadcastMessageStatus(uint8_t connNum, QString TxRxStatus, QString lastPtt);
    void broadcastGpioInStatus(uint8_t gpio1Val, uint8_t gpio2Val, uint8_t gpio3Val, uint8_t gpio4Val, uint8_t gpio1OutVal, uint8_t gpio2OutVal, uint8_t gpio3OutVal, uint8_t gpio4OutVal);
    void updateGpioInStatus(uint8_t gpio1Val,uint8_t gpio2Val, uint8_t gpio3Val, uint8_t gpio4Val,uint8_t gpio1OutVal,uint8_t gpio2OutVal, uint8_t gpio3OutVal, uint8_t gpio4OutVal,QWebSocket *newClient);
    void updateURIConnList(QString uriConnList, QWebSocket *newClient);
    void updateSqlDefeat(int sqlDefeat, QWebSocket *newClient);
    void broadcastURIConnList(QString uriConnList);
    void broadcastConnDuration(QString uri, QString duration);
    void broadcastSystemInfo(QString memmory, QString storage, QString cupusage, QString currentTime, QString currentDate);
    void broadcastSystemMessage(QString nodeSelected);
    void broadcastAudioInfo(qreal audioInLevel, qreal audioOutLevel);
    void broadcastTestmodeEnable(int testModeIndex);
    void updateCATIS(bool enableRecording, int warningAudioLevelTime, int warningPercentFault, int warningPTTMinute, int backupAudioMin, QWebSocket *newClient);
    int clientNum = 0;
    QList<QWebSocket *> m_WebSocketClients;
//    snmpEnable=false
//    defaultMainRadio=true
//    mainVCS=false
//    radioModeTxRx=true
//    radioA_IPAddr=192.168.10.80
//    radioB_IPAddr=192.168.10.18

signals:
    void updateChannelConf(QString radioA_IPAddr, QString radioB_IPAddr, bool radioModeTxRx, bool defaultMainRadio, bool mainVCS, bool snmpEnable, int schedulerIndex, int channelID, QString webchannelID);
    void newConnection(QString menuIndex, QWebSocket *newClient);
    void updateNetwork(uint8_t dhcpmethod, QString ipaddress, QString subnet, QString gateway, QString pridns, QString secdns, QString phyNetworkName);
    void onGPSDataChenged(uint16_t GPSD_Port,QString GPS_Date,QString GPS_Time,double GPS_Lat,double GPS_Long,double GPS_Alt,int GPS_Sat,int GPS_SatUse, bool locked, QString message);
    void updateScheduler(int value, int channelID);
    void updateFirmware();
    void toggleGpioOut(int gpioNum,int gpioVal);
    void updateNTPServer(QString value);
    void setcurrentDatetime(QString value);
    void setLocation(QString value);
    void restartnetwork(QString phyNetworkName);
    void rebootSystem();
    void setgps_voltage(int gpsnum, int voltage);
    void onNewMessage(QString message);
    void getMonitorPage(QWebSocket *pSender);
    void getNetworkPage(QWebSocket *pSender);
    void getSystemPage(QWebSocket *pSender);
    void getNtpPage(QWebSocket *pSender);
    void deleteServer(QString address);
    void addServerPoolPeer(QString ServerPoolPeer, QString address, QString option);
    void getCtrlInfoPage(int idInRole, QWebSocket *pSender);
    void toggleRxEnable(int idInRole);
    void toggleTxEnable(int idInRole);
    void onNumClientChanged(int clientnum);
    void getChannelList(QWebSocket *pSender);
    void getCtrlinfoPage(QWebSocket *pSender,int webchannelID);



private Q_SLOTS:
    void onNewConnection();
    void commandProcess(QString message, QWebSocket *pSender);
    void processMessage(QString message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

    QWebSocket *newSocket;
};

#endif //CHATSERVER_H
