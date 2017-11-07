#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("PortReader");
    w.setWhatsThis("Made by Håkon Hystad");
    w.show();

    return a.exec();
}
