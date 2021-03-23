#include "conecta4.h"
#include "ui_conecta4.h"
#include "juego.h"

#include <QtWidgets>
#include <iostream>

Conecta4::Conecta4(QWidget *parent)
    : QWidget(parent)
{

    nuevoJuego = new Juego();

    tablerowidget = new TableroWidget;
    tablerowidget->setJuego(nuevoJuego);
    tablerowidget->setMouseTracking(true);

    tablerowidget->update();

    statusLabel = createLabel(tr("Juego no iniciado"));

    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    QPushButton *startBT = createButton(tr("Iniciar"), SLOT(startBTClicked()));
    hLayout->addWidget(startBT);

    gameTypeComboBox = new QComboBox;
    gameTypeComboBox->addItem(tr("Humano vs humano"), Juego::HUMANOvsHUMANO);
    gameTypeComboBox->addItem(tr("Humano vs IA"), Juego::HUMANOvsIA);
    gameTypeComboBox->addItem(tr("IA vs Humano"), Juego::IAvsHUMANO);
    gameTypeComboBox->addItem(tr("IA vs IA"), Juego::IAvsIA);
    connect(gameTypeComboBox, QOverload<int>::of(&QComboBox::activated), nuevoJuego, &Juego::setModoJuego);
    hLayout->addWidget(gameTypeComboBox);

    vLayout->insertLayout(-1,hLayout, 1);

    vLayout->addWidget(tablerowidget);

    vLayout->addWidget(statusLabel);

    setLayout(vLayout);

    QObject::connect(nuevoJuego, &Juego::statusChanged, this, &Conecta4::updateMsg);
    QObject::connect(tablerowidget, &TableroWidget::jugadaHumano, nuevoJuego, &Juego::manejarResultadoJugador);
}

Conecta4::~Conecta4()
{
    //    delete ui;
}

void Conecta4::startBTClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (!clickedButton)
      return;

    nuevoJuego->comenzarJuego();
}


void Conecta4::updateMsg(Juego::MensajeJuego msg)
{
    switch (msg) {
    case Juego::TURNO1:
        statusLabel->setText(tr("Jugando... Turno del jugador 1"));
        break;
    case Juego::TURNO2:
        statusLabel->setText(tr("Jugando... Turno del jugador 2"));
        break;
    case Juego::GANA1:
        statusLabel->setText(tr("Juego acabado. Gana el jugador 1"));
        break;
    case Juego::GANA2:
        statusLabel->setText(tr("Juego acabado. Gana el jugador 2"));
        break;
    case Juego::EMPATE:
        statusLabel->setText(tr("Juego acabado. Empate"));
        break;
    case Juego::ERRORJUEGO:
        statusLabel->setText(tr("Juego acabado. ERROR"));
        break;
    default:
        break;
    }
}

QLabel *Conecta4::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setMargin(2);
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
    return label;
}

QPushButton *Conecta4::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

