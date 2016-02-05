#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Circuit_triggered()
{
    if(!m_open_dialog)
        m_open_dialog.reset(new open_circuit_dialog(m_app, ui->canvas, this));
    m_open_dialog->show();
}
