#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "dialoglefdef.h"
#include "control/maincontroller.h"

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MainController mMainController;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionICCAD_2017_triggered();

    void on_actionICCAD_2015_triggered();

    void on_actionQuit_triggered();

    void on_circuit_labelsChanged(QString name, size_t die, size_t cells, size_t pins, size_t nets);

    void on_selected_cellChanged(QString name, QString type,  double x, double y, int worstSlack);

signals:
    void update();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
