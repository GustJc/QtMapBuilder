#include "mainwindow.h"
#include <QApplication>

#include <QGraphicsView>
#include <QGraphicsScene>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString path;
    path = QCoreApplication::applicationDirPath();
    QCoreApplication::addLibraryPath(path);

    MainWindow w;
    w.show();



    return a.exec();
}
