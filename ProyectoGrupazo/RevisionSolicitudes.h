#ifndef REVISIONSOLICITUDES_H
#define REVISIONSOLICITUDES_H

#include <QWidget>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>

#include "Client.h"

struct Usuario {
    QString nombre;
    QString fecha;
};

namespace Ui {
class RevisionSolicitudes;
}

class RevisionSolicitudes : public QWidget
{
    Q_OBJECT

public:
    explicit RevisionSolicitudes(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "", quint8 estadoSolicitud = 0);
    ~RevisionSolicitudes();

private slots:
    void verDetallesUsuario(int);
    void aceptarUsuario(int);
    void rechazarUsuario(int);
    void on_acceptButton_clicked();
    void on_cancelButton_clicked();
    void on_logout_clicked();

    void procesar_solicitudes(const Message& message);

signals: 

    void solicitudesProcesadas(qint16 error);

private:
    Ui::RevisionSolicitudes *ui;
    QWidget *parentWindow;
    Client& client; // Se agrega una referencia Client
    QString username;
    QListWidget* listaWidget;
    QList<QString> solicitudes;
    quint8 estadoSolicitud;

    void mostrarWidgets(quint8 estadoSolicitud);
    const Usuario& obtenerConstancia(int index) const;
    void enviarModificarEstadoSolicitud(const QString& idSolicitud, quint8 estadoSolicitud, quint8 tipoRegistro);
};

#endif // REVISIONSOLICITUDES_H
