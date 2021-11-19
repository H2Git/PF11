#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // UI setting

    this->setWindowTitle(" Tetrix Server Management System");

    QPixmap main(":/img/44.png");
    ui->mainpageLogo->setPixmap(main);
    ui->mainpageLogo->setPixmap(main.scaled(960,540, Qt::KeepAspectRatio));

    ui->AutoLoginCheckBox->setStyleSheet("margin-left:6;");

    ui->InputID->setStyleSheet("QLineEdit{padding-left:6px;}");
    ui->InputPW->setStyleSheet("QLineEdit{padding-left:6px;}");

    ui->lineEdit_1->setStyleSheet("QLineEdit{padding-left:6px;}");
    ui->lineEdit_2->setStyleSheet("QLineEdit{padding-left:6px;}");
    ui->lineEdit_3->setStyleSheet("QLineEdit{padding-left:6px;}");
    ui->lineEdit_4->setStyleSheet("QLineEdit{padding-left:6px;}");
    ui->lineEdit_5->setStyleSheet("QLineEdit{padding-left:6px;}");

    ui->LoginBox->setFlat(true);
    ui->LoginBox->setStyleSheet("border:0;");

    ui->nextPage->setVisible(false);

    //    ui->CheckDBFrame->setFlat(true);
    //    ui->CheckDBFrame->setStyleSheet("border:0;");

    // DB
    db = new DB();
    m_server = new QTcpServer();

    displayMessage(QString("Tetrix Server서버 관리 프로그램이 시작됩니다."));

    // TCP/IP
    if(m_server->listen(QHostAddress::Any, 8080))
    {
        connect(this, &MainWindow::newMessage, this, &MainWindow::displayMessage);
        connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
        ui->statusbar->showMessage("TCP Server is listening...");
        displayMessage(QString("<font style = ' color: GREEN;'> TCP Server 연결에 성공했습니다.</ font>"));
    }
    else
    {
        //QMessageBox::critical(this,"QTCPServer",QString("Unable to start the server: %1.").arg(m_server->errorString()));
        ui->statusbar->showMessage(QString("Unable to start the server: %1.").arg(m_server->errorString()));
        displayMessage(QString("<font style = ' color: RED;'> TCP Server 연결에 실패했습니다.</ font>"));
        //exit(EXIT_FAILURE);
    }


}

MainWindow::~MainWindow()
{
    // TCP/IP 연결 소캣 제거
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    // 서버 소켓 제거
    m_server->close();
    m_server->deleteLater();

    delete ui;
}

void MainWindow::newConnection()
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());
}

void MainWindow::appendToSocketList(QTcpSocket* socket)
{
    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);
    ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));

    displayMessage(QString("<font style = ' color: GREEN;'> Info :: 접속자 sockd: %1, 서버에 접속했습니다.</ font>").arg(socket->socketDescriptor()));
    displayMessage(QString("<font style = ' color: GREEN;'> Info :: 콤보박스에 접속자 리스트가 업데이트됩니다.</ font>").arg(socket->socketDescriptor()));
    //<font style = ' color: blue;'> Test </ font> string!"
}

void MainWindow::discardSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    if (it != connection_set.end()){
        displayMessage(QString("<font style = ' color: RED;'> Info :: 접속자 sockd: %1, 서버와 연결이 종료됩니다.</ font>").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    refreshComboBox();

    socket->deleteLater();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "QTCPServer", "The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "QTCPServer", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
    default:
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        QMessageBox::information(this, "QTCPServer", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void MainWindow::displayMessage(const QString& str)
{
    ui->textBrowser_receivedMessages->append(str);
}

void MainWindow::refreshComboBox(){

    displayMessage(QString("<font style = ' color: RED;'> Info :: 콤보박스 접속자 리스트가 갱신됩니다.</ font>"));

    ui->comboBox_receiver->clear();
    ui->comboBox_receiver->addItem("Broadcast");
    foreach(QTcpSocket* socket, connection_set)
        ui->comboBox_receiver->addItem(QString::number(socket->socketDescriptor()));
}

void MainWindow::returnSignUp(QTcpSocket* sock)
{
    QString str = QString::fromStdString(buffer.toStdString());

    qDebug() << "전송정보" << str;
    QStringList list = str.split(",");
    QString newID = list[0];
    QString newPW = list[1];
    QString newNickName = list[2];

    QSqlQuery qry;
    qry.exec(QString("INSERT INTO tetrix.`member`(id, pw, nickname, `datetime`)VALUES('%1', '%2', '%3', now())").arg(newID, newPW, newNickName));


    if (qry.numRowsAffected() < 1)
    {
        qDebug() << "회원가입 쿼리 업데이트 실패";

        displayMessage(QString("<font style = ' color: RED;'> 회원가입 업데이트를 실패했습니다. </ font>"));

        sendPack(sock, "returnSignUp", "0", NULL);
    }
    else
    {
        qDebug() << "회원가입 쿼리 업데이트 성공";

        displayMessage(QString("<font style = ' color: GREEN;'> 회원가입입 업데이트를 성공했습니다. </ font>"));

        sendPack(sock, "returnSignUp", "1", NULL);
    }
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

            displayMessage(QString("<font style = ' color: GREEN;'> 고객의 로그인 DB 결과를 전송합니다.</ font>"));
            displayMessage(QString("<font style = ' color: GREEN;'> 결과값은 (%1) 입니다.</ font>").arg(result));
        }
        else
        {
            QMessageBox::critical(this,"QTCPServer","Socket doesn't seem to be opened");
            displayMessage(QString("<font style = ' color: RED;'> 매장의 로그인 DB 결과를 전송에 실패했습니다. (0)</ font>"));
        }
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer","Not connected");
        displayMessage(QString("<font style = ' color: RED;'> 매장의 로그인 DB 결과를 전송에 실패했습니다.(0)</ font>"));
    }
}

void MainWindow::returnScoreRecord(QTcpSocket* sock)
{
    QString str = QString::fromStdString(buffer.toStdString());

    qDebug() << "전달받은 플레이어 아이디와 점수는 : " + str + "입니다.";

    QStringList list = str.split(",");

    QString id, score;

    id = list[0];
    score = list[1];

    qDebug() << "전달받은 id는 : " << id;
    qDebug() << "전달받은 score는 : " << score;
    displayMessage(QString("<font style = ' color: GREEN;'> 전달받은 플레이어 아이디와 점수는 : %1, %2 입니다.</ font>").arg(id, score));

    QSqlQuery qry;
    qry.exec(QString("INSERT INTO tetrix.`rank` (id, score, `datetime`) VALUES('%1', '%2', now())").arg(id, score));
    qDebug() << "사용된 쿼리는 : " << QString("INSERT INTO tetrix.`rank` (id, score, `datetime`) VALUES('%1', '%2', now())").arg(id, score);

    if (qry.numRowsAffected() < 1)
    {
        qDebug() << "쿼리 업데이트 실패";

        displayMessage(QString("<font style = ' color: RED;'> 사용자의 랭크 업데이트를 실패했습니다. </ font>"));

        sendPack(sock, "returnScoreRecord", "0", NULL);
    }
    else
    {
        qDebug() << "쿼리 업데이트 성공, 랭크 전체 결과를 플레이어에게 반환합니다.";

        displayMessage(QString("<font style = ' color: GREEN;'> 사용자의 랭크 업데이트를 성공했습니다. </ font>"));


        sendPack(sock, "returnScoreRecord", "1", NULL);
    }

}


void MainWindow::returnAllRank(QTcpSocket* sock)
{
    bool select = true;
    QString id, score, datetime, qryResult;

    QSqlQuery qry;
    qry.exec(QString("SELECT id, score, `datetime` FROM tetrix.`rank`"));


    if(!(select = qry.next()))
    {
        qDebug() << "주문 검색 결과 없음";
        sendPack(sock, "returnAllRank", "3", NULL);
        return;
    }
    else
    {
        while(select)
        {
            id = qry.value(0).toString();
            score = qry.value(1).toString();
            datetime = qry.value(2).toString();

            qryResult += id + "," + score + "," + datetime + ",";

            select = qry.next();

        }
    }

    qDebug() << "랭크 전체 결과 전송할 데이터는 : " + qryResult + "입니다.";

    sendPack(sock, "returnAllRank", "1", qryResult);
}


void MainWindow::readSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }
    displayMessage(QString("<font style = ' color: GREEN;'> 접속자로부터 요청이 있습니다.</ font>"));

    QString header = buffer.mid(0,128);
    QStringList list = header.split(",");

    QString requestType = list[0].split(":")[1];
    QString requestCode = list[1].split(":")[1];
    qDebug() << "requestType " << requestType;
    qDebug() << "requestCode " << requestCode;
    displayMessage(QString("<font style = ' color: GREEN;'> requestType: %1, requestCode: %2 </ font>").arg(requestType, requestCode));

    buffer = buffer.mid(128);
    qDebug() << "buffer " << buffer;

    if(requestType=="requestLogin")
    {
        displayMessage(QString("<font style = ' color: GREEN;'> 접속자가 요청한 로그인 정보를 확인합니다. </ font>"));
        qDebug() << "접속자가 요청한 로그인 정보를 확인합니다.";
        returnLogin(socket);
    }
    else if(requestType == "requestSignUp")
    {
        returnSignUp(socket);
    }
    else if(requestType=="requestUpdateScore")
    {
        returnScoreRecord(socket);
    }
    else if(requestType=="requestAllRank")
    {
        returnAllRank(socket);
    }
}

void MainWindow::returnLogin(QTcpSocket* sock)
{
    QString str = QString::fromStdString(buffer.toStdString());

    qDebug() << "전달받은 플레이어 id와 pw는 : " + str + "입니다.";

    QStringList list = str.split(",");

    bool select = true;
    QString id, pw, DBPW;

    id = list[0];
    pw = list[1];

    QSqlQuery qry;
    qry.exec(QString("SELECT pw FROM tetrix.`member` WHERE id='" + id + "'"));

    if(!(select = qry.next())) // id mismatched
    {
        qDebug() << "아이디 검색 결과 없음";
        displayMessage(QString("<font style = ' color: RED;'> 고객의 로그인 DB 결과가 없습니다.(3)</ font>"));
        sendPack(sock, "returnLogin", "3", NULL);
        return;
    }
    else
    {
        displayMessage(QString("<font style = ' color: GREEN;'> 고객의 로그인 DB 결과를 찾았습니다.</ font>"));

        while(select)
        {
            DBPW = qry.value(0).toString();
            select = qry.next();
        }

        if(pw != DBPW) // password mismatched
        {
            sendPack(sock, "returnLogin", "2", NULL);
        }
        else // password matched
        {
            sendPack(sock, "returnLogin", "1", NULL);
        }
    }
}



void MainWindow::on_StartServer_clicked()
{
    QString id = ui->InputID->text();
    QString pw = ui->InputPW->text();

    if (id == "manager" && pw == "manager") // manager no need DB member data
    {
        QMessageBox::information(this, "Login", "ID and Passwod is Correct :) \nWelcome Manager!");
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {

        QMessageBox::information(this, "Login", "Login Fail T.T");
    }

    db->ConnectDB();

    ui->lineEdit_1->setPlaceholderText("현재값: " + db->getHostName());
    ui->lineEdit_2->setPlaceholderText("현재값: " + QString::number(db->getPort()));
    ui->lineEdit_3->setPlaceholderText("현재값: " + db->getDatabaseName());
    ui->lineEdit_4->setPlaceholderText("현재값: " + db->getUserName());
    ui->lineEdit_5->setPlaceholderText("현재값: " + db->getPassword());

    QSqlDatabase db = QSqlDatabase::database();


    if(!db.open())
    {
        QPixmap DBBG_error(":/img/55.png");
        ui->DBBG->setPixmap(DBBG_error);
        ui->DBBG->setPixmap(DBBG_error.scaled(960,540, Qt::KeepAspectRatio));
        ui->ResetDB->setStyleSheet("color: rgb(164, 0, 0); background-color: rgb(239, 41, 41); border-color: rgb(196, 160, 0);");
        ui->ResetDB->setText("DB 연결 실패");

        ui->textBrowser_receivedMessages->append(QString("<font style = ' color: GREEN;'> DB 연결 실패. </ font>"));

        // DB 종료
        db.close();
        QSqlDatabase::removeDatabase(db.connectionName());
    }
    else
    {
        QPixmap DBBG_connected(":/img/66.png");
        ui->DBBG->setPixmap(DBBG_connected);
        ui->DBBG->setPixmap(DBBG_connected.scaled(960,540, Qt::KeepAspectRatio));
        //ui->ResetDB->setStyleSheet("background-color: rgb(102, 204, 255);");
        ui->ResetDB->setText("DB 연결 성공");


        ui->textBrowser_receivedMessages->append(QString("<font style = ' color: GREEN;'> DB 연결 성공. </ font>"));

    }
}

void MainWindow::on_ResetDB_clicked()
{
    QSqlDatabase database;
    QString hostName;
    QString port;
    QString databaseName;
    QString userName;
    QString password;
    \
    // 초기값 확인
    //    qDebug() << " AA: " << ui->lineEdit_1->text().isEmpty();
    //    qDebug() << " AA: " << ui->lineEdit_2->text().isEmpty();
    //    qDebug() << " AA: " << ui->lineEdit_2->text().toInt();
    //    qDebug() << " AA: " << ui->lineEdit_4->text().isEmpty();
    //    qDebug() << " AA: " << ui->lineEdit_5->text().isEmpty();

    database = QSqlDatabase::addDatabase("QMYSQL");
    db->setDatabase(database);

    if(!(ui->lineEdit_1->text().isEmpty()))
    {
        hostName = ui->lineEdit_1->text();
        db->setHostName(hostName);
        ui->lineEdit_1->setPlaceholderText("현재값: " + db->getHostName());
    }
    if(0 != (ui->lineEdit_2->text().toInt()))
    {
        port = ui->lineEdit_2->text();
        db->setPort(port.toInt());
        ui->lineEdit_2->setPlaceholderText("현재값: " + QString::number(db->getPort()));
    }
    if(!(ui->lineEdit_3->text().isEmpty()))
    {
        databaseName = ui->lineEdit_3->text();
        db->setDatabaseName(databaseName);

        ui->lineEdit_3->setPlaceholderText("현재값: " + db->getDatabaseName());
    }
    if(!(ui->lineEdit_4->text().isEmpty()))
    {
        userName = ui->lineEdit_4->text();
        db->setUserName(userName);
        ui->lineEdit_4->setPlaceholderText("현재값: " + db->getUserName());
    }
    if(!(ui->lineEdit_5->text().isEmpty()))
    {
        password = ui->lineEdit_5->text();
        db->setPassword(password);
        ui->lineEdit_5->setPlaceholderText("현재값: " + db->getPassword());
    }

    //    new input 값 확인;
    //    qDebug() << " c: " << db->getHostName();
    //    qDebug() << " c: " << db->getPort();
    //    qDebug() << " c: " << db->getDatabaseName();
    //    qDebug() << " c: " << db->getUserName();
    //    qDebug() << " c: " << db->getPassword();

    QSqlDatabase const &temp1 = db->getDatabase();
    db->ConnectDB();

    //    qDebug() << "BBB: "<< temp1;

    if(!temp1.database().open())
    {
        QMessageBox::information(this, "DB 연결", "DB 연결 실패, 재입력해주세요.");
        ui->lineEdit_1->setStyleSheet("color: rgb(239, 41, 41); padding-left:6px;");
        ui->lineEdit_2->setStyleSheet("color: rgb(239, 41, 41); padding-left:6px;");
        ui->lineEdit_3->setStyleSheet("color: rgb(239, 41, 41); padding-left:6px;");
        ui->lineEdit_4->setStyleSheet("color: rgb(239, 41, 41); padding-left:6px;");
        ui->lineEdit_5->setStyleSheet("color: rgb(239, 41, 41); padding-left:6px;");

        ui->ResetDB->setText("DB 연결 실패");
        QPixmap DBBG_error(":/img/55.png");
        ui->DBBG->setPixmap(DBBG_error);
        ui->DBBG->setPixmap(DBBG_error.scaled(960,540, Qt::KeepAspectRatio));
    }
    else
    {
        QMessageBox::information(this, "DB 연결", "DB 연결 성공 :)");
        ui->ResetDB->setStyleSheet("background-color: rgb(102, 204, 255);");
        ui->ResetDB->setText("DB 연결 성공");

        ui->textBrowser_receivedMessages->append(QString("<font style = ' color: GREEN;'> DB 연결 성공</ font>"));

        ui->lineEdit_1->setStyleSheet("color: rgb(52, 101, 164); padding-left:6px;");
        ui->lineEdit_2->setStyleSheet("color: rgb(52, 101, 164); padding-left:6px;");
        ui->lineEdit_3->setStyleSheet("color: rgb(52, 101, 164); padding-left:6px;");
        ui->lineEdit_4->setStyleSheet("color: rgb(52, 101, 164); padding-left:6px;");
        ui->lineEdit_5->setStyleSheet("color: rgb(52, 101, 164); padding-left:6px;");

        QPixmap DBBG_connected(":/img/66.png");
        ui->DBBG->setPixmap(DBBG_connected);
        ui->DBBG->setPixmap(DBBG_connected.scaled(960,540, Qt::KeepAspectRatio));
        ui->nextPage->setVisible(true);
    }

    ui->lineEdit_1->setPlaceholderText("현재값: " + db->getHostName());
    ui->lineEdit_2->setPlaceholderText("현재값: " + QString::number(db->getPort()));
    ui->lineEdit_3->setPlaceholderText("현재값: " + db->getDatabaseName());
    ui->lineEdit_4->setPlaceholderText("현재값: " + db->getUserName());
    ui->lineEdit_5->setPlaceholderText("현재값: " + db->getPassword());

    ui->lineEdit_1->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
}

void MainWindow::on_nextPage_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}

