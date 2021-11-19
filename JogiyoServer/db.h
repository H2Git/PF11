#ifndef DB_H
#define DB_H

#include <QtSql/QtSql>
#include <QtSql/QSqlDriver>

class DB
{
public:
    DB();
    void ConnectDB();
    void CheckDB();

    const QSqlDatabase &getDatabase() const;
    void setDatabase(const QSqlDatabase &newDatabase);

    const QString &getHostName() const;
    void setHostName(const QString &newHostName);
    int getPort() const;
    void setPort(int newPort);
    const QString &getDatabaseName() const;
    void setDatabaseName(const QString &newDatabaseName);
    const QString &getUserName() const;
    void setUserName(const QString &newUserName);
    const QString &getPassword() const;
    void setPassword(const QString &newPassword);

private:
    QSqlDatabase database;
    QString hostName;
    int port;
    QString databaseName;
    QString userName;
    QString password;

public slots:
    void ConnectNewDB(QSqlDatabase Pdatabase, QString PhostName, int Pport, QString PdatabaseName, QString PuserName, QString Ppassword);

};

#endif // DB_H
