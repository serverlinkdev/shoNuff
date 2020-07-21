#include "widget.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    w.show();
    return a.exec();
}
