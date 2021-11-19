#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tetrixboard.h"
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , board(new TetrixBoard)
{

    //ui->stackedWidget->setCurrentIndex(0);

    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);

    socket->connectToHost(QHostAddress::LocalHost,8080);

    if(socket->waitForConnected())
    {
        ui->statusbar->showMessage("Connected to Server! 서버 접속중입니다.");
    }
    else
    {
        QMessageBox::critical(this,"QTCPClient", QString("The following error occurred: %1.").arg(socket->errorString()));
        ui->statusbar->showMessage("Disconnected to Server! 서버 접속 종료되었습니다.");
        exit(EXIT_FAILURE);
    }


    nextPieceLabel = new QLabel;
    nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
    nextPieceLabel->setAlignment(Qt::AlignCenter);
    board->setNextPieceLabel(nextPieceLabel);

    scoreLcd = new QLCDNumber(5);
    scoreLcd->setSegmentStyle(QLCDNumber::Filled);
    //! [1]
    levelLcd = new QLCDNumber(2);
    levelLcd->setSegmentStyle(QLCDNumber::Filled);
    linesLcd = new QLCDNumber(5);
    linesLcd->setSegmentStyle(QLCDNumber::Filled);

    startButton = new QPushButton(tr("&Start"));
    startButton->setFocusPolicy(Qt::NoFocus);
    quitButton = new QPushButton(tr("&Quit"));
    quitButton->setFocusPolicy(Qt::NoFocus);
    pauseButton = new QPushButton(tr("&Pause"));
    //! [2] //! [3]
    pauseButton->setFocusPolicy(Qt::NoFocus);

    connect(startButton, &QPushButton::clicked, board, &TetrixBoard::start);
    //! [4] //! [5]
    connect(quitButton , &QPushButton::clicked, qApp, &QCoreApplication::quit);
    connect(pauseButton, &QPushButton::clicked, board, &TetrixBoard::pause);

    connect(board, SIGNAL(gameEnd()), this, SLOT(requestScoreUpdate()));


#if __cplusplus >= 201402L
    connect(board, &TetrixBoard::scoreChanged, scoreLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged, levelLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged, linesLcd, qOverload<int>(&QLCDNumber::display));
#else
    connect(board, &TetrixBoard::scoreChanged, scoreLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged, levelLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged, linesLcd, QOverload<int>::of(&QLCDNumber::display));
#endif

    //QGridLayout *layout = new QGridLayout;

    ui->gridLayout->addWidget(createLabel(tr("NEXT")), 0, 0);
    ui->gridLayout->addWidget(nextPieceLabel, 1, 0);
    ui->gridLayout->addWidget(createLabel(tr("LEVEL")), 2, 0);
    ui->gridLayout->addWidget(levelLcd, 3, 0);
    ui->gridLayout->addWidget(startButton, 4, 0);
    ui->gridLayout->addWidget(board, 0, 1, 6, 1);
    ui->gridLayout->addWidget(createLabel(tr("SCORE")), 0, 2);
    ui->gridLayout->addWidget(scoreLcd, 1, 2);
    ui->gridLayout->addWidget(createLabel(tr("LINES REMOVED")), 2, 2);
    ui->gridLayout->addWidget(linesLcd, 3, 2);
    ui->gridLayout->addWidget(quitButton, 4, 2);
    ui->gridLayout->addWidget(pauseButton, 5, 2);
    //setLayout(ui->gridLayout);
    //resize(550, 370);

    QPixmap loadingBG(":/111.png");
    ui->loadingBG->setPixmap(loadingBG);
    ui->loadingBG->setPixmap(loadingBG.scaled(960,540, Qt::KeepAspectRatio));

    this->setWindowTitle("Tetrix");

    // 레이블로 추가해서 백그라운드 넣기
    //    QPixmap gameBG(":/3.png");
    //    ui->gridLayout->setPixmap(loadingBG);
    //    ui->gridLayout->setPixmap(loadingBG.scaled(960,540, Qt::KeepAspectRatio));

    ui->loadingBtn->setStyleSheet("QPushButton"
                                  "{"
                                  "background-color : lightblue;"
                                  "}"
                                  "QPushButton::pressed"
                                  "{"
                                  "background-color : red;"
                                  "}"
                                  "QPushButton::pressed"
                                  "{ border-radius: 14px; }");

    QPixmap loginBG(":/22.png");
    ui->loginBG->setPixmap(loginBG);
    ui->loginBG->setPixmap(loginBG.scaled(960,540, Qt::KeepAspectRatio));

    ui->loginBtn->setStyleSheet("QPushButton {background-color : lightblue; border-radius: 20px;}"
                                "QPushButton::pressed {background-color : rgb(216, 99, 99); }");
    ui->inputId_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");
    ui->inputPw_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");


    ui->signinBG->setPixmap(loginBG);
    ui->signinBG->setPixmap(loginBG.scaled(960,540, Qt::KeepAspectRatio));

    ui->requestSignUpBtn->setStyleSheet("QPushButton {background-color : lightblue; border-radius: 20px;}"
                                 "QPushButton::pressed {background-color : rgb(216, 99, 99); }");
    ui->signUpEmail_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");
    ui->signUpPw_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");
    ui->signUpPwCheck_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");
    ui->signUpNickname_2->setStyleSheet("QLineEdit {border-radius: 20px; padding-left:16px;}");


    ui->backBtn2->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->backBtn3->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    ui->backBtn4->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");

    QPixmap rankBG(":/33.jpg");
    ui->rankBG->setPixmap(rankBG);
    ui->rankBG->setPixmap(rankBG.scaled(960,540, Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    if(socket->isOpen())
        socket->close();
    delete ui;
}

QLabel *MainWindow::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    return label;
}

void MainWindow::on_loadingBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_loginBtn_clicked()
{

    QString id = ui->inputId_2->text();
    QString pw = ui->inputPw_2->text();
    qDebug() << "사용자가 입력한 id, pw 는 " << id << ", " << pw << " 입니다.";

    QString str;

    str = id + "," + pw;

    sendPack(socket, "requestLogin", "1", str);

}

void MainWindow::requestScoreUpdate()
{

    QString id = ui->inputId_2->text();
    QString score = QString::number(board->getScore());

    qDebug() << "게임이 종료되었습니다.";
    qDebug() << "게임 점수 업데이트에 사용될 아이디는 " + id + ", " + "점수는 " + score + "입니다.";

    QMessageBox::information(this, "Game End", QString("게임 종료\n점수 : %1").arg(score));

    QString str;

    str = id + "," + score;

    sendPack(socket, "requestUpdateScore", "1", str);
}

void MainWindow::on_backBtn3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_backBtn2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_backBtn4_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::discardSocket()
{
    socket->deleteLater();
    socket=nullptr;

    ui->statusbar->showMessage("Disconnected!");
}


void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "QTCPClient", "The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "QTCPClient", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
    default:
        QMessageBox::information(this, "QTCPClient", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}


void MainWindow::on_requestSignUpBtn_clicked()
{
    QString id = ui->signUpEmail_2->text();
    QString pw = ui->signUpPw_2->text();
    QString nickname = ui->signUpNickname_2->text();

    qDebug() << "사용자가 입력한 아이디는 " << id << "입니다.";
    qDebug() << "사용자가 입력한 비밀번호는 " << pw << "입니다.";
    qDebug() << "사용자가 입력한 닉네임은 " << nickname << "입니다.";

    QString str;

    str = id + "," + pw + "," + nickname;

    sendPack(socket, "requestSignUp", "1", str);

}

void MainWindow::sendPack(QTcpSocket* socket, QString func, QString code, QString result)
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_15);

            QByteArray header;
            header.prepend(QString("requestType:%1,requestCode:%2,fileSize:%3;").arg(func, code).arg(result.size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = result.toUtf8();
            byteArray.prepend(header);

            socketStream.setVersion(QDataStream::Qt_5_15);
            socketStream << byteArray;

            //displayMessage(QString("<font style = ' color: GREEN;'> 고객의 로그인 DB 결과를 전송합니다.</ font>"));
            //displayMessage(QString("<font style = ' color: GREEN;'> 결과값은 (%1) 입니다.</ font>").arg(result));
        }
        else
        {
            QMessageBox::critical(this,"QTCPServer","Socket doesn't seem to be opened");
            //displayMessage(QString("<font style = ' color: RED;'> 매장의 로그인 DB 결과를 전송에 실패했습니다. (0)</ font>"));
        }
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer","Not connected");
        //displayMessage(QString("<font style = ' color: RED;'> 매장의 로그인 DB 결과를 전송에 실패했습니다.(0)</ font>"));
    }
}


void MainWindow::readSocket()
{
    qDebug() << "서버에서 새로운 데이터를 입력받았습니다.";

    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        qDebug() << "데이터 전송을 기다립니다..";
        return;
    }

    QString header = buffer.mid(0,128);

    QStringList list = header.split(",");
    QString requestType = list[0].split(":")[1]; // messege,
    QString requestCode = list[1].split(":")[1]; // user id

    qDebug() << "입력받은 requestType은 " << requestType << "입니다.";
    qDebug() << "전송받은 requestCode는 " << requestCode << "입니다.";

    buffer = buffer.mid(128);

    if(requestType=="returnLogin")
    {
        if(requestCode == "0")
            QMessageBox::information(this, "Login", "통신에 문제가 있습니다.");
        else if(requestCode == "1")
        {
            QMessageBox::information(this, "Login", "로그인 성공");
            ui->stackedWidget->setCurrentIndex(3);
        }
        else if(requestCode == "2")
            QMessageBox::information(this, "Login", "아이디가 없습니다.");
        else if(requestCode == "3")
            QMessageBox::information(this, "Login", "비밀번호가 맞지 않습니다.");

        QString inputId_2 = ui->inputId_2->text();
        QString inputPw_2 = ui->inputPw_2->text();

        qDebug() << "사용자가 입력한 id는 " << inputId_2 << "입니다.";
        qDebug() << "사용자가 입력한 pw는 " << inputPw_2 << "입니다.";

    }
    else if(requestType=="returnSignUp")
    {
        if(requestCode == "0")
            QMessageBox::information(this, "Login", "회원 가입 실패.");
        else if(requestCode == "1")
        {
            QMessageBox::information(this, "L공ogin", "회원 가입 성공");
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
    else if(requestType == "returnScoreRecord")
    {
        if(requestCode == "0")
        {
            //QMessageBox::information(this, "Login", "점수 갱신 실패.");
        }
        else if(requestCode == "1")
        {
            QMessageBox::information(this, "Login", "점수 갱신 성공");
            ui->stackedWidget->setCurrentIndex(4);
        }
    }
    else if(requestType == "returnAllRank")
    {
        if(requestCode == "0")
            QMessageBox::information(this, "Login", "점수 갱신 실패.");
        else if(requestCode == "1")
        {
            QString str = QString::fromStdString(buffer.toStdString());

            qDebug() << "서버에서 갱신된 랭크를 전송받았습니다. : " << str;

            QStringList list = str.split(",");

            QString id, score, datetime;

            int listsize = list.size() - 1;

//            ui->tableWidget->removeRow(ui->tableWidget->rowCount());
//            ui->tableWidget->removeColumn(ui->tableWidget->columnCount());


//            ui->tableWidget->clear();
//            ui->tableWidget->reset();
            ui->tableWidget->setRowCount(0);

            int index = 0;
            for(int j = 0; j < listsize; )
            {

                id = list[j];
                j += 1;
                score = list[j];
                j += 1;
                datetime = list[j];
                j += 1;

                qDebug() << "전송받은 id: " << id << "score: " << score << "datetime: " << datetime;

                ui->tableWidget->insertRow(ui->tableWidget->rowCount()); // Row를 추가합니다.
                //int index = ui->tableWidget->rowCount() - 1;
                //ui->tableWidget->insertRow(j); // Row를 추가합니다.
                //int index = ui->tableWidget->rowCount() - 1;

                ui->tableWidget->setItem(index, 0, new QTableWidgetItem(id));
                ui->tableWidget->setItem(index, 1, new QTableWidgetItem(score));
                ui->tableWidget->setItem(index, 2, new QTableWidgetItem(datetime));
                index++;

            }
        }

    }

}

void MainWindow::on_pushButton_clicked()
{
    sendPack(socket, "requestAllRank", "1", NULL);
}

