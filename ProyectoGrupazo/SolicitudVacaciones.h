#ifndef SOLICITUDVACACIONES_H
#define SOLICITUDVACACIONES_H

#include <QWidget>
#include <QDate>
#include <QMessageBox>

#include "UsuarioSolicitudes.h"
#include "Client.h"

namespace Ui {
class SolicitudVacaciones;
}

class SolicitudVacaciones : public QWidget
{
    Q_OBJECT

public:
    explicit SolicitudVacaciones(QWidget *parent = nullptr, UsuarioSolicitudes *UsuarioSolicitudesParentWindow= nullptr, Client& client = Client::getInstance(), QString username = "");
    ~SolicitudVacaciones();

signals:
    void fechasSeleccionadasActualizadas(QDate fechaInicio, QDate fechaFin);
    void response_read(qint8 response);

private slots:
    void on_calendar1_userDateChanged(const QDate& date);
    void on_calendar2_userDateChanged(const QDate& date);
    void actualizarCantidadDias(QDate fechaInicio, QDate fechaFin);

    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();
    void receiveResponseFromServer(const Message& message);

private:
    Ui::SolicitudVacaciones *ui;
    QWidget *parentWindow;
    UsuarioSolicitudes *UsuarioSolicitudesParentWindow;
    Client& client; // Agrega una referencia a Client
    QString username;
};

#endif // SOLICITUDVACACIONES_H

