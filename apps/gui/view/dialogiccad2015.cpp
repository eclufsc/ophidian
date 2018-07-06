#include "dialogiccad2015.h"
#include "ui_dialogiccad2015.h"

DialogICCAD2015::DialogICCAD2015(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogICCAD2015)
{
    ui->setupUi(this);
    ui->erro->setText("Por enquanto os arquivos da Lib e SDC são opcionais. (\"\")");
}

DialogICCAD2015::~DialogICCAD2015()
{
    delete ui;
}

void DialogICCAD2015::on_chooseLef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("LEF Files (*.lef)"));

    if (!fileName.isEmpty()) {
        ui->lefLine->setText(fileName);
        ui->lef->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2015::on_chooseDef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("DEF Files (*.def)"));

    if (!fileName.isEmpty()) {
        ui->defLine->setText(fileName);
        ui->def->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2015::on_chooseVerilog_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Verilog Files (*.v)"));

    if (!fileName.isEmpty()) {
        ui->verilogLine->setText(fileName);
        ui->verilog->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2015::on_okButton_clicked()
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
        emit buildICCAD2015(ui->lefLine->text().toStdString(),
                            ui->defLine->text().toStdString(),
                            ui->verilogLine->text().toStdString(),
                            ui->early_lib_line->text().toStdString(),
                            ui->late_lib_line->text().toStdString(),
                            ui->sdc_line->text().toStdString());
        this->close();
    }
}

void DialogICCAD2015::on_cancelButton_clicked()
{
    this->close();
}


void DialogICCAD2015::on_choose_early_lib_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Lib Files (*.lib)"));

    if (!fileName.isEmpty()) {
        ui->early_lib_line->setText(fileName);
        ui->early_lib->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2015::on_choose_late_lib_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Lib Files (*.lib)"));

    if (!fileName.isEmpty()) {
        ui->late_lib_line->setText(fileName);
        ui->late_lib->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogICCAD2015::on_choose_sdc_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("SDC Files (*.sdc)"));

    if (!fileName.isEmpty()) {
        ui->sdc_line->setText(fileName);
        ui->sdc->setStyleSheet("color: rgb(0, 0, 0);");
    }
}
