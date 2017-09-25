#include "./mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mApp()
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionICCAD_2017_triggered()
{
    DialogLefDef dialog(this);
    dialog.exec();
}

void MainWindow::on_actionICCAD_2015_triggered()
{
    printf("2015");
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}
