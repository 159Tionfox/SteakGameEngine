#include "steakgameengine.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SteakGameEngine w;
    w.show();
    return a.exec();
}
