#include "UsuarioSolicitudes.h"
#include "ui_UsuarioSolicitudes.h"
#include "Login.h"
#include "SolicitudVacaciones.h"
#include "ConstanciaUsuarios.h"
#include "GeneradorConstancias.h"

#include <QScrollArea>


UsuarioSolicitudes::UsuarioSolicitudes(QWidget *parent, Client& client, QString username) :
    QWidget(parent, Qt::Window),
    ui(new Ui::UsuarioSolicitudes),
    parentWindow(parent),
    client(client),
    username(username)
{
    ui->setupUi(this);
    connect(&client, &Client::dataReceived, this, &UsuarioSolicitudes::receiveResponseFromServer);
}

UsuarioSolicitudes::~UsuarioSolicitudes()
{
    delete ui;
}

void UsuarioSolicitudes::receiveResponseFromServer(const Message &message)
{
    this->informacion = message.data;
    qDebug() << "[UI] Recibido mensaje de respuesta de solicitud";
    emit response_read(0);
}

void UsuarioSolicitudes::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}

void UsuarioSolicitudes::on_logout_clicked()
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


void UsuarioSolicitudes::on_acceptButton_clicked()
{
    QEventLoop loop;
    if (ui->radioButton_1->isChecked())
    {
        int solicitudAgregada = EXIT_SUCCESS;
        QDate fechaActual = QDate::currentDate();
        QString formattedActualDate = fechaActual.toString("yyyy-MM-dd");
        QByteArray datagrama(21, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        datagrama.replace(10, 10, formattedActualDate.toUtf8());//Fecha actual
        datagrama.replace(20,1,"2");
        Message message;
        message.data = datagrama;
        message.type = Message::SolicitudConstanciaSalarial;
        client.sendMessage(message);
        connect(this, &UsuarioSolicitudes::response_read, &loop, &QEventLoop::exit);
        loop.exec();
        const char* constData = this->informacion.constData();
        solicitudAgregada = std::stoi(constData);
        if(solicitudAgregada == EXIT_SUCCESS){
            QMessageBox::information(this, "Solicitud de Constancia Salarial", "La solicitud ha sido enviada correctamente para su revisión.");
        }else {
            QMessageBox::warning(this, tr("Error"), tr("La solicitud no fue enviada.Por favor intente de nuevo. Recuerda que solo puedes"
                                                       " solicitar una constancia laboral por semana"));
        }
    }
    else if (ui->radioButton_2->isChecked())
    {
        int solicitudAgregada = EXIT_SUCCESS;
        QDate fechaActual = QDate::currentDate();
        QString formattedActualDate = fechaActual.toString("yyyy-MM-dd");
        QByteArray datagrama(21, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        datagrama.replace(10, 10, formattedActualDate.toUtf8());//Fecha actual
        datagrama.replace(20,1,"1");
        Message message;
        message.data = datagrama;
        message.type = Message::SolicitudConstanciaLaboral;
        client.sendMessage(message);
        connect(this, &UsuarioSolicitudes::response_read, &loop, &QEventLoop::exit);
        loop.exec();
        const char* constData = this->informacion.constData();
        solicitudAgregada = std::stoi(constData);
        if(solicitudAgregada == EXIT_SUCCESS){
            QMessageBox::information(this, "Solicitud de Constancia Laboral", "La solicitud ha sido enviada correctamente para su revisión.");
        }else {
            QMessageBox::warning(this, tr("Error"), tr("La solicitud no fue enviada.Por favor intente de nuevo. Recuerda que solo puedes "
                                                    "solicitar una constancia laboral por semana"));
        }
    }
    else if (ui->radioButton_3->isChecked())
    {
        this->hide();
        SolicitudVacaciones *ventanaSolicitudVacaciones = new SolicitudVacaciones(this, this, client.getInstance(), this->username);
        ventanaSolicitudVacaciones->show();
    } else if (ui->radioButton_4->isChecked())
    {
        QByteArray datagrama(10, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        Message message;
        message.data = datagrama;
        message.type = Message::ConsultarComprobantesDePago;
        client.sendMessage(message);
        connect(this, &UsuarioSolicitudes::response_read, &loop, &QEventLoop::exit);
        loop.exec();
        const char* constData = this->informacion.constData();// comprobantes de pago
       // QMessageBox::information(this, "Solicitud de Comprobantes de Pago", constData);
        QDialog dialog(this);
        dialog.setWindowTitle("Solicitud de Comprobantes de Pago");
        dialog.setMinimumWidth(300); // Establecer el ancho mínimo del diálogo

        QLabel titleLabel("Historial de Pagos:");
        QFont titleFont;
        titleFont.setBold(true);
        titleFont.setPointSize(12);
        titleLabel.setFont(titleFont);

        QVBoxLayout mainLayout(&dialog); // Layout principal
        mainLayout.addWidget(&titleLabel);

        QScrollArea scrollArea;
        QLabel label(constData);
        label.setWordWrap(true); // Asegura que el texto se ajuste automáticamente al ancho del QLabel

        scrollArea.setWidget(&label);
        scrollArea.setWidgetResizable(true);
        scrollArea.setMinimumWidth(300); // Establecer el ancho mínimo del área de desplazamiento

        mainLayout.addWidget(&scrollArea);

        dialog.exec();
    }
}

