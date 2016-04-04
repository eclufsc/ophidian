#ifndef OPEN_CIRCUIT_DIALOG_H
#define OPEN_CIRCUIT_DIALOG_H

#include <QDialog>
#include <memory>
#include "application.h"
#include "mysfmlcanvas.h"

namespace Ui {
class open_circuit_dialog;
}

class open_circuit_dialog : public QDialog
{
    Q_OBJECT
    mysfmlcanvas * m_canvas;
    std::unique_ptr<application> & m_app;
public:
    explicit open_circuit_dialog(std::unique_ptr<application> & app, mysfmlcanvas * canvas, QWidget *parent = 0);
    ~open_circuit_dialog();

private slots:
    void on_button_v_clicked();

    void on_button_lef_clicked();

    void on_button_def_clicked();

    void on_button_lib_clicked();


    void accept();
    void reject();

private:
    Ui::open_circuit_dialog *ui;
};

#endif // OPEN_CIRCUIT_DIALOG_H
