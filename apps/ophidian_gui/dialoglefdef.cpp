#include "dialoglefdef.h"
#include "ui_dialoglefdef.h"

DialogLefDef::DialogLefDef(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLefDef)
{
    ui->setupUi(this);
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
    }
}

void DialogLefDef::on_chooseDef_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("DEF Files (*.def)"));

    if (!fileName.isEmpty()) {
        ui->defLine->setText(fileName);
    }
}

void DialogLefDef::on_okButton_clicked()
{
    std::cout << "LEF: " << ui->lefLine->text().toStdString() << std::endl;
    std::cout << "DEF: " << ui->defLine->text().toStdString() << std::endl;
    this->close();
}

void DialogLefDef::on_cancelButton_clicked()
{
    this->close();
}
