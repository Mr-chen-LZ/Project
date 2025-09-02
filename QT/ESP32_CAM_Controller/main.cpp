#include "widget.h"

#include <QApplication>
#include "mainwindow.h"
#include "mjpegstreamer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
