#include "Database.h"
#include <QDateTime>
#include <QStringList>
#include <QString>

Database::Database(QString dbName, QString user, QString password, QString host,QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);

    qProcess = new QProcess(this);
}
void Database::restartMysql(){
    system("systemctl stop mysqld");
    system("systemctl start mysqld");

    qDebug() << "Restart MySQL";
}

void Database::initServer()
{
    QString query;

    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
        return;
    }
    QSqlQuery qry;
    QString serveraddress;
    QString type;
    QString option;

    query = QString("SELECT address, type, options FROM server");
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }

    else
    {
        while (qry.next()) {
            serveraddress   = qry.value(0).toString();
            type            = qry.value(1).toString();
            option          = qry.value(2).toString();
            QString command = QString("chronyc add %1 %2 %3").arg(type).arg(serveraddress).arg(option);
            system(command.toStdString().c_str());
        }
    }
    db.close();
}

QString Database::insertNewServer(QString ServerPoolPeer, QString address, QString option)
{
    QString prog = "/bin/bash";
    QStringList arguments;
    arguments << "-c" << QString("chronyc add %1 %2 %3").arg(ServerPoolPeer).arg(address).arg(option);;
    qProcess->start(prog , arguments);
    qProcess->waitForFinished();
    QString ret = QString(qProcess->readAll()).trimmed();
    arguments.clear();
    if (ret.contains("200 OK")){
        QString query = QString("INSERT INTO server (address, type, options) VALUES ('%1', '%2', '%3')")
                .arg(address).arg(ServerPoolPeer).arg(option);
        if (!db.open()) {
            qDebug() << "database error! database can not open.";
            emit mysqlError();
            restartMysql();
            return ret;
        }
    //    qDebug() << query;
        QSqlQuery qry;
        qry.prepare(query);
        if (!qry.exec()){
            qDebug() << qry.lastError();
        }
        db.close();
    }
    return ret;
}

bool Database::deleteServer(QString serverAddress)
{
    QString query;

    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
        return false;
    }
    QSqlQuery qry;

    query = QString("DELETE FROM server WHERE address='%1'").arg(serverAddress);
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }

    db.close();
    return  true;
}

bool Database::database_createConnection()
{
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
        return false;
    }
    db.close();
    qDebug() << "Database connected";

    return true;
}
