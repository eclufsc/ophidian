#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace uddac2016 {

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

}

}

