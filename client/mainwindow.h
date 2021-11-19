#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QMetaType>
#include <QString>
#include <QStandardPaths>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QLCDNumber;
class QLabel;
class QPushButton;
class TetrixBoard;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendPack(QTcpSocket* socket, QString func, QString code, QString result);

private slots:
    void on_loadingBtn_clicked();
    void on_loginBtn_clicked();
    void on_pushButton_7_clicked();
    void requestScoreUpdate();
    void on_backBtn3_clicked();
    void on_backBtn2_clicked();
    void on_backBtn4_clicked();
    void on_requestSignUpBtn_clicked();

    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);
    void readSocket();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *createLabel(const QString &text);

    TetrixBoard *board;
    QLabel *nextPieceLabel;
    QLCDNumber *scoreLcd;
    QLCDNumber *levelLcd;
    QLCDNumber *linesLcd;
    QPushButton *startButton;
    QPushButton *quitButton;
    QPushButton *pauseButton;

    QTcpSocket* socket;
};
#endif // MAINWINDOW_H
