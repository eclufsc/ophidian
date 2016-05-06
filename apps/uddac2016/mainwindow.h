#ifndef OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
#define OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H

#include <QMainWindow>
#include "controller.h"

namespace uddac2016 {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    controller m_controller;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

private:
    Ui::MainWindow *ui;
};

}

#endif // OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
