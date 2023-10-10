#ifndef ESTADOSOLICITUDESHECHAS_H
#define ESTADOSOLICITUDESHECHAS_H

#include <QWidget>
#include <QListWidget>

#include "UsuarioConsultas.h"
#include "Client.h"


struct Usuario {
    QString nombre;
    QString fecha;
};

struct Solicitud {
    QString nombre;
    int id;
    QString estado;
    QString color;
};

struct DatosConstancia {
    QString Nombre;
    QString Apellido;
    QString Cedula;
    QString Puesto;
    QString SalarioBruto;
    QString SalarioNeto;
    QString FechaEnvio;
    QString FechaAprobacion;
    QString FechaInicioLabores;
    QString FechaInicioVacaciones;
    QString FechaFinalVacaciones;
};

namespace Ui {
class EstadoSolicitudesHechas;
}

class EstadoSolicitudesHechas : public QWidget
{
    Q_OBJECT

public:
    explicit EstadoSolicitudesHechas(QWidget *parent = nullptr, UsuarioConsultas *UsuarioConsultasParentWindow = nullptr,
     Client& client = Client::getInstance(), QString username = "");
    ~EstadoSolicitudesHechas();

signals:
    void leidas_solicitudes(qint16 error);

private slots:
    void verDetallesUsuario();
    void agregarUsuariosDePrueba();
    QList<Usuario> obtenerUsuariosDePrueba();
    void generarWidgetsSolicitudes(const Usuario& usuario);
    QList<Solicitud> obtenerSolicitudesDeUsuario(const Usuario& usuario);

    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();

    void solicitudes_recibidas(const Message& message);

private:
    Ui::EstadoSolicitudesHechas *ui;
    QListWidget* listaWidget;
    QWidget* parentWindow;
    UsuarioConsultas* UsuarioConsultasParentWindow;
    Client& client; // Agregar una referencia a Client
    QString username;
    QList<Solicitud> solicitudes;
    DatosConstancia datosConstancia;

    void conectarBotonesDetalles();

    QString obtenerColorEstado(const QString& estado);
};

#endif // ESTADOSOLICITUDESHECHAS_H

