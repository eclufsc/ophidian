#ifndef OPEN_LEFDEF_DIALOG_H
#define OPEN_LEFDEF_DIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace uddac2016 {

namespace Ui {
class OpenLEFDEFDialog;
}

class OpenLEFDEFDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenLEFDEFDialog(MainWindow * parent = 0);
    ~OpenLEFDEFDialog();

signals:
    void accepted(QString,QString);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonLEF_clicked();
    void on_buttonDEF_clicked();

private:
    Ui::OpenLEFDEFDialog *ui;
};

}

#endif // OPEN_LEFDEF_DIALOG_H
