#include "iClockDualGPS.h"

iClockDualGPS::iClockDualGPS(QObject *parent) : QObject(parent)
{
    system("systemctl stop serial-getty@ttyAMA0.service");

    system("systemctl stop gpsd");
    system("systemctl stop pgpsd");
    system("systemctl stop chrony");

    networking = new NetworkMng(this);
    gpioResetUblox = new GPIOClass(RESET_UBLOX);
    ledNTP = new GPIOClass(LED_NTP);
    ledGPS = new GPIOClass(LED_GPS);
    ledRUN = new GPIOClass(LED_RUN);
    ipReset = new GPIOClass(RESET_IP);
    ethDetect = new QFileSystemWatcher(this);
    myDatabase = new Database("iClock","userData","Ifz8zean6868**","127.0.0.1",this);
    myDatabase->database_createConnection();

    checkNetworkCardTimer = new QTimer(this);
    QThread::msleep(200);
    gpioResetUblox->setdir_gpio("out");
    ipReset->setdir_gpio("in");


    myConfigurate();

    SocketServer = new ChatServer(1234);
    SSLSocketServer = new SslEchoServer(1235);
    reset_ublox();
    selectGPS_PPS(setGPSUsed);

    connect(SocketServer,SIGNAL(onGPSDataChenged(uint16_t ,QString ,QString ,double ,double ,double ,int ,int, bool, QString )),this,SLOT(onGPSDataChenged(uint16_t ,QString ,QString ,double ,double ,double ,int ,int, bool , QString)));

    connect(SocketServer,SIGNAL(setLocation(QString )),
                      this,SLOT(setLocation(QString )));
    connect(SocketServer,SIGNAL(getNtpPage(QWebSocket *)),
                      this,SLOT(getNtpPage(QWebSocket *)));
    connect(SocketServer,SIGNAL(getSystemPage(QWebSocket *)),
                      this,SLOT(getSystemPage(QWebSocket *)));
    connect(SocketServer,SIGNAL(getNetworkPage(QWebSocket *)),
                      this,SLOT(getNetworkPage(QWebSocket *)));
    connect(SocketServer,SIGNAL(restartnetwork(QString )),
                          this,SLOT(restartnetwork(QString )));
    connect(SocketServer,SIGNAL(updateFirmware()),
                          this,SLOT(updateFirmware()));
    connect(SocketServer,SIGNAL(deleteServer(QString)),
                          this,SLOT(deleteServer(QString)));
    connect(SocketServer,SIGNAL(addServerPoolPeer(QString , QString , QString )),
                          this,SLOT(addServerPoolPeer(QString , QString , QString )));
    connect(SocketServer,SIGNAL(rebootSystem()),
                          this,SLOT(rebootSystem()));
    connect(SocketServer,SIGNAL(setgps_voltage(int , int )),
                          this,SLOT(setgps_voltage(int , int )));

    connect(SocketServer,SIGNAL(updateNetwork(uint8_t , QString , QString , QString , QString , QString , QString )),
                      this,SLOT(updateNetwork(uint8_t , QString , QString , QString , QString , QString , QString )));

    connect(SSLSocketServer,SIGNAL(onGPSDataChenged(uint16_t ,QString ,QString ,double ,double ,double ,int ,int, bool, QString )),
                this,SLOT(onGPSDataChenged(uint16_t ,QString ,QString ,double ,double ,double ,int ,int , bool , QString )));

    connect(SSLSocketServer,SIGNAL(setLocation(QString )),
                      this,SLOT(setLocation(QString )));
    connect(SSLSocketServer,SIGNAL(getNtpPage(QWebSocket *)),
                      this,SLOT(getNtpPage(QWebSocket *)));
    connect(SSLSocketServer,SIGNAL(getSystemPage(QWebSocket *)),
                      this,SLOT(getSystemPage(QWebSocket *)));
    connect(SSLSocketServer,SIGNAL(getNetworkPage(QWebSocket *)),
                      this,SLOT(getNetworkPage(QWebSocket *)));
    connect(SSLSocketServer,SIGNAL(restartnetwork(QString )),
                          this,SLOT(restartnetwork(QString )));
    connect(SSLSocketServer,SIGNAL(updateFirmware()),
                          this,SLOT(updateFirmware()));
    connect(SSLSocketServer,SIGNAL(deleteServer(QString)),
                          this,SLOT(deleteServer(QString)));
    connect(SSLSocketServer,SIGNAL(addServerPoolPeer(QString , QString , QString )),
                          this,SLOT(addServerPoolPeer(QString , QString , QString )));
    connect(SSLSocketServer,SIGNAL(rebootSystem()),
                          this,SLOT(rebootSystem()));
    connect(SSLSocketServer,SIGNAL(setgps_voltage(int , int )),
                          this,SLOT(setgps_voltage(int , int )));

    connect(SSLSocketServer,SIGNAL(updateNetwork(uint8_t , QString , QString , QString , QString , QString , QString )),
                      this,SLOT(updateNetwork(uint8_t , QString , QString , QString , QString , QString , QString )));


    connect(checkNetworkCardTimer,SIGNAL(timeout()),this,SLOT(checkNetworkCard()));
    connect(this,SIGNAL(onGpsLockChenged(bool , uint8_t)),this,SLOT(gpsLockChenged(bool , uint8_t)));
    connect(ethDetect, SIGNAL(fileChanged(const QString &)), this, SLOT(fileETHDetectChanged(const QString &)));
    connect(networking,SIGNAL(lanPlugin(QString)),this,SLOT(lanPlugin(QString)));
    connect(networking,SIGNAL(lanRemove(QString)),this,SLOT(lanRemove(QString)));

    checkNetworkCardTimer->start(100);


    m_IsEventThreadRunning = false;
//    m_EventThreadInstance = new boost::shared_ptr<boost::thread>;
//    m_EventThreadInstance->reset(new boost::thread(boost::bind(&iClockDualGPS::displayThread, this)));

    m_IsEventThreadRunning_1 = false;
//    m_EventThreadInstance_1 = new boost::shared_ptr<boost::thread>;
//    m_EventThreadInstance_1->reset(new boost::thread(boost::bind(&iClockDualGPS::displayThread2, this)));

    myDatabase->initServer();

    setgps_voltage(0,0);
    ledGPS->set_led_brightness(0);
    ledNTP->set_led_brightness(0);
}
void iClockDualGPS::fileETHDetectChanged(const QString & path)
{
    qDebug() << "fileETHDetectChanged" << path;
}
void iClockDualGPS::onGPSDataChenged(uint16_t GPSD_Port,QString GPS_Date,QString GPS_Time,double GPS_Lat,double GPS_Long,double GPS_Alt,int GPS_Sat,int GPS_SatUse, bool locked, QString message)
{
    GPSInfo gpsData;

    gpsData.locked = locked;
    gpsData.date = GPS_Date;
    gpsData.time = GPS_Time;
    gpsData.lat = GPS_Lat;
    gpsData.lon = GPS_Long;
    gpsData.alt = GPS_Alt;
    gpsData.sat = GPS_Sat;
    gpsData.satUse = GPS_SatUse;
    if (GPSD_Port == GPS1)
    {
        if (GPS1Data.locked != locked)
        {
            GPS1Data = gpsData;
            qDebug() <<  GPS_Date << GPS_Time << "GPS1Data onGPSDataChenged" << "locked" << locked;
            emit onGpsLockChenged(locked, 1);
        }
        else
            GPS1Data = gpsData;

    }
    if (GPSD_Port == GPS2)
    {
        if (GPS2Data.locked != locked)
        {
            GPS2Data = gpsData;
            qDebug() <<  GPS_Date << GPS_Time << "GPS2Data onGPSDataChenged" << "locked" << locked;
            emit onGpsLockChenged(locked, 2);
        }
        else
            GPS2Data = gpsData;
    }
    if (message != "")
        SSLSocketServer->broadcastMessage(message);
}
void iClockDualGPS::gpsLockChenged(bool locked, uint8_t gpsDev)
{
    if (locked){
        ledGPS->set_led_brightness(1);

        if (GPS1Data.date.split("/").size() >= 2)
        {
        QStringList dateList = GPS1Data.date.split("/");
        QString date = QString("%1-%2-%3").arg(dateList.at(2)).arg(dateList.at(0)).arg(dateList.at(1));
        QString time = GPS1Data.time;

            QString command = QString("date --set '%1 %2'").arg(date).arg(time);
            system(command.toStdString().c_str());
        }
    }
    else
        ledGPS->set_led_brightness(0);

    if (setGPSUsed == PPSAUTO)
    {
        if (locked){
         if ((gpsDev == 1) & (locked) & (GPS1Data.satUse >= GPS2Data.satUse))
             selectGPS_PPS(PPS1);
         else if ((gpsDev == 2) & (locked) & (GPS2Data.satUse > GPS1Data.satUse))
             selectGPS_PPS(PPS2);
        }
        else {
            if (gpsDev == 1){
                if (GPS2Data.locked)
                    selectGPS_PPS(PPS2);
            }
            else if (gpsDev == 2){
                if (GPS1Data.locked)
                    selectGPS_PPS(PPS1);
            }

        }
    }
}
void iClockDualGPS::selectGPS_PPS(int dev)
{

}

void iClockDualGPS::reset_ublox()
{
    gpioResetUblox->resetGpio();
    QThread::msleep(200);
    gpioResetUblox->setGpio();
    QThread::msleep(200);
    system("systemctl restart gpsd");
    system("systemctl restart pgpsd");
//    QString dateTime = "2000/01/01 00:00";
//    QString date;
//    QString time;
//    if (dateTime.split(" ").size() >= 2){
//        date = QString(dateTime.split(" ").at(0)).replace("/","-");
//        time = QString(dateTime.split(" ").at(1))+":00";
//        QString command = QString("date --set '%1 %2'").arg(date).arg(time);
//        system(command.toStdString().c_str());
//    }
//    QThread::msleep(1000);
    system("systemctl restart chrony");
}
void iClockDualGPS::ipResetCountOnchanged()
{
    if (ipResetCount == 0)
    {
        ledRUN->set_led_trigger("mmc0");
    }
    else if (ipResetCount == 1)
    {
        ledRUN->set_led_trigger("none");
    }
    else if (ipResetCount == 10)
    {
        ledRUN->set_led_trigger("default-on");
    }
    else if (ipResetCount == 30)
    {
        ledRUN->set_led_trigger("heartbeat");
    }
    else if (ipResetCount == 50)
    {
        checkNetworkCardTimer->stop();
        ledRUN->set_led_trigger("mmc0");
        ipResetCount = 50;
        updateNetwork(0, "192.168.1.1",  "255.255.255.0",  "192.168.1.254", "8.8.8.8", "8.8.4.4","eth0");
        updateNetwork(0, "192.168.10.1",  "255.255.255.0",  "", "", "","eth1");
        checkNetworkCardTimer->start(100);
    }
}
void iClockDualGPS::checkNetworkCard()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTime = currentDateTime.toString("hh:mm:ss");
    QString currentDate = currentDateTime.toString("ddd yyyy-MM-dd");
    int hrs = currentDateTime.time().hour();
    int min = currentDateTime.time().minute();
    int sec = currentDateTime.time().second();
    bool resetIp = ipReset->getGpioVal() == 0;
    if (resetIp)
    {
        ipResetCount++;
        ipResetCountOnchanged();
    }
    else
    {
        if (ipResetCount != 0)
            ipResetCountOnchanged();
        ipResetCount = 0;
    }
    QString message = QString("{\"menuID\":\"broadcastLocalTime\", \"currentTime\":\"%1\", \"currentDate\":\"%2\", \"hrs\":%3, \"min\":%4, \"sec\":%5}")
            .arg(currentTime).arg(currentDate).arg(hrs).arg(min).arg(sec);
    if (messageDateTime0 != message){
        SocketServer->broadcastMessage(message);
        SSLSocketServer->broadcastMessage(message);
    }
    messageDateTime0 = message;

    if (secGet != sec){
        secGet = sec;
        if (sec%5 == 0) {
            bool eth0lan_ok = false;
            bool eth1lan_ok = false;
            bool bond0lan_ok = false;

            eth0lan_ok = networking->getLinkDetected("eth0");
            eth1lan_ok = networking->getLinkDetected("eth1");
            if (bonding_enable)
            {
                bond0lan_ok = networking->getLinkDetected("bond0");
                eth0.ipaddress = networking->netWorkCardAddr;
                bond0.ipaddress = networking->netWorkCardAddr;
            }
        }

        bool webClientsConn = (SocketServer->m_WebSocketClients.size() > 0);
        bool sslWebClientsConn = (SSLSocketServer->m_WebSocketClients.size() > 0);
//            chrony_sources = ConnmanETH0->getChronycSources();
        chrony_sources_list = networking->getChronycSourcesCSV();

        if (webClientsConn)
        {
            chrony_clients_list = networking->getChronycClientsCSV();
            double cputemp = networking->getCPUTemp();
            double cpuUsage = networking->getCPUUsage();
            double memUsage = networking->getMemUsage();
            QString uptime = networking->getUPTime();
            message = QString("{\"menuID\":\"systemInfo\", \"cputemp\":\"%1\", \"cpuUsage\":\"%2\", \"memUsage\":%3, \"uptime\":\"%4\", \"serialNumber\":\"%5\"}")
                        .arg(cputemp).arg(cpuUsage).arg(memUsage).arg(uptime).arg(serialNumber);
            SocketServer->broadcastMessage(message);
//            qDebug() << "getCPUTemp" << ConnmanETH0->getCPUTemp() << "getCPUUsage" << ConnmanETH0->getCPUUsage() << "getMemUsage" << ConnmanETH0->getMemUsage();
        }
        if (sslWebClientsConn)
        {
            chrony_clients_list = networking->getChronycClientsCSV();
            double cputemp = networking->getCPUTemp();
            double cpuUsage = networking->getCPUUsage();
            double memUsage = networking->getMemUsage();
            QString uptime = networking->getUPTime();
            message = QString("{\"menuID\":\"systemInfo\", \"cputemp\":\"%1\", \"cpuUsage\":\"%2\", \"memUsage\":%3, \"uptime\":\"%4\", \"serialNumber\":\"%5\"}")
                        .arg(cputemp).arg(cpuUsage).arg(memUsage).arg(uptime).arg(serialNumber);
            SSLSocketServer->broadcastMessage(message);
//            qDebug() << "getCPUTemp" << ConnmanETH0->getCPUTemp() << "getCPUUsage" << ConnmanETH0->getCPUUsage() << "getMemUsage" << ConnmanETH0->getMemUsage();
        }

        int id = 0;
        chrony_sources = "Holdover";
        Q_FOREACH(QString chronySources, chrony_sources_list)
        {
            QStringList chronySourceslist = chronySources.split(",");
            if (chronySourceslist.size() == 10)
            {
                double offset = QString(chronySourceslist.at(7)).toDouble();
                int lastRx = QString(chronySourceslist.at(6)).toInt();
                int local_clock =  QString(chronySourceslist.at(0)) == "#" ? 1 : QString(chronySourceslist.at(0)) == "^" ? 0 : 2;
                QString address = QString(chronySourceslist.at(2));
                bool bestTime = QString(chronySourceslist.at(1)) == "*";
                int Stratum = QString(chronySourceslist.at(3)).toInt();
                if(bestTime)
                    chrony_sources = address;

                message = QString("{\"menuID\":\"chrony_sources_list\", \"chronySources\":\"%1\", \"id\":%2, \"offset\":%3, \"lastRx\":%4, \"address\":\"%5\", \"local_clock\":%6, \"bestTime\":%7, \"size\":%8, \"Stratum\":%9}")
                        .arg(chronySources).arg(id).arg(offset).arg(lastRx).arg(address).arg(local_clock).arg(bestTime).arg(chrony_sources_list.size()).arg(Stratum);
                if (messageDateTime1 != message){
                    messageDateTime1 = message;
                    SocketServer->broadcastMessage(message);
                    SSLSocketServer->broadcastMessage(message);
                }
                id++;
            }
        }
        if (chrony_sources == "Holdover"){
            ledNTP->set_led_trigger("none");
            ledNTP->set_led_brightness(0);
        }
        else if(chrony_sources == "PPS"){
            ledNTP->set_led_trigger("none");
            ledNTP->set_led_brightness(1);
        }
        else{
            ledNTP->set_led_brightness(0);
            ledNTP->set_led_trigger("heartbeat");
        }

        if (webClientsConn)
        {
            id = 0;
            Q_FOREACH(QString chronyClients, chrony_clients_list)
            {
                QStringList chronyClientslist = chronyClients.split(",");
                QString address = QString(chronyClientslist.at(0));
                if ((chronyClientslist.size() == 10) & (address!= "127.0.0.1") & (address!= "::1"))
                {

                    int NTP = QString(chronyClientslist.at(1)).toInt();
                    int Drop = QString(chronyClientslist.at(2)).toInt();
                    int Last = QString(chronyClientslist.at(5)).toInt();

                    message = QString("{\"menuID\":\"chrony_clients_list\", \"chronyClients\":\"%1\", \"id\":%2, \"NTP\":%3, \"Drop\":%4, \"address\":\"%5\", \"Last\":%6, \"size\":%7}")
                            .arg(chronyClients).arg(id).arg(NTP).arg(Drop).arg(address).arg(Last).arg(chrony_clients_list.size());
                    if (messageDateTime2 != message){
                        messageDateTime2 = message;
                        SocketServer->broadcastMessage(message);
                        SSLSocketServer->broadcastMessage(message);
                    }
                    id++;
                }
            }
        }
    }
}
void iClockDualGPS::displayThread2()
{
    qDebug() << "*********************************************************** displayThread2 start ********************************************************" ;
    displayClock = new iClockDisplay(this);
    while (m_IsEventThreadRunning_1)
    {
        boost::unique_lock<boost::mutex> scoped_lock(io_mutex_1);
        displayClock->setcurrentDateTime();
        displayClock->writeOLEDRun();
        QThread::msleep(10);
    }
}



void iClockDualGPS::displayThread()
{
    qDebug() << "*********************************************************** displayThread start ********************************************************" ;
    displayInfo = new infoDisplay(this);
    phyNetwork _eth0;
    phyNetwork _eth1;
    GPSInfo _GPS1Data;
    GPSInfo _GPS2Data;
    int blinkCount = 0;
    bool gps1DataChenged = false;
    bool gps2DataChenged = false;
    QString _Reference;
    QString line2String1 = "Used/All";
    QString line2String2 = "Used/All";
    displayInfo->setTextLine2_1(line2String1);
    displayInfo->setTextLine2_2(line2String2);

    while (m_IsEventThreadRunning)
    {
        boost::unique_lock<boost::mutex> scoped_lock(io_mutex);
        if ((eth0.ipaddress != _eth0.ipaddress) || (eth0.lan_ok != _eth0.lan_ok)) onIP1Changed = true;
        if ((eth1.ipaddress != _eth1.ipaddress) || (eth1.lan_ok != _eth1.lan_ok)) onIP2Changed = true;

        gps1DataChenged = (_GPS1Data.locked != GPS1Data.locked);
        gps2DataChenged = (_GPS2Data.locked != GPS2Data.locked);

        _GPS1Data = GPS1Data;
        _GPS2Data = GPS2Data;

        _eth0 = eth0;
        _eth1 = eth1;

        if (onIP1Changed)
            qDebug() << "eth0.ipaddress" << eth0.ipaddress << eth0.lan_ok << _eth0.ipaddress << _eth0.lan_ok << onIP1Changed;
        QThread::msleep(500);
//        if  (onIP1Changed || onIP2Changed)
        {
            displayInfo->updateIPAddress("eth0",eth0.ipaddress,eth0.lan_ok);
            onIP1Changed = false;
//            qDebug() << "ETH0 Link Detected:" << eth0.lan_ok << eth0.ipaddress;

            displayInfo->updateIPAddress("eth1",eth1.ipaddress,eth1.lan_ok);
            onIP2Changed = false;
//            qDebug() << "ETH1 Link Detected:" << eth1.lan_ok << eth1.ipaddress;

            displayInfo->updateIPAddress("bond0",bond0.ipaddress,bonding_enable);
        }
//        if (gps1DataChenged || gps2DataChenged)
//        {
//            qDebug() << "GPS DataChenged GPS1Data.locked" << _GPS1Data.locked << "GPS2Data.locked" << _GPS2Data.locked;
            displayInfo->setGpsStatus(_GPS1Data.locked,_GPS2Data.locked);
//        }

        QString _chrony_sources = chrony_sources;
        if (_chrony_sources == "PPS"){
            if (currentGPSUsed == PPS1)
                _chrony_sources = "PPS1";
            else if (currentGPSUsed == PPS2)
                _chrony_sources = "PPS2";
        }

        QString Reference = QString("Reference: %1").arg(_chrony_sources);

        if (Reference != _Reference)
        {
            displayInfo->setTextLine3(Reference.toStdString().c_str());
            _Reference = Reference;
            qDebug() << "chronyc sources" << Reference;
        }

        blinkCount++;


        if (blinkCount == 5)
        {

            blinkCount = 0;
        }

        line2String1 = QString("Used/View  %1/%2").arg(_GPS1Data.satUse).arg(_GPS1Data.sat);
        line2String2 = QString("Used/View  %1/%2").arg(_GPS2Data.satUse).arg(_GPS2Data.sat);
        displayInfo->setTextLine2_1(line2String1);
        displayInfo->setTextLine2_2(line2String2);
        displayInfo->writeOLEDRun();
//        qDebug() << "line2String1" << line2String1 << "line2String2" << line2String2;
//        qDebug() << "GPS DataChenged GPS1Data.locked" << _GPS1Data.locked << "GPS2Data.locked" << _GPS2Data.locked;
    }
}
void iClockDualGPS::deleteServer(QString address)
{
    QString command = QString("chronyc delete %1").arg(address);
    system(command.toStdString().c_str());
    myDatabase->deleteServer(address);
}
void iClockDualGPS::addServerPoolPeer(QString ServerPoolPeer, QString address, QString option)
{
    QString addServerPoolPeerState = myDatabase->insertNewServer(ServerPoolPeer, address, option);

    QString message = QString("{\"menuID\":\"addServerPoolPeerState\", \"message\":\"%1\"}")
            .arg(addServerPoolPeerState);
    SocketServer->broadcastMessage(message);
    SSLSocketServer->broadcastMessage(message);
//    QString command = QString("chronyc add %1 %2 %3").arg(ServerPoolPeer).arg(address).arg(option);
//    system(command.toStdString().c_str());
}
