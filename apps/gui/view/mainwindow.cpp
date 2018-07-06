#include "mainwindow.h"
  
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->MyCanvas->setController(mMainController);
    ui->findName->setPlaceholderText("Find cell by name");
    ui->selectedCellName_2->setReadOnly(true);
    ui->selectedCellName_2->setPlaceholderText("Name");
    ui->selectedCellType_2->setReadOnly(true);
    ui->selectedCellType_2->setPlaceholderText("Type");
    ui->selectedCell_w_2->setReadOnly(true);
    ui->selectedCell_w_2->setPlaceholderText("Width");
    ui->selectedCell_h_2->setReadOnly(true);
    ui->selectedCell_h_2->setPlaceholderText("Height");
    mMainController.setCanvas(ui->MyCanvas->canvas());

    QObject::connect(&mMainController, SIGNAL(on_send_circuitChanged(QString, QString, size_t, size_t, size_t)),
                     this, SLOT(on_receive_circuitChanged(QString, QString, size_t, size_t, size_t)));
    QObject::connect(&mMainController, SIGNAL(on_send_cellChanged(QString, QString, double, double, double, double)),
                     this, SLOT(on_receive_cellChanged(QString, QString, double, double, double, double)));

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
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionICCAD_2015_triggered()
{
    DialogICCAD2015 dialog(this);
    QObject::connect(&dialog, SIGNAL(buildICCAD2015(std::string, std::string , std::string)),
                     &mMainController, SLOT(buildICCAD2015(std::string, std::string , std::string)));
    dialog.exec();
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_receive_circuitChanged(QString name, QString die, size_t cells, size_t pins, size_t nets)
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

    ui->selectedCellName_2->setText("");
    ui->selectedCellType_2->setText("");
    ui->selectedCell_w_2->setText("");
    ui->selectedCell_h_2->setText("");
    ui->selectedCell_x_2->setValue(0);
    ui->selectedCell_y_2->setValue(0);
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_receive_cellChanged(QString name, QString type, double width, double height, double x, double y)
{
    QString w = width ? QString::number(width) + "μ" : width == -1? ui->selectedCell_w_2->text() : "";
    QString h = height ? QString::number(height) + "μ" : height == -1? ui->selectedCell_h_2->text() : "";

    if (name.compare("moving"))
    {
        ui->selectedCellName_2->setText(name);
        ui->selectedCellType_2->setText(type);
        ui->selectedCell_w_2->setText(w);
        ui->selectedCell_h_2->setText(h);
    }

    ui->selectedCell_x_2->setValue(x);
    ui->selectedCell_y_2->setValue(y);
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_selectedCellButton_clicked()
{
    double x = ui->selectedCell_x_2->value();
    double y = ui->selectedCell_y_2->value();
    ui->MyCanvas->updatePositionQuad(ophidian::geometry::Point(x, y));
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionSlot_1_triggered()
{
    mMainController.slot1();
    on_findNameButtonClear_clicked();
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionSlot_2_triggered()
{
    mMainController.slot2();
    on_findNameButtonClear_clicked();
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionSlot_3_triggered()
{
    mMainController.slot3();
    on_findNameButtonClear_clicked();
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_findNameButton_clicked()
{
    bool find = ui->MyCanvas->findCellEvent(ui->findName->text());

    if (find)
        ui->findErro->setText("Cell found");
    else
        ui->findErro->setText("Cell not found");
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_findNameButtonClear_clicked()
{
    ui->findName->clear();
    ui->findErro->clear();
    ui->MyCanvas->OnUpdate();
}

void MainWindow::on_actionPNG_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PNG"), "", tr("PNG Format (*.png);;All Files (*)"));

    if (!(fileName.toStdString().find(".png") != std::string::npos)) {
        fileName += ".png";
    }

    ui->MyCanvas->saveToPNG(fileName.toStdString());
}

void MainWindow::on_actionSVG_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save SVG"), "", tr("SVG Format (*.svg);;All Files (*)"));

    if (!(fileName.toStdString().find(".svg") != std::string::npos)) {
        fileName += ".svg";
    }

    ui->MyCanvas->saveToSVG(fileName.toStdString());
}
