#include "openlefdefdialog.h"
#include "ui_openlefdefdialog.h"

#include <QDebug>
#include <QFileDialog>

namespace uddac2016 {

OpenLEFDEFDialog::OpenLEFDEFDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::OpenLEFDEFDialog)
{
    ui->setupUi(this);
    setWindowTitle("Open LEF/DEF");
}

OpenLEFDEFDialog::~OpenLEFDEFDialog()
{

    delete ui;

}

void uddac2016::OpenLEFDEFDialog::on_buttonBox_accepted()
{
    emit accept();
    emit accepted(ui->LEF->text(), ui->DEF->text());
}

void OpenLEFDEFDialog::on_buttonBox_rejected()
{
    emit reject();
}


void OpenLEFDEFDialog::on_buttonLEF_clicked()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open LEF File..."), "", tr("Library Exchange Format (*.lef)"));
    ui->LEF->setText(!text.isEmpty()?text:ui->LEF->text());
}

void OpenLEFDEFDialog::on_buttonDEF_clicked()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open DEF File..."), "", tr("Design Exchange Format (*.def)"));
    ui->DEF->setText(!text.isEmpty()?text:ui->DEF->text());
}

}





