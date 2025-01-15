#include "iClockDualGPS.h"

void iClockDualGPS::myConfigurate()
{
    QSettings *settings;
    const QString cfgfile = FILESETTING;
    qDebug() << "Loading configuration from:" << cfgfile;
    if(QDir::isAbsolutePath(cfgfile)){
        settings = new QSettings(cfgfile,QSettings::IniFormat);
        qDebug() << "Configuration file:" << settings->fileName();
        gpsNumber                = settings->value("GPSConfig/gpsNumber",1).toInt();

        eth0.dhcpmethod          = settings->value("NETWORK_ETH0/DHCP","on").toString();
        eth0.ipaddress           = settings->value("NETWORK_ETH0/LocalAddress","192.168.10.193").toString();
        eth0.subnet              = settings->value("NETWORK_ETH0/Netmask","255.255.255.0").toString();
        eth0.gateway             = settings->value("NETWORK_ETH0/Gateway","").toString();
        eth0.pridns              = settings->value("NETWORK_ETH0/DNS1","").toString();
        eth0.secdns              = settings->value("NETWORK_ETH0/DNS2","").toString();

        eth1.dhcpmethod          = settings->value("NETWORK_ETH1/DHCP","on").toString();
        eth1.ipaddress           = settings->value("NETWORK_ETH1/LocalAddress","192.168.10.194").toString();
        eth1.subnet              = settings->value("NETWORK_ETH1/Netmask","255.255.255.0").toString();
        eth1.gateway             = settings->value("NETWORK_ETH1/Gateway","").toString();
        eth1.pridns              = settings->value("NETWORK_ETH1/DNS1","").toString();
        eth1.secdns              = settings->value("NETWORK_ETH1/DNS2","").toString();

        timeLocation            = settings->value("NETWORK/timeLocation","").toString();
        serialNumber            = settings->value("NETWORK/serialNumber","").toString();

        if (settings->value("NETWORK/gps1_hv_ant_on","").toString() == "false"){
            gps1_hv_ant_on = 0;
        }
        else if (settings->value("NETWORK/gps1_hv_ant_on","").toString() == "true"){
            gps1_hv_ant_on = 1;
        }
        else
            gps1_hv_ant_on          = settings->value("NETWORK/gps1_hv_ant_on",0).toInt();

        bonding_enable            = settings->value("NETWORK/bonding_enable","fasle").toString() == "true";
        bond0.dhcpmethod          = settings->value("NETWORK_BOND/DHCP","off").toString();
        bond0.ipaddress           = settings->value("NETWORK_BOND/LocalAddress","192.168.10.20").toString();
        bond0.subnet              = settings->value("NETWORK_BOND/Netmask","255.255.255.0").toString();
        bond0.gateway             = settings->value("NETWORK_BOND/Gateway","").toString();
        bond0.pridns              = settings->value("NETWORK_BOND/DNS1","").toString();
        bond0.secdns              = settings->value("NETWORK_BOND/DNS2","").toString();


    }
    else{
        qDebug() << "Loading configuration from:" << cfgfile << " FILE NOT FOUND!";
    }
    qDebug() << "Loading configuration completed";
    delete settings;
}

void iClockDualGPS::getNtpPage(QWebSocket *pSender)
{
    int id=0;
    QString message;
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
            pSender->sendTextMessage(message);
            id++;
        }
    }
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
            pSender->sendTextMessage(message);
            id++;
        }
    }
}

void iClockDualGPS::getNetworkPage(QWebSocket *webSender)
{
    qDebug() << "lan0Plugin" << "eth0.dhcpmethod" << eth0.dhcpmethod << eth0.ipaddress;
    if (eth0.dhcpmethod == "on")
    {
        networking->setDHCPIpAddr3("eth0");
        networking->getAddress("eth0");
        eth0.ipaddress = networking->netWorkCardAddr;
        eth0.subnet = networking->netWorkCardMask;
        eth0.gateway = networking->netWorkCardGW;
        eth0.pridns = networking->netWorkCardDNS;
        eth0.macAddress = networking->netWorkCardMac;
        eth0.secdns = "";
    }
    else
    {

    }

    if (eth1.dhcpmethod == "on")
    {
        networking->setDHCPIpAddr3("eth1");
        networking->getAddress("eth1");
        eth1.ipaddress = networking->netWorkCardAddr;
        eth1.subnet = networking->netWorkCardMask;
        eth1.gateway = networking->netWorkCardGW;
        eth1.pridns = networking->netWorkCardDNS;
        eth1.macAddress = networking->netWorkCardMac;
        eth1.secdns = "";
    }
    else
    {

    }
    QString message = QString("{\"menuID\":\"network\", \"dhcpmethod\":\"%1\", \"ipaddress\":\"%2\", \"subnet\":\"%3\", \"gateway\":\"%4\", \"pridns\":\"%5\", \"secdns\":\"%6\", \"macAddress\":\"%7\", \"phyNetworkName\":\"%8\"}")
            .arg(eth0.dhcpmethod).arg(eth0.ipaddress).arg(eth0.subnet).arg(eth0.gateway).arg(eth0.pridns).arg(eth0.secdns).arg(eth0.macAddress).arg("eth0");
    webSender->sendTextMessage(message);

    message = QString("{\"menuID\":\"network\", \"dhcpmethod\":\"%1\", \"ipaddress\":\"%2\", \"subnet\":\"%3\", \"gateway\":\"%4\", \"pridns\":\"%5\", \"secdns\":\"%6\", \"macAddress\":\"%7\", \"phyNetworkName\":\"%8\"}")
                .arg(eth1.dhcpmethod).arg(eth1.ipaddress).arg(eth1.subnet).arg(eth1.gateway).arg(eth1.pridns).arg(eth1.secdns).arg(eth1.macAddress).arg("eth1");
        webSender->sendTextMessage(message);

    message = QString("{\"menuID\":\"network\", \"dhcpmethod\":\"%1\", \"ipaddress\":\"%2\", \"subnet\":\"%3\", \"gateway\":\"%4\", \"pridns\":\"%5\", \"secdns\":\"%6\", \"macAddress\":\"%7\", \"phyNetworkName\":\"%8\"}")
                    .arg(bond0.dhcpmethod).arg(bond0.ipaddress).arg(bond0.subnet).arg(bond0.gateway).arg(bond0.pridns).arg(bond0.secdns).arg(bond0.macAddress).arg("bond0");
            webSender->sendTextMessage(message);
}

void iClockDualGPS::scanFileUpdate()
{
    QStringList fileupdate;
    fileupdate = findFile();
    if (fileupdate.size() > 0){
        if(foundfileupdate == false)
            updateFirmware();
    }
}

QStringList iClockDualGPS::findFile()
{
    QStringList listfilename;
    QString ss="/var/www/html/uploads/";
    const char *sss ; sss = ss.toStdString().c_str();
    QDir dir1("/var/www/html/uploads/");
    QString filepath;
    QString filename;
    QFileInfoList fi1List( dir1.entryInfoList( QDir::Files, QDir::Name) );
    foreach( const QFileInfo & fi1, fi1List ) {
        filepath = QString::fromUtf8(fi1.absoluteFilePath().toLocal8Bit());
        filename = QString::fromUtf8(fi1.fileName().toLocal8Bit());
        listfilename << filepath;
        qDebug() << filepath;// << filepath.toUtf8().toHex();
    }
    return listfilename;
}

void iClockDualGPS::restartnetwork(QString networkName)
{
    networking->resetNetwork();
}

int iClockDualGPS::rebootSystem()
{
    system("reboot");
    exit(0);
}
void iClockDualGPS::updateFirmware()
{
    qDebug() << "Start updateFirmware";
    foundfileupdate = true;
    QStringList fileupdate;
    fileupdate = findFile();
    system("mkdir -p /tmp/update");
    if (fileupdate.size() > 0){
        qDebug() << "Start update";
        updateStatus = 1;
        QString sendMessage = QString("{\"menuID\":\"update\", \"updateStatus\":%1}").arg(updateStatus);
        SocketServer->broadcastMessage(sendMessage);
        SSLSocketServer->broadcastMessage(sendMessage);
        QString commandCopyFile = "cp " + QString(fileupdate.at(0)) + " /tmp/update/update.tar";
        system(commandCopyFile.toStdString().c_str());
        system("tar -xf /tmp/update/update.tar -C /tmp/update/");
        system("sh /tmp/update/update.sh");
        updateStatus = 2;
        sendMessage = QString("{\"menuID\":\"update\", \"updateStatus\":%1}").arg(updateStatus);
        SocketServer->broadcastMessage(sendMessage);
        SSLSocketServer->broadcastMessage(sendMessage);
        qDebug() << "Update complete";
    }
    foundfileupdate = false;
}
void iClockDualGPS::getSystemPage(QWebSocket *webSender)
{
    int dateTimeMethod;

    QString message = QString("{\"menuID\":\"system\", \"SwVersion\":\"%1\", \"HwVersion\":\"%2\", \"dateTimeMethod\":\"%3\", \"location\":\"%4\"}")
            .arg(SWVERSION).arg(HWVERSION).arg(dateTimeMethod).arg(timeLocation);
    webSender->sendTextMessage(message);

}
void iClockDualGPS::updateNetworkInfo(phyNetwork ethernet, QString phyNetworkName)
{
    QString message = QString("{\"menuID\":\"network\", \"dhcpmethod\":\"%1\", \"ipaddress\":\"%2\", \"subnet\":\"%3\", \"gateway\":\"%4\", \"pridns\":\"%5\", \"secdns\":\"%6\", \"macAddress\":\"%7\", \"phyNetworkName\":\"%8\"}")
            .arg(ethernet.dhcpmethod).arg(ethernet.ipaddress).arg(ethernet.subnet).arg(ethernet.gateway).arg(ethernet.pridns).arg(ethernet.secdns).arg(ethernet.macAddress).arg(phyNetworkName);
    SocketServer->broadcastMessage(message);
}
void iClockDualGPS::lanPlugin(QString phyNetworkName)
{
    qDebug() << "lanPlugin" << phyNetworkName;
    if (phyNetworkName == "eth0"){
        qDebug() << "lan0Plugin";
        if (eth0.dhcpmethod == "on")
        {
            networking->setDHCPIpAddr3("eth0");
            networking->getAddress("eth0");
            eth0.ipaddress = networking->netWorkCardAddr;
            eth0.subnet = networking->netWorkCardMask;
            eth0.gateway = networking->netWorkCardGW;
            eth0.pridns = networking->netWorkCardDNS;
            eth0.macAddress = networking->netWorkCardMac;
            eth0.secdns = "";

            QStringList dns = eth0.pridns.split(" ");
            if (dns.size() > 0)
                eth0.pridns = dns.at(0);
            if (dns.size() > 1)
                eth0.secdns = dns.at(1);
        }
        else
        {
            networking->setStaticIpAddr3(eth0.ipaddress,eth0.subnet,eth0.gateway,eth0.pridns,eth0.secdns,"eth0");
        }
        eth0.lan_ok = true;
        updateNetworkInfo(eth0,"eth0");
    }
    else if (phyNetworkName == "eth1"){
        qDebug() << "lan1Plugin";
        if (eth1.dhcpmethod == "on")
        {
            networking->setDHCPIpAddr3("eth1");
            networking->getAddress("eth1");
            eth1.ipaddress = networking->netWorkCardAddr;
            eth1.subnet = networking->netWorkCardMask;
            eth1.gateway = networking->netWorkCardGW;
            eth1.pridns = networking->netWorkCardDNS;
            eth1.macAddress = networking->netWorkCardMac;
            eth1.secdns = "";

            QStringList dns = eth1.pridns.split(" ");
            if (dns.size() > 0)
                eth1.pridns = dns.at(0);
            if (dns.size() > 1)
                eth1.secdns = dns.at(1);
        }
        else
        {
            networking->setStaticIpAddr3(eth1.ipaddress,eth1.subnet,eth1.gateway,eth1.pridns,eth1.secdns,"eth1");
        }
        eth1.lan_ok = true;
        updateNetworkInfo(eth1,"eth1");
    }
}


void iClockDualGPS::lanRemove(QString phyNetworkName)
{
    qDebug() << "lanRemove" << phyNetworkName;
    if (phyNetworkName == "eth0"){
        if (eth0.dhcpmethod == "on")
        {
            eth0.ipaddress = "";
            eth0.subnet = "";
            eth0.gateway = "";
            eth0.pridns = "";
            eth0.secdns = "";
        }
        eth0.lan_ok = false;
        updateNetworkInfo(eth0,"eth0");
    }
    else if (phyNetworkName == "eth1"){
        if (eth1.dhcpmethod == "on")
        {
            eth1.ipaddress = "";
            eth1.subnet = "";
            eth1.gateway = "";
            eth1.pridns = "";
            eth1.secdns = "";
        }
        eth1.lan_ok = false;
        updateNetworkInfo(eth1,"eth1");
    }
}

QString iClockDualGPS::getTimezone()
{
    QProcess getTimezoneProcess;
    QString prog = "/bin/bash";//shell
    QStringList arguments;
    QString Timezone;
    arguments << "-c" << QString("ls -la /etc/localtime | grep '/usr/share/zoneinfo/' | awk '{print $11}'");
    getTimezoneProcess.start(prog , arguments);
    getTimezoneProcess.waitForFinished(100);
    Timezone = QString(getTimezoneProcess.readAll()).trimmed();
    arguments.clear();
    qDebug() << "Timezone" << Timezone;
    return Timezone.replace("/usr/share/zoneinfo/","");
}
void iClockDualGPS::setLocation(QString location)
{
    if (!location.contains("Select"))
    {
        QString command = QString("ln -sf /usr/share/zoneinfo/%1  /etc/localtime").arg(location);
        system(command.toStdString().c_str());
        timeLocation = location;
    }

    QSettings *settings;
    const QString cfgfile = FILESETTING;
    qDebug() << "Loading configuration from:" << cfgfile;
    if(QDir::isAbsolutePath(cfgfile)){
        settings = new QSettings(cfgfile,QSettings::IniFormat);
        qDebug() << "Configuration file:" << settings->fileName();
        settings->setValue("NETWORK/timeLocation",timeLocation);
    }
    delete settings;
}
void iClockDualGPS::updateNetwork(uint8_t DHCP, QString LocalAddress, QString Netmask, QString Gateway, QString DNS1, QString DNS2,QString phyNetworkName)
{
    QSettings *settings;
    const QString cfgfile = FILESETTING;
    QString strDhcpMethod = "off";
    if (DHCP) strDhcpMethod = "on";

    if(QDir::isAbsolutePath(cfgfile))
    {
        qDebug() << "Loading configuration from:" << cfgfile;
        settings = new QSettings(cfgfile,QSettings::IniFormat);
        qDebug() << "Configuration file:" << settings->fileName();


        if(phyNetworkName == "eth0")
        {
            settings->setValue("NETWORK_ETH0/DHCP",strDhcpMethod);
            settings->setValue("NETWORK_ETH0/LocalAddress",LocalAddress);
            settings->setValue("NETWORK_ETH0/Netmask",Netmask);
            settings->setValue("NETWORK_ETH0/Gateway",Gateway);
            settings->setValue("NETWORK_ETH0/DNS1",DNS1);
            settings->setValue("NETWORK_ETH0/DNS2",DNS2);
        }
        else if (phyNetworkName == "eth1")
        {
            settings->setValue("NETWORK_ETH1/DHCP",strDhcpMethod);
            settings->setValue("NETWORK_ETH1/LocalAddress",LocalAddress);
            settings->setValue("NETWORK_ETH1/Netmask",Netmask);
            settings->setValue("NETWORK_ETH1/Gateway",Gateway);
            settings->setValue("NETWORK_ETH1/DNS1",DNS1);
            settings->setValue("NETWORK_ETH1/DNS2",DNS2);
        }
        else if (phyNetworkName == "bond0")
        {
            settings->setValue("NETWORK/bonding_enable",bonding_enable);
            settings->setValue("NETWORK_BOND/DHCP",strDhcpMethod);
            settings->setValue("NETWORK_BOND/LocalAddress",LocalAddress);
            settings->setValue("NETWORK_BOND/Netmask",Netmask);
            settings->setValue("NETWORK_BOND/Gateway",Gateway);
            settings->setValue("NETWORK_BOND/DNS1",DNS1);
            settings->setValue("NETWORK_BOND/DNS2",DNS2);
        }


    }

    if  ((phyNetworkName == "eth0") || (phyNetworkName == "eth1"))
    {
        bonding_enable = false;
    }
    else
    {
        bonding_enable = true;
    }

    if (DHCP)
    {
        networking->setDHCPIpAddr3(phyNetworkName);
    }
    else{
        networking->setStaticIpAddr3(LocalAddress,Netmask,Gateway,DNS1,DNS2,phyNetworkName);
    }

    if(phyNetworkName == "eth0")
    {
        eth0.dhcpmethod = strDhcpMethod;
        eth0.ipaddress = LocalAddress;
        eth0.subnet = Netmask;
        eth0.gateway = Gateway;
        eth0.pridns = DNS1;
        eth0.secdns = DNS2;
        updateNetworkInfo(eth0, "eth0");
    }
    else if(phyNetworkName == "eth1")
    {
        eth1.dhcpmethod = strDhcpMethod;
        eth1.ipaddress = LocalAddress;
        eth1.subnet = Netmask;
        eth1.gateway = Gateway;
        eth1.pridns = DNS1;
        eth1.secdns = DNS2;
        updateNetworkInfo(eth1, "eth1");
    }
    else{
        bond0.dhcpmethod = strDhcpMethod;
        bond0.ipaddress = LocalAddress;
        bond0.subnet = Netmask;
        bond0.gateway = Gateway;
        bond0.pridns = DNS1;
        bond0.secdns = DNS2;
        updateNetworkInfo(bond0, "bond0");
    }

    delete settings;
}


void iClockDualGPS::setgps_voltage(int gpsnum, int voltage)
{

}
