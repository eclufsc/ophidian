#include "dialoglefdef.h"
#include "ui_dialoglefdef.h"

DialogLefDef::DialogLefDef(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLefDef)
{
    ui->setupUi(this);
}

DialogLefDef::DialogLefDef(QWidget *parent, apps::Application *app) :
    DialogLefDef(parent)
{
    mApp = app;
}

DialogLefDef::~DialogLefDef()
{
    delete ui;
}

void DialogLefDef::on_chooseLef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("LEF Files (*.lef)"));

    if (!fileName.isEmpty()) {
        ui->lefLine->setText(fileName);
        ui->lef->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogLefDef::on_chooseDef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("DEF Files (*.def)"));

    if (!fileName.isEmpty()) {
        ui->defLine->setText(fileName);
        ui->def->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogLefDef::on_chooseVerilog_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Verilog Files (*.v)"));

    if (!fileName.isEmpty()) {
        ui->verilogLine->setText(fileName);
        ui->verilog->setStyleSheet("color: rgb(0, 0, 0);");
    }
}

void DialogLefDef::on_okButton_clicked()
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
        mApp->buildICCAD2015(ui->lefLine->text().toStdString(), ui->defLine->text().toStdString(), ui->verilogLine->text().toStdString());
        this->close();
    }
}

void DialogLefDef::on_cancelButton_clicked()
{
    this->close();
}
