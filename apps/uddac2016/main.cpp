#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uddac2016::MainWindow w;
    w.show();

    return a.exec();
}
