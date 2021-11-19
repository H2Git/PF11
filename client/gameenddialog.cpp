#include "gameenddialog.h"
#include "ui_gameenddialog.h"

gameEndDialog::gameEndDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gameEndDialog)
{
    ui->setupUi(this);
}

gameEndDialog::~gameEndDialog()
{
    delete ui;
}

bool gameEndDialog::getCheck() const
{
    return check;
}

void gameEndDialog::setCheck(bool newCheck)
{
    check = newCheck;
}

void gameEndDialog::on_pushButton_clicked()
{
        check = 1;
        return;
}

