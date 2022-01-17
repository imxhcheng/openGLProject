#include "Hand_3D.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Hand_3D w;
    w.show();
    return a.exec();
}
