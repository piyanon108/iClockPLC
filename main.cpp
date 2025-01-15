#include <QCoreApplication>
#include <iClockDualGPS.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    iClockDualGPS run;
    return a.exec();
}
