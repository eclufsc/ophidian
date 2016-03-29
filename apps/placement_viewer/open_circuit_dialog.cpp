#include "open_circuit_dialog.h"
#include "ui_open_circuit_dialog.h"

#include <QFileDialog>

open_circuit_dialog::open_circuit_dialog(std::unique_ptr<application> &app, mysfmlcanvas * canvas, QWidget *parent) :
    QDialog(parent),
    m_canvas(canvas),
    m_app(app),
    ui(new Ui::open_circuit_dialog)
{
    ui->setupUi(this);
}

open_circuit_dialog::~open_circuit_dialog()
{
    delete ui;
}

void open_circuit_dialog::on_button_v_clicked()
{
    auto v_file = QFileDialog::getOpenFileName(this, tr("Open Verilog File..."), "", tr("Verilog (*.v)"));
    if(!v_file.isEmpty())
        ui->file_v->setText(v_file);
}

void open_circuit_dialog::on_button_lef_clicked()
{
    auto lef_file = QFileDialog::getOpenFileName(this, tr("Open LEF File..."), "", tr("Library Exchange Format (*.lef)"));
    if(!lef_file.isEmpty())
        ui->file_lef->setText(lef_file);
}

void open_circuit_dialog::on_button_def_clicked()
{
    auto def_file = QFileDialog::getOpenFileName(this, tr("Open DEF File..."), "", tr("Design Exchange Format (*.def)"));
    if(!def_file.isEmpty())
        ui->file_def->setText(def_file);
}

void open_circuit_dialog::accept()
{
    m_app.reset(new application(ui->file_v->text().toStdString(), ui->file_lef->text().toStdString(), ui->file_def->text().toStdString()));

    QDialog::accept();
    m_canvas->load_circuit();
    m_canvas->app(m_app.get());
}

void open_circuit_dialog::reject()
{
    QDialog::reject();
}
