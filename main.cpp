#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}
