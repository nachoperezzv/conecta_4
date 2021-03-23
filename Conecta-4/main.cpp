#include "conecta4.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication Q4App(argc, argv);
    Conecta4 juego;
    juego.show();
    return Q4App.exec();
}
