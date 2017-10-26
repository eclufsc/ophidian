#include "mainwindow.h"
  
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->MyCanvas->setController(mMainController);
    mMainController.setCanvas(ui->MyCanvas->canvas());

    /* Connecting signals */
    QObject::connect(&mMainController, SIGNAL(on_circuit_labelsChanged(QString, QString, size_t, size_t, size_t)),
                     this, SLOT(on_circuit_labelsChanged(QString, QString, size_t, size_t, size_t)));
    QObject::connect(&mMainController, SIGNAL(on_selected_cellChanged(QString, QString, double, double, int)),
                     this, SLOT(on_selected_cellChanged(QString, QString, double, double, int)));
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
                     &mMainController, SLOT(buildICCAD2015(std::string, std::string , std::string)));
    dialog.exec();
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_circuit_labelsChanged(QString name, QString die, size_t cells, size_t pins, size_t nets)
{
    ui->circuitName_2->setText(name);
    ui->circuitDieArea_2->setText(die);
    ui->circuitCells_2->setText(QString::number(cells));
    ui->circuitPins_2->setText(QString::number(pins));
    ui->circuitNets_2->setText(QString::number(nets));

    ui->MyCanvas->reserveMinimumOfQuads(cells);
}

void MainWindow::on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack)
{
    ui->selectedCellName_2->setText(name);
    ui->selectedCellType_2->setText(type);
    ui->selectedCell_x_2->setText(QString::number(x));
    ui->selectedCell_y_2->setText(QString::number(y));
    // ui->selectedCellWSlack_2->setText(QString::number(worstSlack)); //!< Not displaying
}
