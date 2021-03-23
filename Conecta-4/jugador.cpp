#include "jugador.h"

Jugador::Jugador(QObject *parent) : QObject(parent)
{
    jugador = 2;
    columna = -1;
    fin = false;
}

int Jugador::getColumna() const
{
    return columna;
}

int Jugador::getJugador() const
{
    return jugador;
}

bool Jugador::getFin() const
{
    return fin;
}

void Jugador::setFin(bool value)
{
    fin = value;
}

void Jugador::asignarTablero(Tablero *t)
{
    tablero = t;
}
