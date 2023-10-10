#include "SolicitudVacaciones.h"
#include "ui_SolicitudVacaciones.h"
#include "Login.h"

SolicitudVacaciones::SolicitudVacaciones(QWidget *parent,  UsuarioSolicitudes *UsuarioSolicitudesParent,  Client& client,  QString username)
    : QWidget(parent, Qt::Window),
      ui(new Ui::SolicitudVacaciones),
      parentWindow(parent),
      UsuarioSolicitudesParentWindow(UsuarioSolicitudesParent),
      client(client),
      username(username)
{
    ui->setupUi(this);
    connect(this, &SolicitudVacaciones::fechasSeleccionadasActualizadas, this, &SolicitudVacaciones::actualizarCantidadDias);
    connect(&client, &Client::dataReceived, this, &SolicitudVacaciones::receiveResponseFromServer);
}

SolicitudVacaciones::~SolicitudVacaciones()
{
    delete ui;
}

void SolicitudVacaciones::on_calendar1_userDateChanged(const QDate& date)
{
    QDate fechaInicio = ui->calendar1->date();
    QDate fechaFin = ui->calendar2->date();

    emit fechasSeleccionadasActualizadas(fechaInicio, fechaFin);
}

void SolicitudVacaciones::on_calendar2_userDateChanged(const QDate& date)
{
    QDate fechaInicio = ui->calendar1->date();
    QDate fechaFin = ui->calendar2->date();

    emit fechasSeleccionadasActualizadas(fechaInicio, fechaFin);
}

void SolicitudVacaciones::actualizarCantidadDias(QDate fechaInicio, QDate fechaFin)
{
    int totalDias = 0;

    if (fechaInicio.isValid() && fechaFin.isValid())
    {
        if (fechaInicio <= fechaFin)
            totalDias = fechaInicio.daysTo(fechaFin);
    }

    ui->dias->setText(QString::number(totalDias));
}

void SolicitudVacaciones::receiveResponseFromServer(const Message &message)
{
    const char* constData = message.data.constData();
    int solicitudAgregada =  std::stoi(constData);
    qDebug() << "[UI] Recibido mensaje de respuesta de solicitud de vacaciones";
    emit response_read(solicitudAgregada);
}

void SolicitudVacaciones::on_acceptButton_clicked()
{
    QEventLoop loop;
    int solicitudAgregada = EXIT_SUCCESS;
    QDate fechaActual = QDate::currentDate();
    QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
    QByteArray datagrama(40, '\0');
    datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
    datagrama.replace(10, 10, formattedActualDate.toUtf8());//Fecha actual
    datagrama.replace(20, 10, ui->calendar1->date().toString("dd/MM/yyyy").toUtf8());//Fecha inicio vacaciones
    datagrama.replace(30, 10, ui->calendar2->date().toString("dd/MM/yyyy").toUtf8());//Fecha final vacaciones
    Message message;
    message.data = datagrama;
    message.type = Message::SolicitudVacaciones;
    client.sendMessage(message);
    connect(this, &SolicitudVacaciones::response_read, &loop, &QEventLoop::quit);
    solicitudAgregada = loop.exec();
    if(solicitudAgregada == EXIT_SUCCESS){
      QMessageBox::information(this, "Solicitud a revisión", "La solicitud ha sido enviada para su revisión.");
    }else {
      QMessageBox::warning(this, tr("Error"), tr("La solicitud no fue enviada.Por favor intente de nuevo"));
    }
}


void SolicitudVacaciones::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void SolicitudVacaciones::on_logout_clicked()
{
    QMessageBox::StandardButton confirmarLogout;
    confirmarLogout = QMessageBox::question(this, "Cerrar sesión ", "¿Esta seguro que desea cerrar sesión?", QMessageBox::Yes | QMessageBox::No);
    if(confirmarLogout == QMessageBox::Yes){
        Client& client = Client::getInstance();  // Obtener la instancia del singleton del cliente
        close();
        client.disconnectFromServer();
        QEventLoop closed;
        connect(&client, &Client::disconnected, &closed, &QEventLoop::quit);
        closed.exec();
        this->deleteLater();
    }
}

