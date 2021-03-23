#include "tablero.h"

Tablero::Tablero(QObject *parent) : QObject(parent)
{
    for(int fila=0;fila<FILAS_TABLERO;fila++)
        for(int col=0;col<COLS_TABLERO;col++)
            tablero[fila][col]=0;
}

Tablero::Tablero(QObject *parent, const Tablero &tab) : QObject(parent)
{
    for(int fila=0;fila<FILAS_TABLERO;fila++)
        for(int col=0;col<COLS_TABLERO;col++)
            tablero[fila][col]=tab.tablero[fila][col];
}


/**
 * @brief Tablero::cambiarCasilla - Cambia el valor de una casilla del tablero
 * @param fila - posición de la fila en el tablero
 * @param col - posición de la columna en el tablero
 * @param valor - nuevo valor que queremos poner en la posición (fila, col)
 */
void Tablero::cambiarCasilla(int fila, int col, int valor)
{
    if(fila>=0 && fila<FILAS_TABLERO && col>=0 && col<COLS_TABLERO)
        tablero[fila][col] = valor;
}

/**
 * @brief Tablero::limpiarTablero - Limpia el tablero. Deja todas las casillas vacías.
 */
void Tablero::limpiarTablero()
{
    for(int fila=0;fila<FILAS_TABLERO;fila++)
        for(int col=0;col<COLS_TABLERO;col++)
            tablero[fila][col]=0;
}

/**
 * @brief Tablero::tableroLleno - Indica si el tablero está lleno
 * @return true cuando ninguna casilla del tablero tiene valor 0
 */
bool Tablero::tableroLleno()
{
    bool lleno = true;
    for(int fila=FILAS_TABLERO-1;fila>=0 && lleno;fila--)
        for(int col=COLS_TABLERO-1;col>=0 && lleno; col--)
            if(tablero[fila][col]==0) lleno=false;

    return lleno;
}

/**
 * @brief Tablero::cuatroEnRaya - Comprueba si se ha hecho 4 en raya
 * Realiza la búsqueda en horizontal, en vertical y en diagonal
 * @return 0 no hay 4 en raya. 1 ha hecho 4 en raya el jugador 1. 2 ha hecho 4 en raya el jugador 2 .
 */
int Tablero::cuatroEnRaya()
{
    //Casilla por la que busca
    int casilla;
    //Jugador que a hecho el 4 en raya
    int ganador = 0;

    //Indica que ha encontrado 4 en raya
    bool fin = false;

    int fila = 0;
    int col = 0;

    while ((!fin) && (fila < FILAS_TABLERO))
    {
        col = 0;
        while ((!fin) && (col < COLS_TABLERO))
        {
            //Para cada casilla del tablero
            casilla = tablero[fila][col];
            //Si la casilla no está vacía comprueba si pertenece a un 4 en raya
            if(casilla != 0)
            {
                //Realiza una búsqueda en horizontal
                if(col+3 < COLS_TABLERO)
                    if ((tablero[fila][col+1]==casilla) && (tablero[fila][col+2]==casilla) && (tablero[fila][col+3]==casilla))
                    {
                        ganador = casilla;
                        fin = true;
                    }

                //Realiza una búsqueda en vertical
                if(fila+3 < FILAS_TABLERO)
                    if((tablero[fila+1][col] == casilla) && (tablero[fila+2][col] == casilla) && (tablero[fila+3][col]== casilla))
                    {
                        ganador = casilla;
                        fin = true;
                    }

                //Realiza una búsqueda diagonal. Debe buscar en las 2 diagonales a partir de una casilla
                if(fila+3 < FILAS_TABLERO)
                {
                    //Realiza una búsqueda en la primera diagonal
                    if(col-3 >= 0)
                    {
                        if((tablero[fila+1][col-1] == casilla) && (tablero[fila+2][col-2] == casilla) && (tablero[fila+3][col-3]== casilla))
                        {
                            ganador = casilla;
                            fin = true;
                        }
                    }

                    //Realiza una búsqueda en la segunda diagonal
                    if(col+3 < COLS_TABLERO)
                    {
                        if ((tablero[fila+1][col+1]==casilla) && (tablero[fila+2][col+2]==casilla) && (tablero[fila+3][col+3]==casilla))
                        {
                            ganador = casilla;
                            fin = true;
                        }
                    }

                }
           }
           col++;
       }
       fila++;
    }
    return ganador;

}

/**
 * @brief Tablero::ponerFicha - Inserta una ficha en el tablero, en la columna especificada.
 * @param col - columna donde se va a poner la ficha
 * @param jugador - jugador que pone la ficha
 * @return - 0 si la ficha se inserta correctamente. 1 si la columna en la que se intenta colocar está llena. -1 en caso de error.
 */
int Tablero::ponerFicha(int col, int jugador)
{
    bool encontrado = false;

    //Comprueba que la columna pasada como parámetro sea correcta
    if ((col < 0) || (col >= COLS_TABLERO))
       return -1;

    //Comprueba la primera casilla libre de esa columna (si tiene)
    for (int fila=0;(fila<FILAS_TABLERO && !encontrado);fila++)
    {
        if(tablero[fila][col] == 0)
        {
            encontrado = true;
            //Si encuentra una casilla libre, pone la ficha
            tablero[fila][col] = jugador;
        }
    }

    //Si cuando sale del for no ha encontrado ninguna casilla libre es que la columna está llena
    if(!encontrado)
        return 1;
    else
        return 0;

}

/**
 * @brief Tablero::comprobarColumna - Comprueba si la columna que se le pasa como parámetro es correcta para una jugada.
 * @param col - columna a comprobar
 * @return fila en la que le se colocaría la ficha o -1 en caso de error.
 */
int Tablero::comprobarColumna(int col)
{
    int fila = -1;
    bool encontrado = false;

    //Comprueba que la columna pasada como parámetro sea correcta. Si no lo es, fuerza a no entrar en el siguiente bucle
    if ((col < 0) || (col >= COLS_TABLERO))
    {
       encontrado = true;
    }

    //Comprueba la primera casilla libre de esa columna (si tiene)
    for (int i=0;(i<FILAS_TABLERO && !encontrado);i++)
    {
        if(tablero[i][col] == 0)
        {
            fila = i;
            encontrado = true;
        }
    }
    return fila;

}




