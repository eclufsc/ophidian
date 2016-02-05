#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <application.h>

#include "open_circuit_dialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    std::unique_ptr<application> m_app;
    std::unique_ptr<open_circuit_dialog> m_open_dialog;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_Circuit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
