#include "mainwindow.h"
#include <QApplication>

#include "resources.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    resources::load_resources();

    MainWindow w;
    w.show();

    return a.exec();
}
