#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>

#include <QFileDialog>

namespace uddac2016 {

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_controller(*this)
{
    ui->setupUi(this);
    ui->menuBar->setController(m_controller);
    m_controller.init_canvas_controller(ui->Canvas);
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_button_clicked()
{
    auto text = QFileDialog::getOpenFileName(this, tr("Open Verilog File..."), "", tr("Verilog (*.v)"));
    if(!text.isEmpty())
        m_controller.run_SA(text.toStdString());
}


}

