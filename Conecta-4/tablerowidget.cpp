#include "tablerowidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <stdlib.h>
#include <iostream>
#include "tablero.h"

TableroWidget::TableroWidget(QWidget *parent) : QWidget(parent)
{
    actualColumn = -1;
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

QSize TableroWidget::minimumSizeHint() const
{
    return QSize(140, 140);
}

QSize TableroWidget::sizeHint() const
{
    return QSize(350, 350);
}

void TableroWidget::mouseMoveEvent(QMouseEvent *event)
{
    int tam = width()<height()?width():height();
    int col = ((float)event->x() / tam)*7;
    if(col>6) col=6;

    if(col!=actualColumn)
    {
        actualColumn = col;
        this->update();
    }
}

/**
 * @brief TableroWidget::mouseReleaseEvent Inserta una ficha en el tablero cuando el jugador humano hace click
 * @param event Responsable de la llamada al procedimiento
 */
void TableroWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(juego->getJugando() && juego->getEsJugadorHumano())
        emit jugadaHumano(actualColumn, juego->getTurno());
}

void TableroWidget::setJuego(Juego *value)
{
    juego = value;
}


void TableroWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    Tablero *tablero = juego->getTablero();


    int tam = width()<height()?width():height();
    int cellSize = tam/7;
    int diameter = cellSize-6;

    painter.setPen(QPen(QColor(50, 50, 50, 255),2));
    QLinearGradient linearGradient(0, 0, cellSize*COLS_TABLERO, cellSize*FILAS_TABLERO);
            linearGradient.setColorAt(0.0, Qt::white);
            linearGradient.setColorAt(0.2, Qt::green);
            linearGradient.setColorAt(1.0, Qt::black);
    painter.setBrush(linearGradient);
    painter.drawRect(0, cellSize, cellSize*COLS_TABLERO, cellSize*FILAS_TABLERO);



    for(int fila=0;fila<FILAS_TABLERO;fila++)
        for(int col=0;col<COLS_TABLERO;col++)
        {
            if(tablero)
            {
                if(tablero->obtenerCasilla(fila, col)==1)
                    painter.setBrush(QBrush(QColor(255, 0, 0)));
                else if(tablero->obtenerCasilla(fila, col)==2)
                    painter.setBrush(QBrush(QColor(225, 225, 0)));
                else {
                    painter.setBrush(QBrush(QColor(190, 190, 200)));
                }
            }
            else {
                painter.setBrush(QBrush(QColor(190, 190, 200)));
            }

            painter.drawEllipse(col*cellSize+3, cellSize*(FILAS_TABLERO+1)-cellSize*(fila+1)+3, diameter, diameter);
        }

    //muestra ficha a insertar
    if(juego->getJugando() && juego->getEsJugadorHumano())
    {
        if(juego->getTurno()==1)
            painter.setBrush(QBrush(QColor(255, 0, 0)));
        else
            painter.setBrush(QBrush(QColor(225, 225, 0)));
        painter.drawEllipse(actualColumn*cellSize+3, 3, diameter, diameter);
    }

}
