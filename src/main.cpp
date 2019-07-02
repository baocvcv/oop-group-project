#include "mainwindow.h"
#include "MixDisplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(300,500);
    w.show();

    return a.exec();
}
