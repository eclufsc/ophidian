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

    /* Name of Actions */
    ui->actionSlot_1->setText("Slot 1");
    ui->actionSlot_2->setText("Slot 2");
    ui->actionSlot_3->setText("Slot 3");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionICCAD_2017_triggered()
{
    DialogICCAD2017 dialog(this);
    QObject::connect(&dialog, SIGNAL(buildICCAD2017(std::string, std::string , std::string)),
                     &mMainController, SLOT(buildICCAD2017(std::string, std::string , std::string)));
    dialog.exec();

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
    if (name.compare("removing")) {
        ui->circuitName_2->setText(name);
        ui->circuitDieArea_2->setText(die);

        ui->MyCanvas->reserveMinimumOfQuads(cells);

        ophidian::geometry::Point boundaries = mMainController.chipBoundaries();
        ophidian::geometry::Point centerView(boundaries.x()/2.0, boundaries.y()/2.0);

        ui->MyCanvas->centerViewOn(centerView);
        ui->MyCanvas->viewSize(boundaries);
    }

    ui->circuitCells_2->setText(QString::number(cells));
    ui->circuitPins_2->setText(QString::number(pins));
    ui->circuitNets_2->setText(QString::number(nets));
}

void MainWindow::on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack)
{
    if (name.compare("moving"))
    {
        ui->selectedCellName_2->setText(name);
        ui->selectedCellType_2->setText(type);
    }

    ui->selectedCell_x_2->setValue(x);
    ui->selectedCell_y_2->setValue(y);
    // ui->selectedCellWSlack_2->setText(QString::number(worstSlack)); //!< Not displaying
}

void MainWindow::on_pushButton_clicked()
{
    double x = ui->selectedCell_x_2->value();
    double y = ui->selectedCell_y_2->value();
    ui->MyCanvas->updatePositionQuad(ophidian::geometry::Point(x, y));
}

void MainWindow::on_actionSlot_1_triggered()
{
    mMainController.slot1();
}

void MainWindow::on_actionSlot_2_triggered()
{
    mMainController.slot2();
}

void MainWindow::on_actionSlot_3_triggered()
{
    mMainController.slot3();
}