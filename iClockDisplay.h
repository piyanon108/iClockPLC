#ifndef ICLOCKDISPLAY_H
#define ICLOCKDISPLAY_H
/*
 * Line 1: iClock NTP Server            (ASCI) [63:0]
 * Line 2: GPS1: searching GPS2: Locked (BOOL) [65:64]
 * Line 3: Source: PPS1                 (ASCI) [129:66]
 * Line 4: L1: 192.168.10.20 L2: 192.168.10.20 (HEX) [139:130]
 */
#include <QObject>
#include <SPI.h>
#include <QString>
#define BUFFERSIZE         50
#define DATESIZE           32
#define TIMESIZE           10
#define DATEINDEX          2
#define TIMEINDEX          34

#define CHECKSUM        (BUFFERSIZE-1)

#define MESSAGEID		(BUFFERSIZE-2)
#define SPIDEV          "/dev/spidev3.0"

class iClockDisplay : public QObject
{
    Q_OBJECT
public:
    explicit iClockDisplay(QObject *parent = nullptr);
    void setTextLineTime(const char *text);
    void setTextLineDate(const char *text);
    void setcurrentDateTime();
    void writeOLEDRun();
signals:

private:
    char *spiDev;
    SPIClass *OLEDSPI;
    uint8_t messageID = 0;
    uint8_t tx_tmp[BUFFERSIZE];
    uint8_t tx[BUFFERSIZE];
    uint8_t rx[BUFFERSIZE];
    bool checkNewData();
    void checkSum();
    void writeOLED();


};

#endif // ICLOCKDISPLAY_H
