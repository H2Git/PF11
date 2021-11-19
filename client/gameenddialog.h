#ifndef GAMEENDDIALOG_H
#define GAMEENDDIALOG_H

#include <QDialog>

namespace Ui {
class gameEndDialog;
}

class gameEndDialog : public QDialog
{
    Q_OBJECT

public:
    explicit gameEndDialog(QWidget *parent = nullptr);
    ~gameEndDialog();

    bool getCheck() const;
    void setCheck(bool newCheck);

private slots:
    void on_pushButton_clicked();

private:
    Ui::gameEndDialog *ui;
    int check;
};

#endif // GAMEENDDIALOG_H
