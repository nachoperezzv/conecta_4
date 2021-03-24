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
            for(auto c:pos){    //De la ficha del jugador se revisan las casillas contiguas solo de las posiciones que no se han visitado
                                //hasta ahora, según la iteración del bucle
                if(tablero->obtenerCasilla(i,j) == jugador){
                    bool fichasPegadas1 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == 0);                //[O]
                    bool fichasPegadas2 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == jugador);          //[O]       + [O]
                    bool fichasPegadas3 = (isFeasible({2*c.x+i,2*c.y+j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == jugador);  //[O][O]    + [O]
                    bool fichasPegadas4 = (isFeasible({3*c.x+i,3*c.y+j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == jugador);  //[O][O][O] + [O]

                    if(fichasPegadas1){
                        //Para hacer comprobaciones de las fichas que se encuentran aisladas en la dirección asignada.

                        //direccion 1
                        if(c.x==1 and c.y==-1){
                            bool sePuedePonerIzq = tablero->obtenerCasilla(i,j-1) != 0;

                            if(sePuedePonerIzq){
                                /*  [ ][ ][ ]
                                 *  [ ][/][ ]
                                 *  [ ][X][O]
                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                    bool noHayCasillaIzqIzq = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool siHayCasillaIzqIzq = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador;
                                    bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+1,j-2) != 0;

                                    if(noHayCasillaIzqIzq and sePuedePonerIzqIzq)
                                        valorO += 500;
                                                    /*  [ ][ ][ ][ ]
                                                     *  [ ][/][ ][ ]
                                                     *  [ ][X][/][ ]
                                                     *  [ ][O][X][O]
                                                     */
                                    else if(siHayCasillaIzqIzq){
                                                        /*  [ ][ ][ ][ ]
                                                         *  [ ][O][ ][ ]
                                                         *  [ ][X][/][ ]
                                                         *  [ ][O][X][O]
                                                         */
                                        bool siHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;
                                        bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                        bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;

                                        if(siHayCasillaIzqIzqIzq)
                                            valorO += 30000;
                                                                /*  [O][ ][ ][ ]
                                                                 *  [X][O][ ] []
                                                                 *  [X][O][/][ ]
                                                                 *  [O][X][O][O]
                                                                 */
                                        else if(noHayCasillaIzqIzqIzq and sePuedePonerIzqIzqIzq)
                                            valorO += 8000;
                                                                /*  [/][ ][ ][ ]
                                                                 *  [X][O][ ] []
                                                                 *  [X][O][/][ ]
                                                                 *  [O][X][O][O]
                                                                 */

                                    }
                                }
                                else
                                    valorO += 100;
                            }
                        }

                        //direccion 2
                        if(c.x==1 and c.y==0){
                            //Esta condición ya es probada en fichasPegadas1
                            valorO += 500;
                                                /* [ ][/][ ]
                                                 * [ ][O][ ]
                                                 * [ ][X][ ]
                                                 */
                        }

                        //direccion 3
                        if(c.x==1 and c.y==1){
                            //bool noHayCasillaDch = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);   //Se comprueba casilla izquierda este vacía
                            bool sePuedePonerDch = tablero->obtenerCasilla(i,j+1) != 0;

                            if(sePuedePonerDch){
                                /*  [ ][ ][ ]
                                 *  [ ][/][ ]
                                 *  [O][X][ ]
                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y})){
                                    //bool noHayCasillaDchDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);
                                    //bool siHayCasillaDchDch  = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador);
                                    bool noHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool siHayCasillaDchDch  = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador;
                                    bool sePuedePonerDchDch  = tablero->obtenerCasilla(i+1,j+2) != 0;

                                    if(noHayCasillaDchDch and sePuedePonerDchDch)
                                        valorO += 500;
                                                        /*  [ ][ ][/]
                                                         *  [ ][/][X]
                                                         *  [O][X][O]
                                                         */
                                    else if(siHayCasillaDchDch){
                                                        /*  [ ][ ][O]
                                                         *  [ ][/][X]
                                                         *  [O][X][O]
                                                         */
                                        if(isFeasible({i+3*c.x,j+3*c.y})){

                                            bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;
                                            bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                            bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i+2,j+3) != 0;

                                            if(siHayCasillaDchDchDch)
                                                valorO += 30000;
                                                                    /*  [ ][ ][ ][O]
                                                                     *  [ ][ ][O][X]
                                                                     *  [ ][/][X][X]
                                                                     *  [O][X][O][O]
                                                                     */
                                            else if(noHayCasillaDchDchDch and sePuedePonerDchDchDch)
                                                valorO += 8000;
                                                                    /*  [ ][ ][ ][/]
                                                                     *  [ ][ ][O][X]
                                                                     *  [ ][/][X][X]
                                                                     *  [O][X][O][O]
                                                                     */
                                        }
                                    }
                                }
                                else
                                    valorO += 500;
                            }
                        }

                        //direccion 4
                        if(c.x==0 and c.y==1){
                            bool sePuedePonerDch =  (isFeasible({i-1,j+1}) and tablero->obtenerCasilla(i-1,j+1)!=0) or i==0;

                            if(sePuedePonerDch){
                                                /* [ ][ ]    [O][/]
                                                 * [O][/] o  [X][X]
                                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                                        /* [ ][ ][ ][ ]    [O][/][ ][ ]
                                                         * [O][/][ ][ ] o  [X][X][ ][ ]
                                                         */
                                    bool noHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool sePuedePonerDchDch = tablero->obtenerCasilla(i-1,j+2) != 0 or i==0;
                                    bool siHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador;

                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i-1,j+3) != 0 or i==0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;

                                    if(siHayCasillaDchDch){
                                           /* [ ][O][/][O][ ]
                                            * [ ][X][O][X][ ]
                                            */
                                        if(siHayCasillaDchDchDch)
                                            valorO += 30000;
                                                   /* [ ][O][/][O][O]
                                                    * [ ][X][O][X][X]
                                                    */
                                        else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                            valorO += 8000;
                                                    /* [ ][O][/][O][/]
                                                     * [ ][X][O][X][X]
                                                     */
                                    }
                                    else{
                                        if(sePuedePonerDchDch and noHayCasillaDchDch)
                                            /* [ ][O][/][/][ ]
                                             * [ ][X][O][X][ ]
                                             */
                                            if(siHayCasillaDchDchDch)
                                            valorO += 2000;
                                                    /* [ ][O][/][/][O]
                                                     * [ ][X][O][X][X]
                                                     */
                                            else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                                valorO += 500;
                                                    /* [ ][ ][ ][ ]
                                                     * [O][/][/][/]
                                                     */
                                    }
                                }
                            }
                        }
                    }

                    if(fichasPegadas2){
                        //Esto es para hacer comprobaciones de fichas pegadas. Es decir, comprueba que que la ficha directamente continua en cada
                        //una de las direcciones indicadas este dentro de los limites del mapa y pertenezca al jugador.

                        // direccion 1
                        if(c.x==1 and c.y==-1){ //Condicion que va a comprobar las fichas disponibles en la direccion señalada

                            if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                /*  [ ][ ][ ][ ]
                                 *  [ ][ ][ ][ ]
                                 *  [ ][ ][O][ ]
                                 *  [ ][ ][X][O]
                                 */

                                bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+1,j-2) != 0;
                                bool noHayCasillaIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia

                                if(sePuedePonerIzqIzq and noHayCasillaIzqIzq){
                                                /*  [ ][ ][ ][ ]
                                                 *  [ ][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                 *  [ ][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                 *  [ ][X][X][O]                                                                     *
                                                 */
                                    bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                    bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool siHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;

                                    if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                        valorO += 8000;
                                                                /*  [/][ ][ ][ ]
                                                                 *  [X][/][ ][ ]
                                                                 *  [O][X][O][ ]
                                                                 *  [X][X][X][O]
                                                                 */
                                    else if(siHayCasillaIzqIzqIzq)
                                        valorO += 30000;
                                                                /*  [O][ ][ ][ ]
                                                                 *  [X][/][ ][ ]
                                                                 *  [O][X][O][ ]
                                                                 *  [X][X][X][O]
                                                                 */

                                }
                            }
                        }

                        // direccion 2
                        if(c.x==1 and c.y==0){
                            //Condición que va a comprar las fichas disponibles en la dirección señalada. En este las colocadas en disposición vertical.
                            bool noHayCasillaEnc = (isFeasible({i+2,j}) and isFeasible({i+3,j}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);       // Se comprueba si la casilla dos posiciones por encima de la marcada esta libre.

                            if(noHayCasillaEnc) //Se comprueba que habiendo dos juntas en direccion vertical, no solo este libre la que esta inmediatamente arriba, si no que se pueda completar el cuatro en raya sin que quede bloqueado
                                valorO += 8000; //porque se llega al final del tablero
                                            /*  [/]
                                             *  [/]
                                             *  [O]
                                             *  [O]
                                             */
                        }

                        // direccion 3
                        if(c.x==1 and c.y==1){
                            //El algoritmo que sigue es como el de la direccion 1 pero en vez de comprobar la diagonal izquierda, se comprueba la diagonal derecha
                            if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                /*  [ ][ ][ ][ ]
                                 *  [ ][ ][ ][ ]
                                 *  [ ][O][ ][ ]
                                 *  [O][X][ ][ ]
                                 */

                                bool sePuedePonerDchDch = tablero->obtenerCasilla(i+1,j+2) != 0;
                                bool noHayCasillaDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia

                                if(sePuedePonerDchDch and noHayCasillaDchDch){
                                                /*  [ ][ ][ ][ ]
                                                 *  [ ][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                 *  [ ][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                 *  [ ][X][X][O]                                                                     *
                                                 */
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i+2,j+3) != 0;
                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;

                                    if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                        valorO += 8000;
                                                                /*  [ ][ ][ ][/]
                                                                 *  [ ][ ][/][X]
                                                                 *  [ ][O][X][O]
                                                                 *  [O][X][X][O]
                                                                 */
                                    else if(siHayCasillaDchDchDch)
                                        valorO += 30000;
                                                                /*  [ ][ ][ ][O]
                                                                 *  [ ][ ][/][X]
                                                                 *  [ ][O][X][O]
                                                                 *  [O][X][X][O]
                                                                 */

                                }
                            }
                        }

                        // direccion 4
                        if(c.x==0 and c.y==1){ //condición que se refiere a sí la posición que se esta comprobando es la de la casilla derecha.

                            bool sePuedePonerDch =  (isFeasible({i-1,j+2}) and tablero->obtenerCasilla(i-1,j+2)!=0) or i==0;

                            if(sePuedePonerDch){
                                                /* [ ][ ][ ]    [O][O][ ]
                                                 * [O][O][ ] o  [X][X][X]
                                                 */
                                if(isFeasible({i+3*c.x,j+3*c.y})){
                                                        /* [ ][ ][ ][ ]    [O][O][ ][ ]
                                                         * [O][O][ ][ ] o  [X][X][ ][ ]
                                                         */

                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i-1,j+3) != 0 or i==0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador;

                                    if(siHayCasillaDchDchDch)
                                        valorO += 30000;
                                           /* [O][O][/][O]
                                            */
                                    else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                        valorO += 8000;
                                            /* [O][O][/][/]
                                             */
                                }
                            }
                        }
                    }

                    if(fichasPegadas2 and fichasPegadas3){

                        //direccion 1:
                        if(c.x==1 and c.y==-1){

                            if(isFeasible({i+3*c.x,j+3*c.y})){
                                valorO += 5000;
                                /* [ ][ ][ ][ ]
                                 * [ ][O][ ][ ]
                                 * [ ][X][O][ ]
                                 * [ ][X][X][O]
                                 */
                                bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;

                                if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                    valorO += 30000;
                                                /* [/][ ][ ][ ]
                                                 * [X][O][ ][ ]
                                                 * [X][X][O][ ]
                                                 * [X][X][X][O]
                                                 */
                                else
                                    valorO += 4000;
                            }

                        }
                        //direccion 2:
                        if(c.x==1 and c.y==0){
                            //Condición que va a comprar las fichas disponibles en la dirección señalada. En este las colocadas en disposición vertical.
                            bool noHayCasillaEnc = (isFeasible({i+3,j}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);       // Se comprueba si la casilla dos posiciones por encima de la marcada esta libre.

                            if(noHayCasillaEnc) //Se comprueba que habiendo dos juntas en direccion vertical, no solo este libre la que esta inmediatamente arriba, si no que se pueda completar el cuatro en raya sin que quede bloqueado
                                valorO += 30000; //porque se llega al final del tablero
                                            /*  [/]
                                             *  [0]
                                             *  [O]
                                             *  [O]
                                             */
                        }
                        //direccion 3:
                        if(c.x==1 and c.y==1){
                            if(isFeasible({i+3*c.x,j+3*c.y})){
                                valorO += 5000;
                                /* [ ][ ][ ][ ]
                                 * [ ][ ][O][ ]
                                 * [ ][O][X][ ]
                                 * [O][X][X][ ]
                                 */
                                bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;

                                if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                    valorO += 30000;
                                                /* [ ][ ][ ][/]
                                                 * [ ][ ][O][X]
                                                 * [ ][O][X][X]
                                                 * [O][X][X][X]
                                                 */
                                else
                                    valorO += 4000;
                            }
                        }
                        //direccion 4:
                        if(c.x==0 and c.y==1){
                            bool sePuedeCasillaDchDchDch = isFeasible({i,j+3}) and (tablero->obtenerCasilla(i-1,j+3) != 0 or i==0);

                            if(sePuedeCasillaDchDchDch)
                                valorO += 30000;
                                                /* [ ][ ][ ][ ]     [O][O][O][/]
                                                 * [O][O][O][/]  o  [X][X][X][X]
                                                 */
                        }
                    }

                    if(fichasPegadas2 and fichasPegadas3 and fichasPegadas4){
                        valorO += 1000000;
                    }
                }
                //Aqui acaba el calculo para el tablero del jugador MAX

                //Aqui empieza el calculo para el tablero del jugador MIN
                else if(tablero->obtenerCasilla(i,j) == oponente){
                    bool fichasPegadas1 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == 0);                //[O]
                    bool fichasPegadas2 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == oponente);          //[O]       + [O]
                    bool fichasPegadas3 = (isFeasible({2*c.x+i,2*c.y+j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == oponente);  //[O][O]    + [O]
                    bool fichasPegadas4 = (isFeasible({3*c.x+i,3*c.y+j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == oponente);  //[O][O][O] + [O]

                    if(fichasPegadas1){
                        //Para hacer comprobaciones de las fichas que se encuentran aisladas en la dirección asignada.

                        //direccion 1
                        if(c.x==1 and c.y==-1){
                            bool sePuedePonerIzq = tablero->obtenerCasilla(i,j-1) != 0;

                            if(sePuedePonerIzq){
                                /*  [ ][ ][ ]
                                 *  [ ][/][ ]
                                 *  [ ][X][O]
                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                    bool noHayCasillaIzqIzq = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool siHayCasillaIzqIzq = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == oponente;
                                    bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+1,j-2) != 0;

                                    if(noHayCasillaIzqIzq and sePuedePonerIzqIzq)
                                        valorX += 1000;
                                                    /*  [ ][ ][ ][ ]
                                                     *  [ ][/][ ][ ]
                                                     *  [ ][X][/][ ]
                                                     *  [ ][O][X][O]
                                                     */
                                    else if(siHayCasillaIzqIzq){
                                                        /*  [ ][ ][ ][ ]
                                                         *  [ ][O][ ][ ]
                                                         *  [ ][X][/][ ]
                                                         *  [ ][O][X][O]
                                                         */
                                        bool siHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;
                                        bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                        bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;

                                        if(siHayCasillaIzqIzqIzq)
                                            valorX += 50000;
                                                                /*  [O][ ][ ][ ]
                                                                 *  [X][O][ ] []
                                                                 *  [X][O][/][ ]
                                                                 *  [O][X][O][O]
                                                                 */
                                        else if(noHayCasillaIzqIzqIzq and sePuedePonerIzqIzqIzq)
                                            valorX += 10000;
                                                                /*  [/][ ][ ][ ]
                                                                 *  [X][O][ ] []
                                                                 *  [X][O][/][ ]
                                                                 *  [O][X][O][O]
                                                                 */

                                    }
                                }
                                else
                                    valorX += 500;
                            }
                        }

                        //direccion 2
                        if(c.x==1 and c.y==0){
                            //Esta condición ya es probada en fichasPegadas1
                            valorX += 1000;
                                                /* [ ][/][ ]
                                                 * [ ][O][ ]
                                                 * [ ][X][ ]
                                                 */
                        }

                        //direccion 3
                        if(c.x==1 and c.y==1){
                            //bool noHayCasillaDch = (isFeasible({i+c.x,j+c.y}) and tablero->obtenerCasilla(i+c.x,j+c.y) == 0);   //Se comprueba casilla izquierda este vacía
                            bool sePuedePonerDch = tablero->obtenerCasilla(i,j+1) != 0;

                            if(sePuedePonerDch){
                                /*  [ ][ ][ ]
                                 *  [ ][/][ ]
                                 *  [O][X][ ]
                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y})){
                                    //bool noHayCasillaDchDch = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);
                                    //bool siHayCasillaDchDch  = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == jugador);
                                    bool noHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool siHayCasillaDchDch  = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == oponente;
                                    bool sePuedePonerDchDch  = tablero->obtenerCasilla(i+1,j+2) != 0;

                                    if(noHayCasillaDchDch and sePuedePonerDchDch)
                                        valorX += 1000;
                                                        /*  [ ][ ][/]
                                                         *  [ ][/][X]
                                                         *  [O][X][O]
                                                         */
                                    else if(siHayCasillaDchDch){
                                                        /*  [ ][ ][O]
                                                         *  [ ][/][X]
                                                         *  [O][X][O]
                                                         */
                                        if(isFeasible({i+3*c.x,j+3*c.y})){

                                            bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;
                                            bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                            bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i+2,j+3) != 0;

                                            if(siHayCasillaDchDchDch)
                                                valorX += 50000;
                                                                    /*  [ ][ ][ ][O]
                                                                     *  [ ][ ][O][X]
                                                                     *  [ ][/][X][X]
                                                                     *  [O][X][O][O]
                                                                     */
                                            else if(noHayCasillaDchDchDch and sePuedePonerDchDchDch)
                                                valorX += 10000;
                                                                    /*  [ ][ ][ ][/]
                                                                     *  [ ][ ][O][X]
                                                                     *  [ ][/][X][X]
                                                                     *  [O][X][O][O]
                                                                     */
                                        }
                                    }
                                }
                                else
                                    valorX += 1000;
                            }
                        }

                        //direccion 4
                        if(c.x==0 and c.y==1){
                            bool sePuedePonerDch =  (isFeasible({i-1,j+1}) and tablero->obtenerCasilla(i-1,j+1)!=0) or i==0;

                            if(sePuedePonerDch){
                                                /* [ ][ ]    [O][/]
                                                 * [O][/] o  [X][X]
                                                 */
                                if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                                        /* [ ][ ][ ][ ]    [O][/][ ][ ]
                                                         * [O][/][ ][ ] o  [X][X][ ][ ]
                                                         */
                                    bool noHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0;
                                    bool sePuedePonerDchDch = tablero->obtenerCasilla(i-1,j+2) != 0 or i==0;
                                    bool siHayCasillaDchDch = tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == oponente;

                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i-1,j+3) != 0 or i==0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;

                                    if(siHayCasillaDchDch){
                                           /* [ ][O][/][O][ ]
                                            * [ ][X][O][X][ ]
                                            */
                                        if(siHayCasillaDchDchDch)
                                            valorX += 50000;
                                                   /* [ ][O][/][O][O]
                                                    * [ ][X][O][X][X]
                                                    */
                                        else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                            valorX += 10000;
                                                    /* [ ][O][/][O][/]
                                                     * [ ][X][O][X][X]
                                                     */
                                    }
                                    else{
                                        if(sePuedePonerDchDch and noHayCasillaDchDch)
                                            /* [ ][O][/][/][ ]
                                             * [ ][X][O][X][ ]
                                             */
                                            if(siHayCasillaDchDchDch)
                                            valorX += 4000;
                                                    /* [ ][O][/][/][O]
                                                     * [ ][X][O][X][X]
                                                     */
                                            else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                                valorX += 1000;
                                                    /* [ ][ ][ ][ ]
                                                     * [O][/][/][/]
                                                     */
                                    }
                                }
                            }
                        }
                    }

                    if(fichasPegadas2){
                        //Esto es para hacer comprobaciones de fichas pegadas. Es decir, comprueba que que la ficha directamente continua en cada
                        //una de las direcciones indicadas este dentro de los limites del mapa y pertenezca al jugador.

                        // direccion 1
                        if(c.x==1 and c.y==-1){ //Condicion que va a comprobar las fichas disponibles en la direccion señalada

                            if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                /*  [ ][ ][ ][ ]
                                 *  [ ][ ][ ][ ]
                                 *  [ ][ ][O][ ]
                                 *  [ ][ ][X][O]
                                 */

                                bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+1,j-2) != 0;
                                bool noHayCasillaIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia

                                if(sePuedePonerIzqIzq and noHayCasillaIzqIzq){
                                                /*  [ ][ ][ ][ ]
                                                 *  [ ][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                 *  [ ][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                 *  [ ][X][X][O]                                                                     *
                                                 */
                                    bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                    bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool siHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;

                                    if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                        valorX += 10000;
                                                                /*  [/][ ][ ][ ]
                                                                 *  [X][/][ ][ ]
                                                                 *  [O][X][O][ ]
                                                                 *  [X][X][X][O]
                                                                 */
                                    else if(siHayCasillaIzqIzqIzq)
                                        valorX += 50000;
                                                                /*  [O][ ][ ][ ]
                                                                 *  [X][/][ ][ ]
                                                                 *  [O][X][O][ ]
                                                                 *  [X][X][X][O]
                                                                 */

                                }
                            }
                        }

                        // direccion 2
                        if(c.x==1 and c.y==0){
                            //Condición que va a comprar las fichas disponibles en la dirección señalada. En este las colocadas en disposición vertical.
                            bool noHayCasillaEnc = (isFeasible({i+2,j}) and isFeasible({i+3,j}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);       // Se comprueba si la casilla dos posiciones por encima de la marcada esta libre.

                            if(noHayCasillaEnc) //Se comprueba que habiendo dos juntas en direccion vertical, no solo este libre la que esta inmediatamente arriba, si no que se pueda completar el cuatro en raya sin que quede bloqueado
                                valorX += 10000; //porque se llega al final del tablero
                                            /*  [/]
                                             *  [/]
                                             *  [O]
                                             *  [O]
                                             */
                        }

                        // direccion 3
                        if(c.x==1 and c.y==1){
                            //El algoritmo que sigue es como el de la direccion 1 pero en vez de comprobar la diagonal izquierda, se comprueba la diagonal derecha
                            if(isFeasible({i+2*c.x,j+2*c.y}) and isFeasible({i+3*c.x,j+3*c.y})){
                                /*  [ ][ ][ ][ ]
                                 *  [ ][ ][ ][ ]
                                 *  [ ][O][ ][ ]
                                 *  [O][X][ ][ ]
                                 */

                                bool sePuedePonerDchDch = tablero->obtenerCasilla(i+1,j+2) != 0;
                                bool noHayCasillaDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia

                                if(sePuedePonerDchDch and noHayCasillaDchDch){
                                                /*  [ ][ ][ ][ ]
                                                 *  [ ][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                 *  [ ][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                 *  [ ][X][X][O]                                                                     *
                                                 */
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i+2,j+3) != 0;
                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;

                                    if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                        valorX += 10000;
                                                                /*  [ ][ ][ ][/]
                                                                 *  [ ][ ][/][X]
                                                                 *  [ ][O][X][O]
                                                                 *  [O][X][X][O]
                                                                 */
                                    else if(siHayCasillaDchDchDch)
                                        valorX += 50000;
                                                                /*  [ ][ ][ ][O]
                                                                 *  [ ][ ][/][X]
                                                                 *  [ ][O][X][O]
                                                                 *  [O][X][X][O]
                                                                 */

                                }
                            }
                        }

                        // direccion 4
                        if(c.x==0 and c.y==1){ //condición que se refiere a sí la posición que se esta comprobando es la de la casilla derecha.

                            bool sePuedePonerDch =  (isFeasible({i-1,j+2}) and tablero->obtenerCasilla(i-1,j+2)!=0) or i==0;

                            if(sePuedePonerDch){
                                                /* [ ][ ][ ]    [O][O][ ]
                                                 * [O][O][ ] o  [X][X][X]
                                                 */
                                if(isFeasible({i+3*c.x,j+3*c.y})){
                                                        /* [ ][ ][ ][ ]    [O][O][ ][ ]
                                                         * [O][O][ ][ ] o  [X][X][ ][ ]
                                                         */

                                    bool noHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;
                                    bool sePuedePonerDchDchDch = tablero->obtenerCasilla(i-1,j+3) != 0 or i==0;
                                    bool siHayCasillaDchDchDch = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == oponente;

                                    if(siHayCasillaDchDchDch)
                                        valorX += 50000;
                                           /* [O][O][/][O]
                                            */
                                    else if(sePuedePonerDchDchDch and noHayCasillaDchDchDch)
                                        valorX += 10000;
                                            /* [O][O][/][/]
                                             */
                                }
                            }
                        }
                    }

                    if(fichasPegadas2 and fichasPegadas3){

                        //direccion 1:
                        if(c.x==1 and c.y==-1){

                            if(isFeasible({i+3*c.x,j+3*c.y})){
                                valorX += 8000;
                                /* [ ][ ][ ][ ]
                                 * [ ][O][ ][ ]
                                 * [ ][X][O][ ]
                                 * [ ][X][X][O]
                                 */
                                bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;

                                if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                    valorX += 50000;
                                                /* [/][ ][ ][ ]
                                                 * [X][O][ ][ ]
                                                 * [X][X][O][ ]
                                                 * [X][X][X][O]
                                                 */
                                else
                                    valorX += 6000;
                            }

                        }
                        //direccion 2:
                        if(c.x==1 and c.y==0){
                            //Condición que va a comprar las fichas disponibles en la dirección señalada. En este las colocadas en disposición vertical.
                            bool noHayCasillaEnc = (isFeasible({i+3,j}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);       // Se comprueba si la casilla dos posiciones por encima de la marcada esta libre.

                            if(noHayCasillaEnc) //Se comprueba que habiendo dos juntas en direccion vertical, no solo este libre la que esta inmediatamente arriba, si no que se pueda completar el cuatro en raya sin que quede bloqueado
                                valorX += 50000; //porque se llega al final del tablero
                                            /*  [/]
                                             *  [0]
                                             *  [O]
                                             *  [O]
                                             */
                        }
                        //direccion 3:
                        if(c.x==1 and c.y==1){
                            if(isFeasible({i+3*c.x,j+3*c.y})){
                                valorX += 8000;
                                /* [ ][ ][ ][ ]
                                 * [ ][ ][O][ ]
                                 * [ ][O][X][ ]
                                 * [O][X][X][ ]
                                 */
                                bool sePuedePonerIzqIzqIzq = tablero->obtenerCasilla(i+2,j-3) != 0;
                                bool noHayCasillaIzqIzqIzq = tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0;

                                if(sePuedePonerIzqIzqIzq and noHayCasillaIzqIzqIzq)
                                    valorX +=530000;
                                                /* [ ][ ][ ][/]
                                                 * [ ][ ][O][X]
                                                 * [ ][O][X][X]
                                                 * [O][X][X][X]
                                                 */
                                else
                                    valorX += 6000;
                            }
                        }
                        //direccion 4:
                        if(c.x==0 and c.y==1){
                            bool sePuedeCasillaDchDchDch = isFeasible({i,j+3}) and (tablero->obtenerCasilla(i-1,j+3) != 0 or i==0);

                            if(sePuedeCasillaDchDchDch)
                                valorX += 50000;
                                                /* [ ][ ][ ][ ]     [O][O][O][/]
                                                 * [O][O][O][/]  o  [X][X][X][X]
                                                 */
                        }
                    }

                    if(fichasPegadas2 and fichasPegadas3 and fichasPegadas4){
                        valorX += 2000000;
                    }
                }
                //Aqui termina el calculo para el tablero del jugador MIN

            }
            //Aqui acaba la iteracion de direcciones para las casillas
        }
        //Aqui se acaba la iteración de las columnas
    }
    //Aqui se acaba la iteracion de las filas

    return valorO-valorX;
}



