#ifndef OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
#define OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H

#include <QMainWindow>

namespace uddac2016 {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

}

#endif // OPHIDIAN_APPS_UDDAC2016_MAINWINDOW_H
