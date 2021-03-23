#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "tablero.h"
#include "jugadormaquina.h"

using namespace std::chrono;

class Tablero;
class JugadorMaquina;

class Juego : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    const bool DEBUG = true;

    enum MensajeJuego { //Mensaje que se mostrará en el juego:
        TURNO1,         //TURNO1. Turno del jugador 1.
        TURNO2,         //TURNO2. Turno del jugador 2.
        GANA1,          //GANA1. Ha ganado el jugador 1.
        GANA2,          //GANA2. Ha ganado el jugador 2.
        EMPATE,         //3. Empate.
        ERRORJUEGO      //4. Error. Colocación de ficha incorrecta.
    };

    enum ModoJuego {    //Indica el modo de juego.
        HUMANOvsHUMANO, //1. Humano contra humano.
        HUMANOvsIA,     //2. Humano contra máquina.
        IAvsHUMANO,     //3. Humano contra máquina.
        IAvsIA          //4. Máquina contra máquina.
    };


    explicit Juego(QObject *parent = nullptr, int tMaximo=1000, ModoJuego modoJ=HUMANOvsHUMANO, int turnoJugador=1);
//    Juego(int tMaximo, ModoJuego modoJ, int turnoJugador);

    bool getJugando() const;

    int getTurno() const;

    int getTiempoMaximo() const;
    void setTiempoMaximo(int value);

    Juego::MensajeJuego getMensaje() const;
    void setMensaje(const MensajeJuego &value);

    Juego::ModoJuego getModoJuego() const;

    void comenzarJuego();
    void reiniciarJuego();

    Tablero *getTablero() const;
    void setTablero(Tablero *value);

    bool getEsJugadorHumano() const;

public slots:
    void manejarResultadoJugador(const int, const int);
    void comunicarJugadaParcial(const int);
    void setModoJuego(const int value);
    void setTiempoExcedido();

signals:
    void statusChanged(MensajeJuego newMsg);
    void comenzarBusqueda(const int);

private:
    void jugadaMaquina();

    Tablero *tablero;

    bool jugando;

    //Tiempo máximo de respuesta del ordenador
    int tiempoMaximo;

    ModoJuego modoJuego;


    //Para saber a quién le toca el turno, 1 jugador1, 2 jugador2
    int turno;

    MensajeJuego mensaje;

    //el jugador máquina. Podrá jugar como jugador 1 o jugador 2
    JugadorMaquina *jugadorIA;

    //control tiempo maximo
    bool tiempoExcedido, jugadaAcabada;
    int jugadaParcial;


    bool esJugadorHumano;

    QTimer *timerTiempoMaximo;
    high_resolution_clock::time_point timePointInicioIA;

};

#endif // JUEGO_H
