#include "mainwindow.h"
  
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->MyCanvas->setController(mCanvasController);

    /* Connecting signals */
    QObject::connect(&mCanvasController, SIGNAL(changeCircuitBox(QString, size_t, size_t, size_t, size_t)),
                     this, SLOT(on_circuit_labelsChanged(QString, size_t, size_t, size_t, size_t)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionICCAD_2017_triggered()
{

}

void MainWindow::on_actionICCAD_2015_triggered()
{
    DialogLefDef dialog(this);
    QObject::connect(&dialog, SIGNAL(buildICCAD2015(std::string, std::string , std::string)),
                     &mCanvasController, SLOT(buildICCAD2015(std::string, std::string , std::string)));
    dialog.exec();
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_circuit_labelsChanged(QString name, size_t die, size_t cells, size_t pins, size_t nets)
{
    ui->circuitName_2->setText(name);
    ui->circuitDieArea_2->setText(QString::number(die));
    ui->circuitCells_2->setText(QString::number(cells));
    ui->circuitPins_2->setText(QString::number(pins));
    ui->circuitNets_2->setText(QString::number(nets));
    emit update();
}

