#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// DB
#include "db.h"
#include <QMessageBox>
#include <QWidget>

// TCT/IP
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>

#include <QFile>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void returnLogin(QTcpSocket* sock);
    void returnSignUp(QTcpSocket* sock);
    void returnScoreRecord(QTcpSocket* sock);
    void returnAllRank(QTcpSocket* sock);
    void sendPack(QTcpSocket* socket, QString func, QString code, QString result);

signals:
    void newMessage(QString);

private slots:
    // TCP/IP
    void newConnection();
    void appendToSocketList(QTcpSocket* socket);
    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);
    void displayMessage(const QString& str);
    void refreshComboBox();
    void readSocket();

    // DB
    void on_StartServer_clicked();
    void on_ResetDB_clicked();

    void on_nextPage_clicked();

private:

    Ui::MainWindow *ui;
    DB *db;

    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
    QByteArray buffer;

};
#endif // MAINWINDOW_H
