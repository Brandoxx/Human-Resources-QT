#include "UsuarioConsultas.h"
#include "ui_UsuarioConsultas.h"
#include "Login.h"
#include "EstadoSolicitudesHechas.h"
#include "ConstanciaUsuarios.h"
#include "GeneradorConstancias.h"

UsuarioConsultas::UsuarioConsultas(QWidget *parent, Client& client, QString username) :
    QWidget(parent, Qt::Window),
    ui(new Ui::UsuarioConsultas),
    parentWindow(parent),
    client(client),
    username(username)
{
    ui->setupUi(this);
    connect(&client, &Client::dataReceived, this, &UsuarioConsultas::handle_message);
}

UsuarioConsultas::~UsuarioConsultas()
{
    delete ui;
}

void UsuarioConsultas::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}

void UsuarioConsultas::handle_message(const Message& message){
    qDebug() << "Received message: Type =" << message.type << ", Content =" << message.data;
    Message::MessageType type = message.type;
    switch (type)
    {
    case Message::ConsultarSalarioBase:
    case Message::ConsultarSaldoVacaciones:
    case Message::SolicitarAnotacionesAlExpediente:
        this->informacion = message.data;
        emit read_information(2);
    default:
        break;
    }
}

void UsuarioConsultas::on_logout_clicked()
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

void UsuarioConsultas::on_acceptButton_clicked()
{
    QEventLoop wait_for_answer;
    if (ui->radioButton_1->isChecked())
    {
        // QMessageBox::information(this, "Consulta de Saldo de Vacaciones", informeSaldoVacaciones());
        // Solamente es una prueba para verificar si esta funcionando
//        GeneradorConstancias informe;
//        this->hide();
//        ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.SolicitudVacacionesAprobada());
//        constancia->show();
        QByteArray datagrama(10, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        Message message;
        message.data = datagrama;
        message.type = Message::ConsultarSaldoVacaciones;
        client.sendMessage(message);

        connect(this, &UsuarioConsultas::read_information, &wait_for_answer, &QEventLoop::quit);
        wait_for_answer.exec();
        //qDebug() << "Informacion: " << this->informacion;
        QString nombre = QString::fromUtf8(this->informacion.left(15));
        QString apellido = QString::fromUtf8(this->informacion.mid(15, 15).constData());
        QString vacaciones = QString::fromUtf8(this->informacion.right(3).constData());
        QMessageBox::information(this, "Consulta de Saldo de Vacaciones", informeSaldoVacaciones(nombre,apellido,vacaciones));

    }
    else if (ui->radioButton_2->isChecked())
    {
        // QMessageBox::information(this, "Consulta de Salario Base", informeSalarioBase());
        QByteArray datagrama(10, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        Message message;
        message.data = datagrama;
        message.type = Message::ConsultarSalarioBase;
        client.sendMessage(message);

        connect(this, &UsuarioConsultas::read_information, &wait_for_answer, &QEventLoop::quit);
        wait_for_answer.exec();
        //qDebug() << "Informacion: " << this->informacion;
        QString nombre = QString::fromUtf8(this->informacion.mid(10, 15).constData());
        QString apellido = QString::fromUtf8(this->informacion.mid(25, 15).constData());
        QString salario = QString::fromUtf8(this->informacion.right(10).constData());
        QMessageBox::information(this, "Consulta de Salario Base", informeSalarioBase(nombre,apellido,salario));
    }
    else if (ui->radioButton_3->isChecked())
    {
        // TODO
	this->hide();
        EstadoSolicitudesHechas* ventanaEstadoSolicitudesHechas = new EstadoSolicitudesHechas(this, this, client.getInstance(), this->username);
        ventanaEstadoSolicitudesHechas->show();
    } else if (ui->radioButton_4->isChecked())
    {
        int solicitudAgregada = EXIT_SUCCESS;
        // Consulta de anotaciones al expediente
        QDate fechaActual = QDate::currentDate();
        QString formattedActualDate = fechaActual.toString("yyyy-MM-dd");
        QByteArray datagrama(20, '\0');
        datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
        datagrama.replace(10, 10, formattedActualDate.toUtf8());//Fecha actual
        Message message;
        message.data = datagrama;
        message.type = Message::SolicitarAnotacionesAlExpediente;
        client.sendMessage(message);

        connect(this, &UsuarioConsultas::read_information, &wait_for_answer, &QEventLoop::quit);
        wait_for_answer.exec();
        //qDebug() << "Informacion: " << this->informacion;
        const char* constData = this->informacion.constData();
        solicitudAgregada =  std::stoi(constData);
        if(solicitudAgregada == EXIT_SUCCESS){
            QMessageBox::information(this, "Solicitud de Anotaciones al expediente ", "La solicitud ha sido enviada correctamente para su revisión.");
        }else {
            QMessageBox::warning(this, tr("Error"), tr("La solicitud no fue enviada.Por favor intente de nuevo."));
        }
    }
}

QString UsuarioConsultas::informeSalarioBase(const QString& nombre, const QString& apellido, const QString& salarioBase)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h1 style='text-align: center;'>Salario Base</h1>";
    informacion += "<hr>";
    informacion += "<p><strong>Nombre:</strong> %1</p>";
    informacion += "<p><strong>Apellido:</strong> %2</p>";
    informacion += "<p><strong>Salario Base:</strong> ₡%3</p>";
    informacion += "<hr>";
    informacion += "</body></html>";

    informacion = informacion.arg(nombre, apellido, salarioBase);
    return informacion;
}


QString UsuarioConsultas::informeSaldoVacaciones(const QString& nombre, const QString& apellido, const QString& saldoVacaciones)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h1 style='text-align: center;'>Saldo de Vacaciones</h1>";
    informacion += "<hr>";
    informacion += "<p><strong>Nombre:</strong> %1</p>";
    informacion += "<p><strong>Apellido:</strong> %2</p>";
    informacion += "<p><strong>Saldo de vacaciones:</strong> %3</p>";
    informacion += "<hr>";
    informacion += "</body></html>";

    informacion = informacion.arg(nombre, apellido, saldoVacaciones);
    return informacion;
}

