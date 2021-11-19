#include "db.h"

DB::DB()
{
    // default DB setting
    database = QSqlDatabase::addDatabase("QMYSQL");
    hostName = "localhost";
    port = 3306;
    databaseName = "1111";
    userName = "root";
    password = "0000";

}

void DB::ConnectDB()
{
    // apply default DB setting
    database.setHostName(hostName); // IP 또는 DNS Host name
    database.setPort(port);
    database.setDatabaseName(databaseName); // DB명
    database.setUserName(userName); // 계정 명
    database.setPassword(password); // 계정 Password
    database.open();

    // check default DB setting
    qDebug() << database;
    //database.close();
}

void DB::ConnectNewDB(QSqlDatabase Pdatabase, QString PhostName, int Pport, QString PdatabaseName, QString PuserName, QString Ppassword)
{
    database = Pdatabase;
    hostName = PhostName;
    port = Pport;
    databaseName = PdatabaseName;
    userName = PuserName;
    password = Ppassword;
}

void DB::CheckDB()
{

}

const QSqlDatabase &DB::getDatabase() const
{
    return database;
}

void DB::setDatabase(const QSqlDatabase &newDatabase)
{
    database = newDatabase;
}

const QString &DB::getHostName() const
{
    return hostName;
}

void DB::setHostName(const QString &newHostName)
{
    hostName = newHostName;
}

int DB::getPort() const
{
    return port;
}

void DB::setPort(int newPort)
{
    port = newPort;
}

const QString &DB::getDatabaseName() const
{
    return databaseName;
}

void DB::setDatabaseName(const QString &newDatabaseName)
{
    databaseName = newDatabaseName;
}

const QString &DB::getUserName() const
{
    return userName;
}

void DB::setUserName(const QString &newUserName)
{
    userName = newUserName;
}

const QString &DB::getPassword() const
{
    return password;
}

void DB::setPassword(const QString &newPassword)
{
    password = newPassword;
}
