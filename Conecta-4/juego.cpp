#include <qtimer.h>
#include <qapplication.h>
#include <QWidget>
#include <QDebug>
#include "juego.h"
#include <iostream>

using namespace std::chrono;

Juego::Juego(QObject *parent, int tMaximo, ModoJuego modoJ, int turnoJugador) :
    QObject(parent), tiempoMaximo(tMaximo), modoJuego(modoJ), turno(turnoJugador)
{
    tablero = new Tablero;

    jugando = false;
    tiempoExcedido = false;

    //creamos el jugador controlado por la IA
    jugadorIA = new JugadorMaquina(nullptr);
    connect(&workerThread, &QThread::finished, jugadorIA, &QObject::deleteLater);
    connect(this, &Juego::comenzarBusqueda, jugadorIA, &JugadorMaquina::comenzarBusqueda);
    connect(jugadorIA, &JugadorMaquina::trabajoFinalizado, this, &Juego::manejarResultadoJugador);
    connect(jugadorIA, &Jugador::resultadoParcial, this, &Juego::comunicarJugadaParcial);
    //hacemos que ejecute en un hilo distinto
    jugadorIA->moveToThread(&workerThread);
    workerThread.start();

    //timer para controlar la ejecución máxima
    timerTiempoMaximo = new QTimer( this );
    timerTiempoMaximo->setSingleShot(true);
    connect( timerTiempoMaximo, SIGNAL(timeout()), this, SLOT(setTiempoExcedido()) );
}

bool Juego::getJugando() const
{
    return jugando;
}

int Juego::getTurno() const
{
    return turno;
}

int Juego::getTiempoMaximo() const
{
    return tiempoMaximo;
}

void Juego::setTiempoMaximo(int value)
{
    tiempoMaximo = value;
}

Juego::MensajeJuego Juego::getMensaje() const
{
    return mensaje;
}

void Juego::setMensaje(const MensajeJuego &value)
{
    mensaje = value;
}

Juego::ModoJuego Juego::getModoJuego() const
{
    return modoJuego;
}

void Juego::setModoJuego(const int value)
{
    modoJuego = (Juego::ModoJuego)value;
}

void Juego::comenzarJuego()
{
    //Indica que ya empieza el juego.
    jugando = true;


    //limpiamos el tablero
    tablero->limpiarTablero();

    //Indica que se muestre el mensaje de turno.
    turno = 1;
    esJugadorHumano = (modoJuego==HUMANOvsHUMANO || modoJuego==HUMANOvsIA);
    mensaje = Juego::TURNO1;
    emit statusChanged(mensaje);
    if(!esJugadorHumano)
        jugadaMaquina();
}


/**
 * @brief Juego::reiniciarJuego Reinicializa todas las variables para empezar un nuevo juego.
 */
void Juego::reiniciarJuego()
{
    tablero->limpiarTablero();
    turno = 1;
    mensaje = TURNO1;
    emit statusChanged(mensaje);
    jugando = false;

}


/**
 * @brief Juego::jugadaMaquina Crea un thread donde se ejecutará JugadorMaquina para que calcule la jugada.
 * @param jugador JugadorMaquina que está jugando.
 * @return 0 si la ficha se inserta correctamente. 1 si la columna en la que se intenta colocar está llena. -1 en caso de error.
 */
void Juego::jugadaMaquina()
{
    if(jugadorIA==nullptr)
        std::cout<<"Jugador maquina no existe"<<std::endl;
    else {
        //Se duplica el tablero para que no se pueda modificar el tablero de juego desde la clase JugadorMaquina.
        jugadorIA->asignarTablero(new Tablero(nullptr, *tablero));

        //comenzamos el thread del jugadorMaquina
        jugadaParcial = 0;
        jugadaAcabada = false;
        tiempoExcedido = false;
        jugadorIA->setTiempoExcedido(false);
        emit comenzarBusqueda(turno);

        //por ahora, esperamos que termine el hilo...
        timerTiempoMaximo->start(tiempoMaximo);

        //almacenamos "now()" para contar el tiempo que tarda el algoritmo (high_resolution_clock es más preciso que el timer)
        timePointInicioIA = high_resolution_clock::now();

        if (DEBUG)
            qDebug() << "Jugada IA iniciada...";
    }
}

Tablero *Juego::getTablero() const
{
    return tablero;
}

void Juego::setTablero(Tablero *value)
{
    tablero = value;
}

void Juego::setTiempoExcedido() {
    if(!jugadaAcabada) {
        if (DEBUG)
            qDebug() << "Tiempo máximo excedido:" << tiempoMaximo << "ms";

        tiempoExcedido = true;
        jugadorIA->setTiempoExcedido(true);
    }
}

void Juego::manejarResultadoJugador(const int columna, const int jugador)
{
    int resultado;

    if(!esJugadorHumano) {
        timerTiempoMaximo->stop();
        high_resolution_clock::time_point timePointFinIA = high_resolution_clock::now();

        if (DEBUG) {
            if (!tiempoExcedido) {
                qDebug() << "Jugada IA finalizada";
                qDebug() << "Tiempo:" << duration_cast<milliseconds>(timePointFinIA - timePointInicioIA).count() << "ms";
            }
            qDebug() << "Columna:" << (tiempoExcedido ? jugadaParcial : columna) << (tiempoExcedido ? "(jugada parcial)" : "");
            qDebug() << "-------------------------";
        }
    }

    if(esJugadorHumano || !tiempoExcedido)
    {
        jugadaAcabada = true;
        //Introduce la ficha en el tablero
        resultado = tablero->ponerFicha(columna, jugador);
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            widget->update();
        }
    }
    else {
        resultado = tablero->ponerFicha(jugadaParcial, jugador); //si fuera de tiempo la ficha se pone en la columna reportada cuando acabó el tiempo
    }
    //Si la colocación de la ficha ha sido correcta.
    if(resultado == 0)
    {
        //Comprueba si ha ganado el jugador.
        if(tablero->cuatroEnRaya()!= 0)
        {
            if(turno==1)
               mensaje = GANA1;
            else
                mensaje = GANA2;
            jugando = false;
        }
        else {
            //Comprueba si hay empate.
            if(tablero->tableroLleno())
            {
                mensaje = EMPATE;
                jugando = false;
            }
        }
    }
    else //Si la ficha se coloca incorrectamente el juego termina.
    {
       mensaje = ERRORJUEGO;
       jugando = false;
    }

    //cambio de turno y siguiente jugador
    if(jugando)
    {
        if(turno == 1)
        {
            turno = 2;
            mensaje = TURNO2;
            esJugadorHumano = (modoJuego==HUMANOvsHUMANO || modoJuego==IAvsHUMANO);
        }
        else
        {
            turno = 1;
            mensaje = TURNO1;
            esJugadorHumano = (modoJuego==HUMANOvsHUMANO || modoJuego==HUMANOvsIA);
        }
    }
    emit statusChanged(mensaje);
    if(!esJugadorHumano && jugando)
    {
        if(modoJuego==IAvsIA) //si el modo de juego es IAvsIA añadimos un retardo de 500ms para que se vea como juegan
        {
            QTimer::singleShot(500, [&]{

                jugadaMaquina();
            });
        }
        else
            jugadaMaquina();
    }
}

void Juego::comunicarJugadaParcial(const int jugada)
{
    if (!tiempoExcedido) {
        jugadaParcial = jugada;
    }
}

bool Juego::getEsJugadorHumano() const
{
    return esJugadorHumano;
}
