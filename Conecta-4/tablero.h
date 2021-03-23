#ifndef TABLERO_H
#define TABLERO_H

#include <QObject>

#define FILAS_TABLERO 6
#define COLS_TABLERO 7

class Tablero : public QObject
{
    Q_OBJECT
public:
    explicit Tablero(QObject *parent = 0);
    Tablero(QObject *parent, const Tablero &tab);

    /**
     * @brief existeFicha - Indica si existe una ficha en la posición indicada.
     * @param fila - posición de la fila en el tablero
     * @param col - posición de la columna en el tablaro
     * @return true si el valor del tablero en (fila, col) es distinto de cero, false en otro caso
     */
    inline bool existeFicha(int fila, int col) {return tablero[fila][col]!=0;}

    /**
     * @brief obtenerCasilla - Devuelve a quién pertenece la casilla especificada.
     * @param fila - posición de la fila en el tablero
     * @param col - posición de la columna en el tablaro
     * @return 0 si está vacía, 1 hay ficha del jugador 1, 2 hay ficha del jugador 2
     */
    inline int obtenerCasilla(int fila, int col) {return tablero[fila][col];}

    void cambiarCasilla(int fila, int col, int valor);
    void limpiarTablero();
    bool tableroLleno();
    int cuatroEnRaya();
    int ponerFicha(int col, int jugador);
    int comprobarColumna(int col);

private:
    int tablero[FILAS_TABLERO][COLS_TABLERO];
};

#endif // TABLERO_H
