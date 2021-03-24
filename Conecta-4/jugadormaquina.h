#ifndef JUGADORMAQUINA_H
#define JUGADORMAQUINA_H

#include "jugador.h"

struct coord{
   int x;
   int y;
};

struct valor_col{
   int col;
   int val;
};

class JugadorMaquina: public Jugador
{
public:
    JugadorMaquina(Jugador *parent);
    void setTiempoExcedido(bool excedido);

    const int NIVEL_DEFECTO = 8;


public slots:
    void comenzarBusqueda(const int jug) override; //NO MODIFICAR ESTRUCTURA DE LA FUNCIÓN


private:
    int         minimax(int, int, int, int);
    //int         minimax(int,int);

    int         heuristica();
    int         giveOponent();
    bool        isFeasible(coord);
    valor_col   MAX(int, valor_col, int);
    valor_col   MIN(int, valor_col, int);

    //int         alfa = INT_MIN;
    //int         beta = INT_MAX;
    int         aux;


    bool        tiempoExcedido; // Úsalo para detener el algoritmo minimax cuando sea true
    valor_col   mejorJug;
    valor_col   mejorOpo;
    std::vector<coord> pos = {{-1,1,}, {0,1},{1,1},{1,0}};
};

#endif // JUGADORMAQUINA_H
