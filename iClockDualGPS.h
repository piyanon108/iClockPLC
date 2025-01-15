#ifndef ICLOCKDUALGPS_H
#define ICLOCKDUALGPS_H

#define SWVERSION "12012025"
#define HWVERSION "iClockPLC V1.2 2025"

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QSettings>
#include <QFileInfoList>
#include <QDir>
#include <QFileSystemWatcher>

#include <QObject>
#include "ChatServer.h"
#include "NetworkMng.h"
#include "GPIOClass.h"
#include "SPI.h"
#include "Database.h"
#include "infoDisplay.h"
#include "iClockDisplay.h"
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "sslechoserver.h"

#define GPS1    2947
#define GPS2    2949
#define PPSAUTO 0
#define PPSAUTO 0
#define PPS1    1
#define PPS2    2
/*
gpiochip4: GPIOs 252-425, parent: platform/ff0a0000.gpio, zynqmp_gpio:
 gpio-259 (                    |led1                ) out lo
 gpio-260 (                    |led2                ) out lo

gpiochip3: GPIOs 426-429, parent: platform/firmware:zynqmp-firmware:gpio, firmware:zynqmp-firmware:gpio:

gpiochip2: GPIOs 430-493, parent: platform/a00b0000.gpio, a00b0000.gpio:

gpiochip1: GPIOs 494-497, parent: platform/a0010000.gpio, a0010000.gpio:
 gpio-494 (                    |pps_axi_gpio0       ) in  lo IRQ
 gpio-495 (                    |pps_axi_gpio1       ) in  lo IRQ

gpiochip0: GPIOs 498-511, parent: platform/a0000000.gpio, a0000000.gpio:
*/
#define FILESETTING      "/home/pi/.config/iClock/setting.ini"

#define GPIOPPS1             "1011" //U13 [0] **device-tree GPIO1

#define LED_NTP          "ntp" //[26] **device-tree GPIO26
#define LED_GPS          "gps" //[27] **device-tree GPIO27
#define LED_RUN          "ACT"
#define OLED1_DC         "" //[NC]
#define OLED1_RESET      "" //[NC]
#define OLED2_DC         "" //[NC]
#define OLED2_RESET      "" //[NC]
#define RTC_RESET        "" //[NC]
#define RESET_UBLOX      "540" //[28]
#define RESET_IP         "520" //[8]

#define DC_CTRL          "" //[NC]
#define GPS_DC_SELECT    "" //[NC]

#define OLED_INFO_SPIDEV     "/dev/spidev1.0"
#define OLED_RTC_SPIDEV      "/dev/spidev2.0"

#define GPS_DC_5V       0
#define GPS_DC_12V      1
#define GPS_OFF_DC      2

class iClockDualGPS : public QObject
{
    Q_OBJECT
public:
    explicit iClockDualGPS(QObject *parent = nullptr);

signals:
    void onGpsLockChenged(bool lock, uint8_t gpsDev);
private:
    void reset_ublox();
    void selectGPS_PPS(int dev);
    int gpsNumber = 0;
    QString timeLocation = "";
    QString ntpServer = "";
    GPIOClass *gpioResetUblox;
    GPIOClass *ledNTP;
    GPIOClass *ledGPS;
    GPIOClass *ipReset;
    GPIOClass *ledRUN;
    NetworkMng *networking;
    Database   *myDatabase;
    int secGet;
    ChatServer *SocketServer;
    SslEchoServer *SSLSocketServer;
    QString messageDateTime0;
    QString messageDateTime1;
    QString messageDateTime2;
    QString serialNumber;
    QTimer *checkNetworkCardTimer;
    infoDisplay *displayInfo;
    iClockDisplay *displayClock;
    QFileSystemWatcher *ethDetect;
    QStringList ethDetectPath;
    uint8_t setGPSUsed = 0;
    uint8_t currentGPSUsed = 0;
    uint8_t dacBuffRx[2] = {0,0};
    uint8_t dacBuffTx[2] = {0x84,0x50};
    struct GPSInfo{
        QString date;
        QString time;
        double lat;
        double lon;
        double alt;
        int sat;
        int satUse;
        double speed;
        bool locked = false;
    };

    struct phyNetwork{
        QString dhcpmethod;
        QString ipaddress;
        QString subnet;
        QString gateway;
        QString pridns;
        QString secdns;
        QString macAddress;
        QString serviceName = "";
        QString phyNetworkName = "";
        bool lan_ok = false;
    };

    phyNetwork eth0;
    phyNetwork eth1;
    phyNetwork bond0;

    GPSInfo GPS1Data;
    GPSInfo GPS2Data;

    int gps1_hv_ant_on = -1;

    bool bonding_enable = false;

    bool foundfileupdate;
    int updateStatus;
    void myConfigurate();

    void scanFileUpdate();
    QStringList findFile();
    void updateNetworkInfo(phyNetwork ethernet, QString phyNetworkName);
    QString getTimezone();

    boost::mutex io_mutex;
    boost::atomic_bool m_IsEventThreadRunning;
    boost::shared_ptr<boost::thread> *m_EventThreadInstance;

    boost::mutex io_mutex_1;
    boost::atomic_bool m_IsEventThreadRunning_1;
    boost::shared_ptr<boost::thread> *m_EventThreadInstance_1;

    bool onIP1Changed = false;
    bool onIP2Changed = false;
    QString chrony_sources;
    QString chrony_clients;
    QStringList chrony_sources_list;
    QStringList chrony_clients_list;

    int ipResetCount = 0;
    void ipResetCountOnchanged();
private slots:
    void getSystemPage(QWebSocket *webSender);
    void getNetworkPage(QWebSocket *webSender);
    void getNtpPage(QWebSocket *pSender);
    void restartnetwork(QString networkName);
    int rebootSystem();
    void updateFirmware();
    void updateNetwork(uint8_t DHCP, QString LocalAddress, QString Netmask, QString Gateway, QString DNS1, QString DNS2,QString phyNetworkName);
    void setLocation(QString location);
    void onGPSDataChenged(uint16_t GPSD_Port, QString GPS_Date, QString GPS_Time, double GPS_Lat, double GPS_Long, double GPS_Alt, int GPS_Sat, int GPS_SatUse, bool locked, QString message);
    void checkNetworkCard();
    void lanPlugin(QString phyNetworkName);
    void lanRemove(QString phyNetworkName);
    void gpsLockChenged(bool locked, uint8_t gpsDev);
    void fileETHDetectChanged(const QString & path);

    void deleteServer(QString address);
    void addServerPoolPeer(QString ServerPoolPeer, QString address, QString option);

    void displayThread();
    void displayThread2();
    void setgps_voltage(int gpsnum, int voltage);
};

#endif // ICLOCKDUALGPS_H
