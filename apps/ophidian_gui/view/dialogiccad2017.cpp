#include "dialogiccad2017.h"
#include "ui_dialogiccad2017.h"

DialogICCAD2017::DialogICCAD2017(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogICCAD2017)
{
    ui->setupUi(this);
}

DialogICCAD2017::~DialogICCAD2017()
{
    delete ui;
}

void DialogICCAD2017::on_chooseLef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("LEF Files (*.lef)"));

    if (!fileName.isEmpty()) {
        ui->lefLine->setText(fileName);
        ui->lef->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2017::on_chooseDef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("LEF Files (*.lef)"));

    if (!fileName.isEmpty()) {
        ui->defLine->setText(fileName);
        ui->def->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2017::on_chooseVerilog_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("DEF Files (*.def)"));

    if (!fileName.isEmpty()) {
        ui->verilogLine->setText(fileName);
        ui->verilog->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2017::on_okButton_clicked()
{
    bool lef = ui->lefLine->text().isEmpty();
    bool def = ui->defLine->text().isEmpty();
    bool verilog = ui->verilogLine->text().isEmpty();
    QString color;

    if (lef || def || verilog)
    {
        ui->erro->setText("Informe o local dos arquivos em vermelho!");
        if (lef)
        {
            color = "color: rgb(255, 0, 0);";
        } else {
            color = "color: rgb(0, 0, 0);";
        }

        ui->lef->setStyleSheet(color);

        if (def)
        {
            color = "color: rgb(255, 0, 0);";
        } else {
            color = "color: rgb(0, 0, 0);";
        }

        ui->def->setStyleSheet(color);

        if (verilog)
        {
            color = "color: rgb(255, 0, 0);";
        } else {
            color = "color: rgb(0, 0, 0);";
        }

        ui->verilog->setStyleSheet(color);

    } else {
        emit buildICCAD2017(ui->lefLine->text().toStdString(), ui->defLine->text().toStdString(), ui->verilogLine->text().toStdString());
        this->close();
    }
}

void DialogICCAD2017::on_cancelButton_clicked()
{
    this->close();
}

