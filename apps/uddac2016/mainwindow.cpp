#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>

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

}

