#ifndef TABLEROWIDGET_H
#define TABLEROWIDGET_H


#include <QWidget>
#include "juego.h"

class Tablero;

/**
 * @brief The TableroWidget class
 * Esta clase tiene dos funciones:
 *  - Mostrar el estado del tablero de juego
 *  - Obtener la entrada del usuario en caso de escoger la modalidad humano vs IA
 */
class TableroWidget : public QWidget
{
    Q_OBJECT
public:
    TableroWidget(QWidget *parent = 0);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;


    void setJuego(Juego *value);

signals:
    void jugadaHumano(const int, const int);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:

    int actualColumn;
    Juego *juego;

};

#endif // TABLEROWIDGET_H
