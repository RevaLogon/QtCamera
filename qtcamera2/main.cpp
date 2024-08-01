#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString SINGLE_INSTANCE_KEY = "kuzey_tuncer_omer_faruk";
    QSharedMemory sharedMemory(SINGLE_INSTANCE_KEY);
    if (sharedMemory.attach()) {
        return 1;
    }
    if (!sharedMemory.create(1)) {
        return 1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
