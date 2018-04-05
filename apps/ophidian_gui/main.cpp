#include "view/mainwindow.h"
#include <QApplication>
#include <cstdlib>

int main(int argc, char *argv[])
{
    setenv("LC_NUMERIC","en_US.UTF-8",1);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
