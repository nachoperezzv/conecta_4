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
                        tablero->cambiarCasilla(fila,i,0);
                    }
                }
                return mejorOpo.val;
            }
        }
    }
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
    else if(jugador == 2)
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
                    bool fichasPegadas2 = (isFeasible({c.x+i,c.y+j}) and tablero->obtenerCasilla(c.x+i,c.y+j) == jugador);          //[O] + [O]
                    bool fichasPegadas3 = (isFeasible({2*c.x+i,2*c.y+j}) and tablero->obtenerCasilla(2*c.x+i,2*c.y+j) == jugador);  //[O] + [O][O]
                    bool fichasPegadas4 = (isFeasible({3*c.x+i,3*c.y+j}) and tablero->obtenerCasilla(3*c.x+i,3*c.y+j) == jugador);  //[O] + [O][O][O]

                    if(fichasPegadas2){
                        //Esto es para hacer comprobaciones de fichas pegadas. Es decir, comprueba que que la ficha directamente continua en cada
                        //una de las direcciones indicadas este dentro de los limites del mapa y pertenezca al jugador.

                        // direccion 1
                        if(c.x==1 and c.y==-1){ //Condicion que va a comprobar las fichas disponibles en la direccion
                                                //señalada
                            //En un primer lugar si hay una combinación posible
                            bool noHayCasillaIzq = (isFeasible({i+2*c.x,j+2*c.y}) and tablero->obtenerCasilla(i+2*c.x,j+2*c.y) == 0);         //Se comprueba si la casilla dos diagonales a la izquierda esta vacia
                            bool sePuedePonerIzq = tablero->obtenerCasilla(i,j+c.y) != 0;

                            bool sePuedePonerIzqIzq = tablero->obtenerCasilla(i+c.x,j+2*c.y) != 0;
                            bool noHayCasillaIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == 0);      //Se comprueba si la casilla tres diagonales a la izquierda esta vacia
                            bool siHayCasillaIzqIzq = (isFeasible({i+3*c.x,j+3*c.y}) and tablero->obtenerCasilla(i+3*c.x,j+3*c.y) == jugador);

                            if(noHayCasillaIzq and sePuedePonerIzq){
                                                                    /*
                                                                     *  [][/][ ][ ] Se comprueba que no haya ficha y que se pueda poner en el siguiente movimiento porque
                                                                     *  [][X][O][ ] tiene una ficha directamente debajo sobre la que apoyarse.
                                                                     *  [][X][X][O]
                                                                     *
                                                                     */
                                if(siHayCasillaIzqIzq){
                                            /*
                                             *  [O][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                }
                                if(noHayCasillaIzqIzq and sePuedePonerIzqIzq){
                                            /*
                                             *  [/][ ][ ][ ]
                                             *  [X][/][ ][ ]
                                             *  [O][X][O][ ]
                                             *  [X][X][X][O]
                                             */
                                }

                            }
                        }

                        // direccion 2
                        if(c.x==1 and c.y==0){

                        }

                        // direccion 3
                        if(c.x==1 and c.y==1){

                        }

                        // direccion 4
                        if(c.x==0 and c.y==1){ //condición que se refiere a sí la posición que se esta comprobando
                                               //es la de la casilla derecha.
                            //Se va a comprobar si lo que se tiene es una combinación de dos casillas juntas
                            //y que ambas tengas dos espacios disponibles a cada lado.
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
                                    valorO += 5000;
                                if(siHayCasillaIzqIzq and siHayCasillaDchDch)   // [O][/][O][O][/][O][/]
                                    valorO += 15000;

                            }
                            if((OpHayCasillaIzq and noHayCasillaDch) or (noHayCasillaIzq and OpHayCasillaDch)){ // [][X][O][O][/][][]
                                valorO += 500;
                                if(noHayCasillaDchDch)                  // [][X][O][O][/][/][]
                                    valorO += 4000;
                                if(siHayCasillaDchDch)                  // [][X][O][O][/][O][]
                                    valorO += 8000;
                            }
                            if(noHayCasillaIzq and OpHayCasillaDch){    // [][/][O][O][X][][]
                                valorO += 500;
                                if(noHayCasillaIzqIzq)                  // [/][/][O][O][X][][]
                                    valorO += 4000;
                                if(siHayCasillaIzqIzq)                  // [O][/][O][O][X][][]
                                    valorO += 8000;
                            }
                        }
                    }

                    if(fichasPegadas3){

                    }

                    if(fichasPegadas4){

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



