#include "mainwindow.h"

#include <libxml/xmlversion.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    LIBXML_TEST_VERSION

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
