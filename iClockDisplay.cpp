#include "iClockDisplay.h"
#include <QDateTime>
#include <QDebug>
#include <QByteArray>
iClockDisplay::iClockDisplay(QObject *parent) : QObject(parent)
{
    OLEDSPI = new SPIClass(SPIDEV);

}
void iClockDisplay::setTextLineTime(const char *text)
{
    bool textEnd = false;
    for(int i=0; i<TIMESIZE; i++)
    {
        if (textEnd == false){
            if (text[i] == '\0') textEnd = true;
            tx[TIMEINDEX+i] = text[i];
        }
        else
            tx[TIMEINDEX+i] = 0x00;
    }
}
void iClockDisplay::setTextLineDate(const char *text)
{
    bool textEnd = false;
    for(int i=0; i<DATESIZE; i++)
    {
        if (textEnd == false){
            if (text[i] == '\0') textEnd = true;
            tx[DATEINDEX+i] = text[i];
        }
        else
            tx[DATEINDEX+i] = 0x00;
    }
}
void iClockDisplay::setcurrentDateTime()
{
    QString timeformat = "hh:mm:ss";
    QString format = "dddd, d MMMM yyyy";
    QString currentTime = QTime::currentTime().toString(timeformat);;
    QString currentDate = QDate::currentDate().toString(format);
    setTextLineTime(currentTime.toStdString().c_str());
    setTextLineDate(currentDate.toStdString().c_str());
}
void iClockDisplay::checkSum()
{
    uint8_t chk = 0;
    for (int i=0;i<BUFFERSIZE-1;i++)
    {
        chk ^= tx[i];
    }
    tx[CHECKSUM] = chk;
}
bool iClockDisplay::checkNewData()
{
    for (int i=0;i<BUFFERSIZE;i++)
    {
        if (tx[i] != tx_tmp[i]){
//            qDebug() << "send new oled data";
            return true;
        }
    }
    return false;
}
void iClockDisplay::writeOLEDRun()
{
    writeOLED();
}

void iClockDisplay::writeOLED()
{
    if (checkNewData() == false) return;
    messageID++;
    tx[MESSAGEID] = messageID;
    tx[0] = 0xAA;
    tx[1] = 0xBB;
    checkSum();
//    QByteArray qByteArray;

    OLEDSPI->send_byte_data(tx,rx,BUFFERSIZE);
    for (int i=0;i<BUFFERSIZE;i++)
    {
        tx_tmp[i] = tx[i];
//        qByteArray.append(tx[i]);
    }
//    qDebug() << qByteArray;
}
