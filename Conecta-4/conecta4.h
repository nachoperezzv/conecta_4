#ifndef CONECTA4_H
#define CONECTA4_H

#include <QWidget>

#include "tablero.h"
#include "tablerowidget.h"
#include "juego.h"

//QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QComboBox;
//QT_END_NAMESPACE
class TableroWidget;
class Tablero;
class Juego;


class Conecta4 : public QWidget
{
    Q_OBJECT

public:
    Conecta4(QWidget *parent = nullptr);
    ~Conecta4();

public slots:
    void startBTClicked();
    void updateMsg(Juego::MensajeJuego msg);

private:
    QLabel *createLabel(const QString &text);
    QPushButton *createButton(const QString &text, const char *member);

//    Tablero *C4Tablero;
    Juego *nuevoJuego;
    TableroWidget *tablerowidget;
    QLabel *statusLabel;
    QComboBox *gameTypeComboBox;
};
#endif // CONECTA4_H
