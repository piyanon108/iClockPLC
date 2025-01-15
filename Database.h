#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QtSql>
#include <QString>
#include <QStringList>
#include <QProcess>
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QString dbName, QString user, QString password, QString host, QObject *parent = nullptr);
    void restartMysql();
    bool database_createConnection();
    QString insertNewServer(QString ServerPoolPeer, QString address, QString option);
    bool deleteServer(QString serverAddress);
    void initServer();


signals:
    void mysqlError();
private:
    QProcess *qProcess;
    QSqlDatabase db;
};

#endif // DATABASE_H
