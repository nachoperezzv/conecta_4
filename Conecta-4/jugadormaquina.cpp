#include "jugadormaquina.h"
#include <iostream>
#include "tablero.h"

/**
 * @brief JugadorMaquina::JugadorMaquina Función que se ejecuta en el thread
 */
JugadorMaquina::JugadorMaquina(Jugador *parent): Jugador(parent)
{
    tiempoExcedido = false;
}

void JugadorMaquina::comenzarBusqueda(const int jug)
{
    jugador = jug;
    //Llama a la función Minimax que implementa el algoritmo para calcular la jugada
    minimax(0, jugador);
    columna = mejorJug.col;

    //No borrar esta línea!!
    emit trabajoFinalizado(columna, jugador);
}

void JugadorMaquina::setTiempoExcedido(bool excedido) {
    tiempoExcedido = excedido;
}

/**
 * @brief JugadorMaquina::minimax Se debe determinar la mejor jugada mediante el algoritmo Minimax.
 * El tablero de juego se encuentra en la variable tablero.
 * Al final de la función de la variable columna debe contener la tirada.
 */
int JugadorMaquina::minimax(int nivel_profundidad, int j)
{
    if(!tiempoExcedido){
        if(//condiciones de fin, donde se es hoja
                nivel_profundidad == NIVEL_DEFECTO or
                tablero->cuatroEnRaya()            or
                tablero->tableroLleno()
                ){
            return heuristica();
        }
        else{
            int fila;
            nivel_profundidad++;

            if(j == jugador){
                mejorJug.val = INT_MIN;
                for(int i=0;i<COLS_TABLERO;i++){
                    fila = tablero->comprobarColumna(i);
                    if(fila != -1){
                        tablero->ponerFicha(i,jugador);
                        mejorJug = MAX(minimax(nivel_profundidad,giveOponent()), mejorJug, i);
                        emit resultadoParcial(mejorJug.col);
                        tablero->cambiarCasilla(fila,i,0);
                    }
                }
                return mejorJug.val;
            }

            else{
                mejorOpo.val = INT_MAX;
                for(int i=0;i<COLS_TABLERO;i++){
                    fila = tablero->comprobarColumna(i);
                    if(fila != -1){
                        tablero->ponerFicha(i,giveOponent());
                        mejorOpo = MIN(minimax(nivel_profundidad,jugador), mejorOpo, i);
                        emit resultadoParcial(mejorOpo.col);
                        tablero->cambiarCasilla(fila,i,0);
                    }
                }
                return mejorOpo.val;
            }
        }
    }
    else
        return 0;
}

valor_col JugadorMaquina::MAX(int a, valor_col b, int i){
    if(a > b.val){
        b.val = a;
        b.col = i;
    }
    return b;
}

valor_col JugadorMaquina::MIN(int a, valor_col b, int i){
    if(a < b.val){
        b.val = a;
        b.col = i;
    }
    return b;
}

int JugadorMaquina::giveOponent()
{
    if(jugador == 1)
        return 2;
    else
        return 1;
}

bool JugadorMaquina::isFeasible(coord p)
{
    if(p.x < FILAS_TABLERO and p.y < COLS_TABLERO)
        return true;
    else
        return false;
}


int JugadorMaquina::heuristica()
{
    int valorO = 0;
    int valorX = 0;
    int oponente = giveOponent();

    for(int i=0; i<FILAS_TABLERO;i++){
        for(int j=0; j<COLS_TABLERO;j++){
            //Aquí empieza el calculo del tablero para el jugador MAX
            if(tablero->obtenerCasilla(i,j) == jugador){
                for(auto c:pos){    //De la ficha del jugador se revisan las casillas contiguas solo de las posiciones que no se han visitado
                                    //hasta ahora, según la iteración del bucle
                    bool fichasPegadas1 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == 0);                //[O]
                    bool fichasPegadas2 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == jugador);          //[O]       + [O]
                    bool fichasPegadas3 = (isFeasible({2*c.x+i,2*c.y+j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == jugador);  //[O][O]    + [O]
                    bool fichasPegadas4 = (isFeasible({3*c.x+i,3*c.y+j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == jugador);  //[O][O][O] + [O]

                    if(fichasPegadas1){
                        //Para hacer comprobaciones de las fichas que se encuentran aisladas en la dirección asignada.

                        //direccion 1
                        if(c.x==1 and c.y==-1){
                            bool noHayCasillaIzq = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);   //Se comprueba casilla izquierda este vacía
                            bool sePuedePonerIzq = tablero->obtenerCasilla(i,j-1) != 0;

                            if(noHayCasillaIzq and sePuedePonerIzq){
                                /*  [ ][ ][ ]
                                 *  [O][/][ ]
                                 *  [X][O][O]
                                 *  [O][X][O]
                                 */
                                bool noHayCasillaIzqIzq = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);
                                bool siHayCasillaIzqIzq = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador);
                                bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+1,j-2) != 0;

                                if(noHayCasillaIzqIzq and sePuedePonerIzqIzq)
                                    valorO += 500;
                                                    /*  [/][ ][ ]
                                                     *  [O][/][ ]
                                                     *  [X][O][O]
                                                     *  [O][X][O]
                                                     */
                                if(siHayCasillaIzqIzq){
                                    valorO += 1000;
                                                    /*  [O][ ][ ]
                                                     *  [O][/][ ]
                                                     *  [X][O][O]
                                                     *  [O][X][O]
                                                     */
                                    bool siHayCasillaIzqIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador);
                                    bool noHayCasillaIzqIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);
                                    bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;

                                    if(siHayCasillaIzqIzqIzq)
                                        valorO += 20000;
                                                            /*  [O][ ][ ][ ]
                                                             *  [X][O][ ] []
                                                             *  [X][O][/][ ]
                                                             *  [O][X][O][O]
                                                             */
                                    if(noHayCasillaIzqIzqIzq and sePuedePonerIzqIzqIzq)
                                        valorO += 4000;
                                                            /*  [/][ ][ ][ ]
                                                             *  [X][O][ ] []
                                                             *  [X][O][/][ ]
                                                             *  [O][X][O][O]
                                                             */
                                }
                            }
                        }

                        //direccion 2
                        if(c.x==1 and c.y==0){
                            bool noHayCasillaEnc = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);
                            if(noHayCasillaEnc)
                                valorO += 500;
                                                /* [ ][/][ ]
                                                 * [ ][O][ ]
                                                 * [ ][X][ ]
                                                 */
                        }

                        //direccion 3
                        if(c.x==1 and c.y==1){
                            bool noHayCasillaDch = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);   //Se comprueba casilla izquierda este vacía
                            bool sePuedePonerDch = tablero->obtenerCasilla(i,j+1) != 0;

                            if(noHayCasillaDch and sePuedePonerDch){
                                /*  [ ][ ][ ]
                                 *  [O][/][ ]
                                 *  [X][O][O]
                                 *  [O][X][O]
                                 */
                                bool noHayCasillaDchDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);
                                bool siHayCasillaDchDch  = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador);
                                bool sePuedePonerDchDch  = tablero->obtenerCasilla(i+1,j+2) != 0;

                                if(noHayCasillaDchDch and sePuedePonerDchDch)
                                    valorO += 500;
                                                    /*  [/][ ][ ]
                                                     *  [O][/][ ]
                                                     *  [X][O][O]
                                                     *  [O][X][O]
                                                     */
                                if(siHayCasillaDchDch){
                                    valorO += 1000;
                                                    /*  [O][ ][ ]
                                                     *  [O][/][ ]
                                                     *  [X][O][O]
                                                     *  [O][X][O]
                                                     */
                                    bool siHayCasillaDchDchDch = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador);
                                    bool noHayCasillaDchDchDch = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i+2,j+3) != 0;

                                    if(siHayCasillaDchDchDch)
                                        valorO += 20000;
                                                            /*  [O][ ][ ][ ]
                                                             *  [X][O][ ] []
                                                             *  [X][O][/][ ]
                                                             *  [O][X][O][O]
                                                             */
                                    if(noHayCasillaDchDchDch and sePuedePonerDchDchDch)
                                        valorO += 4000;
                                                            /*  [/][ ][ ][ ]
                                                             *  [X][O][ ] []
                                                             *  [X][O][/][ ]
                                                             *  [O][X][O][O]
                                                             */
                                }
                            }
                        }

                        //direccion 4
                        if(c.x==0 and c.y==1){
                            bool noHayCasillaDch = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);
                            bool sePuedePonerDch = (isFeasible({i-1,j+1}) and tablero->obtenerCasilla(i-1,j+1) != 0);

                            if(sePuedePonerDch and noHayCasillaDch){
                                /* [ ][O][/][ ][ ]
                                 * [ ][X][O][X][ ]
                                 */
                                bool noHayCasillaDchDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);
                                bool sePuedePonerDchDch = (isFeasible({i-1,j+2}) and tablero->obtenerCasilla(i-1,j+2) != 0);
                                bool siHayCasillaDchDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador);

                                bool noHayCasillaDchDchDch = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);
                                bool sePuedePonerDchDchDch = (isFeasible({i-1,j+2}) and tablero->obtenerCasilla(i-1,j+2) != 0);

                                if(siHayCasillaDchDch){
                                       /* [ ][O][/][O][ ]
                                        * [ ][X][O][X][ ]
                                        */
                                }
                                if(sePuedePonerDchDch and noHayCasillaDchDch){
                                        /* [ ][O][/][/][ ]
                                         * [ ][X][O][X][ ]
                                         */
                                }
                            }
                        }


                    }

                    if(fichasPegadas2){
                        //Esto es para hacer comprobaciones de fichas pegadas. Es decir, comprueba que que la ficha directamente continua en cada
                        //una de las direcciones indicadas este dentro de los limites del mapa y pertenezca al jugador.

                        // direccion 1
                        if(c.x==1 and c.y==-1){ //Condicion que va a comprobar las fichas disponibles en la direccion señalada

                            bool noHayCasillaIzq = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);         //Se comprueba si la casilla dos diagonales a la izquierda esta vacia
                            bool sePuedePonerIzq = tablero->obtenerCasilla(i,j+c.y) != 0;

                            bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+c.x,j+2*c.y) != 0;
                            bool noHayCasillaIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia
                            bool siHayCasillaIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador);

                            if(noHayCasillaIzq and sePuedePonerIzq){
                                                                    /*  [][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                                     *  [][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                                     *  [][X][X][O]                                                                     *
                                                                     */
                                if(siHayCasillaIzqIzq){ //No se comprueba si hay casilla debajo de esta dado que se presupone que debe existir una debajo para uqe una halla en la posicion comprobada
                                            /*  [O][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                    valorO += 1000;

                                    if(isFeasible({i-1,j+1}) and (tablero->obtenerCasilla(i-1,j+1) == 0 and ((i-2==0 or isFeasible({i-2,j+1})) and tablero->obtenerCasilla(i-2,j+1)!=0)))
                                        valorO += 7000;
                                                            /*  [O][ ][ ][ ][ ]
                                                             *  [O][/][ ][ ][ ]
                                                             *  [X][O][O][ ][ ]
                                                             *  [O][X][O][O][ ]
                                                             *  [X][X][X][X][O]
                                                             */
                                    else
                                        valorO += 3000;
                                                            /*  [O][ ][ ][ ][ ]
                                                             *  [O][/][ ][ ][ ]
                                                             *  [X][O][O][ ][ ]
                                                             *  [O][X][O][O][ ]
                                                             *  [X][X][X][X][/]
                                                             */
                                }
                                if(noHayCasillaIzqIzq and sePuedePonerIzqIzq){
                                            /*  [/][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                    valorO += 3000;
                                }
                            }
                        }

                        // direccion 2
                        if(c.x==1 and c.y==0){
                            //Condición que va a comprar las fichas disponibles en la dirección señalada. En este las colocadas en disposición vertical.

                            bool noHayCasillaEnc = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);       // Se comprueba si la casilla dos posiciones por encima de la marcada esta libre.
                            //bool opTieneCasilEnc = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == oponente);// o si por el contrario esta ocupada por una casilla oponente

                            if(noHayCasillaEnc)
                                valorO += 3000;
                                            /*  [ ][ ][/]
                                             *  [O][X][O]
                                             *  [X][X][O]
                                             */
                            //if(opTieneCasilEnc)
                            //    valorO += 0;
                                            /*  [ ][ ][X]
                                             *  [O][X][O]
                                             *  [X][X][O]
                                             */
                        }

                        // direccion 3
                        if(c.x==1 and c.y==1){
                            //Como direccion 1 pero en vez de comprobar la diagonal izquierda, se comprueba la diagonal derecha
                            bool noHayCasillaDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);         //Se comprueba si la casilla dos diagonales a la izquierda esta vacia
                            bool sePuedePonerDch = tablero->obtenerCasilla(i,j+c.y) != 0;

                            bool sePuedePonerDchDch = tablero->obtenerCasilla(i+c.x,j+2*c.y) != 0;
                            bool noHayCasillaDchDch = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia
                            bool siHayCasillaDchDch = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador);

                            if(noHayCasillaDch and sePuedePonerDch){
                                                                    /*  [][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                                     *  [][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                                     *  [][X][X][O]                                                                     *
                                                                     */
                                if(siHayCasillaDchDch){ //No se comprueba si hay casilla debajo de esta dado que se presupone que debe existir una debajo para uqe una halla en la posicion comprobada
                                            /*  [O][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                    if(isFeasible({i-1,j+1}) and (tablero->obtenerCasilla(i-1,j-1) == 0 and ((i-2==0 or isFeasible({i-2,j-1})) and tablero->obtenerCasilla(i-2,j-1)!=0)))
                                        valorO += 8000;
                                                            /*  [O][ ][ ][ ][ ]
                                                             *  [O][/][ ][ ][ ]
                                                             *  [X][O][O][ ][ ]
                                                             *  [O][X][O][O][ ]
                                                             *  [X][X][X][X][O]
                                                             */
                                    else
                                        valorO += 4000;
                                                            /*  [O][ ][ ][ ][ ]
                                                             *  [O][/][ ][ ][ ]
                                                             *  [X][O][O][ ][ ]
                                                             *  [O][X][O][O][ ]
                                                             *  [X][X][X][X][/]
                                                             */
                                }
                                if(noHayCasillaDchDch and sePuedePonerDchDch){
                                            /*  [/][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                    valorO += 3000;
                                }
                            }
                        }

                        // direccion 4
                        if(c.x==0 and c.y==1){ //condición que se refiere a sí la posición que se esta comprobando es la de la casilla derecha.

                            bool noHayCasillaIzq = (isFeasible({i-c.x,j-c.y}) and tablero->obtenerCasilla(i-c.x,j-c.y) == 0);
                            bool noHayCasillaDch = (isFeasible({2*c.x+i,2*c.y +j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == 0);
                            bool OpHayCasillaIzq = (isFeasible({i-c.x,j-c.y}) and tablero->obtenerCasilla(i-c.x,j-c.y) == oponente);
                            bool OpHayCasillaDch = (isFeasible({2*c.x+i,2*c.y +j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == oponente);

                            bool noHayCasillaIzqIzq = (isFeasible({i-2*c.x,j-2*c.y}) and tablero->obtenerCasilla(i-2*c.x,j-2*c.y) == 0);
                            bool noHayCasillaDchDch = (isFeasible({3*c.x+i,3*c.y +j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == 0);
                            bool siHayCasillaIzqIzq = (isFeasible({i-2*c.x,j-2*c.y}) and tablero->obtenerCasilla(i-2*c.x,j-2*c.y) == jugador);
                            bool siHayCasillaDchDch = (isFeasible({3*c.x+i,3*c.y +j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == jugador);


                            if(noHayCasillaIzq and noHayCasillaDch){ // [][/][O][O][/][][]
                                valorO += 1000;

                                if(noHayCasillaIzqIzq and noHayCasillaDchDch)  // [/][/][O][O][/][/][]
                                    valorO += 3000;

                                if((siHayCasillaIzqIzq and noHayCasillaDchDch) or (siHayCasillaDchDch and noHayCasillaIzqIzq))
                                                                                // [O][/][O][O][/][/][] o [/][/][O][O][/][O][/]
                                    valorO += 8000;
                                if(siHayCasillaIzqIzq and siHayCasillaDchDch)   // [O][/][O][O][/][O][/]
                                    valorO += 20000;

                            }
                            if((OpHayCasillaIzq and noHayCasillaDch) or (noHayCasillaIzq and OpHayCasillaDch)){ // [][X][O][O][/][][]
                                valorO += 500;
                                if(noHayCasillaDchDch)                  // [][X][O][O][/][/][]
                                    valorO += 3000;
                                if(siHayCasillaDchDch)                  // [][X][O][O][/][O][]
                                    valorO += 8000;
                            }
                            if(noHayCasillaIzq and OpHayCasillaDch){    // [][/][O][O][X][][]
                                valorO += 500;
                                if(noHayCasillaIzqIzq)                  // [/][/][O][O][X][][]
                                    valorO += 3000;
                                if(siHayCasillaIzqIzq)                  // [O][/][O][O][X][][]
                                    valorO += 8000;
                            }
                        }
                    }

                    if(fichasPegadas2 and fichasPegadas3){

                    }

                    if(fichasPegadas2 and fichasPegadas3 and fichasPegadas4){
                        valorO += 1000000;
                    }
                }
            }
            //Aqui acaba el calculo para el tablero del jugador MAX

            //Aqui empieza el calculo para el tablero del jugador MIN
            else if(tablero->obtenerCasilla(i,j) == oponente){
                for(auto c:pos){
                    if(isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == oponente){

                        //direccion: O ->
                        if(c.x==0 and c.y==1){ //condición que se refiere a sí la posición que se esta comprobando
                                               //es la de la casilla derecha.
                            //Se va a comprobar si lo que se tiene es una combinación de dos casillas juntas
                            //y que ambas tengas dos espacios disponibles a cada lado.
                            bool noHayCasillaIzq  = (isFeasible({i-c.x,j-c.y}) and tablero->obtenerCasilla(i-c.x,j-c.y) == 0);
                            bool noHayCasillaDch  = (isFeasible({2*c.x+i,2*c.y +j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == 0);
                            if(noHayCasillaIzq or noHayCasillaDch){
                                //Se comprueba si la casilla a la izquierda y derecha del grupo de dos que se esta
                                //analizando esta vacía.
                                valorX += 100;
                                if(noHayCasillaIzq and noHayCasillaDch){
                                    valorX += 2500;
                                    bool noHayCasillaIzqIzq = (isFeasible({i-2*c.x,j-2*c.y}) and tablero->obtenerCasilla(i-2*c.x,j-2*c.y) == 0);
                                    bool noHayCasillaDchDch = (isFeasible({3*c.x+i,3*c.y +j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == 0);
                                    if(noHayCasillaIzqIzq or noHayCasillaDchDch){
                                        valorX += 5000;
                                        if(noHayCasillaIzqIzq and noHayCasillaDchDch){
                                            valorX += 50000;
                                        }
                                    }
                                }
                            }
                        }//fin direccion: O ->

                        valorX += 100;
                        if(isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == oponente){
                            valorX +=5000;
                            if(isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == oponente){
                                valorX +=20000;
                            }
                        }
                    }
                }
            }
            //Aqui termina el calculo para el tablero del jugador MIN
         }
    }
    return valorO-valorX;
}



