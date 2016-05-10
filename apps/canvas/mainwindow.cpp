#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto quad = ui->canvas->drawQuad({0.0, 0.0}, {0.0, 10000.0}, {10000.0, 10000.0}, {10000.0, 0.0});
    ui->canvas->paint(quad, sf::Color::Red);


    ophidian::gui::batch_animation * animation = new ophidian::gui::batch_animation(ui->canvas->quadsBatch(), 120);
    for(int i = 0; i < 4; ++i)
    {
        (*animation)[i].position.x = 1000.0;
        (*animation)[i].position.y = 2000.0;
    }

    ui->canvas->animate(animation);




}

MainWindow::~MainWindow()
{
    delete ui;
}
