#ifndef JUGADOR_H
#define JUGADOR_H

#include <QObject>
#include "tablero.h"

class Jugador : public QObject
{
    Q_OBJECT
public:
    explicit Jugador(QObject *parent = nullptr);

    int getColumna() const;

    int getJugador() const;

    bool getFin() const;
    void setFin(bool value);

    void asignarTablero(Tablero *t);

public slots:
    virtual void comenzarBusqueda(const int jug)
    {
        jugador = jug;
        emit trabajoFinalizado(0, jugador);
    }

signals:
    void trabajoFinalizado(const int, const int);
    void resultadoParcial(const int);

protected:
    int columna;
    int jugador;
    bool fin;

    Tablero *tablero;
};

#endif // JUGADOR_H
