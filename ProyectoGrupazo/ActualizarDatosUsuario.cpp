#include "ActualizarDatosUsuario.h"
#include "ui_ActualizarDatosUsuario.h"
#include "ui_ActualizarUsuarios.h"

ActualizarDatosUsuario::ActualizarDatosUsuario(QWidget *parent, Client& client, QString username) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ActualizarDatosUsuario),
    parentWindow(parent),
    client(client),
    username(username)
{
    ui->setupUi(this);
    this->username = username;
    ui->cedula->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{9}")));
    ui->telefono->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{8}")));
    ui->salarioBruto->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,10}")));
    ui->salarioNeto->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,10}")));
    QObject::connect(&client, &Client::dataReceived, this, &ActualizarDatosUsuario::receiveResponseFromServer);
    this->consultarDatosDeUsuario(this->username);
}

ActualizarDatosUsuario::~ActualizarDatosUsuario()
{
    delete ui;
}

void ActualizarDatosUsuario::on_addUserButton_clicked()
{
    QString username, password, name, lastname, email,
        phone, personalId, job, grossPay, netPay;

    username = ui->username->text();
    name = ui->nombre->text();
    lastname = ui->apellido->text();
    email = ui->correo->text();
    phone = ui->telefono->text();
    personalId = ui->cedula->text();
    job = ui->puesto->text();
    grossPay = ui->salarioBruto->text();
    netPay = ui->salarioNeto->text();

    QByteArray datagrama(10, '\0');
    datagrama.replace(0, qMin(username.length(), 10), username.toUtf8());
    datagrama.replace(10, qMin(name.length(), 15), name.toUtf8());
    datagrama.replace(25, qMin(lastname.length(), 15), lastname.toUtf8());
    datagrama.replace(40, qMin(email.length(), 31), email.toUtf8());
    datagrama.replace(71, qMin(phone.length(), 9), phone.toUtf8());
    datagrama.replace(80, qMin(personalId.length(), 11), personalId.toUtf8());
    datagrama.replace(91, qMin(job.length(), 30), job.toUtf8());
    datagrama.replace(121, qMin(grossPay.length(), 10), grossPay.toUtf8());
    datagrama.replace(131, qMin(netPay.length(), 10), netPay.toUtf8());

    Message message;
    message.data = datagrama;
    message.type = Message::EditarUsuarioDB;
    qDebug() << "[UI] Enviado mensaje de edicion "
                "de usuario a la DB. Username: " << username;
    client.sendMessage(message);

    QEventLoop dbWait;
    connect(this, &ActualizarDatosUsuario::read_information, &dbWait, &QEventLoop::exit);
    dbWait.exec();
    int success = std::atoi(this->informacion);

    if(success == EXIT_SUCCESS) {
        qDebug() << "[UI] Se edito el usuario correctamente en BDD";
        QString mensaje = "El usuario " + ui->username->text() + " se ha editado con éxito en BDD!";
        QMessageBox::information(this,"Éxito!", mensaje);
    } else {
        qDebug() << "[UI] Recibido error al editar usuario en BDD";
        QString mensaje = "Hubo un error al editar el usuario " + ui->username->text() + " en BDD!";
        QMessageBox::information(this,"Error!", mensaje);
    }
}


void ActualizarDatosUsuario::receiveResponseFromServer(const Message &message)
{
    qDebug() << "Received message: Type =" << message.type << ", Content =" << message.data;
    Message::MessageType type = message.type;
    switch (type)
    {
    case Message::ConsultarDatosDeUsuario:
        this->informacion = message.data;
        emit read_information(0);
        break;
    case Message::EditarUsuarioDB:
        this->informacion = message.data;
        emit read_information(0);
        break;
    default:
        break;
    }
}

void ActualizarDatosUsuario::fillInputInfo() {
    qDebug() << this->informacion;
    QString username = QString::fromUtf8(this->informacion.left(10).constData());
    QString name = QString::fromUtf8(this->informacion.mid(10, 15).constData());
    QString lastname = QString::fromUtf8(this->informacion.mid(25, 15).constData());
    QString email = QString::fromUtf8(this->informacion.mid(40, 31).constData());
    QString phone = QString::fromUtf8(this->informacion.mid(71, 9).constData());
    QString personalId = QString::fromUtf8(this->informacion.mid(80, 11).constData());
    QString job = QString::fromUtf8(this->informacion.mid(91, 30).constData());
    QString grossPay = QString::fromUtf8(this->informacion.mid(121, 13).constData());
    QString netPay = QString::fromUtf8(this->informacion.mid(134, 13).constData());

    grossPay = grossPay.trimmed();
    netPay = netPay.trimmed();

    grossPay = grossPay.remove((grossPay.size()-3), (grossPay.size()-1));
    netPay = netPay.remove((netPay.size()-3), (netPay.size()-1));

    ui->username->setText(username);
    ui->nombre->setText(name);
    ui->apellido->setText(lastname);
    ui->correo->setText(email);
    ui->telefono->setText(phone);
    ui->cedula->setText(personalId);
    ui->puesto->setText(job);
    ui->salarioBruto->setText(grossPay);
    ui->salarioNeto->setText(netPay);
}

void ActualizarDatosUsuario::consultarDatosDeUsuario(QString username) {
    QByteArray datagrama(10, '\0');
    datagrama.replace(0, qMin(username.length(), 10), username.toUtf8());

    Message message;
    message.data = datagrama;
    message.type = Message::ConsultarDatosDeUsuario;
    qDebug() << "[UI] Enviado mensaje de consulta de datos "
                "de usuario a la DB. Username: " << username;

    // Enviar el mensaje al servidor
    client.sendMessage(message);

    QEventLoop wait_for_answer;
    connect(this, &ActualizarDatosUsuario::read_information, &wait_for_answer, &QEventLoop::quit);
    wait_for_answer.exec();

    this->fillInputInfo();
}

void ActualizarDatosUsuario::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void ActualizarDatosUsuario::on_logout_clicked()
{
    QMessageBox::StandardButton confirmarLogout;
    confirmarLogout = QMessageBox::question(this, "Cerrar sesión ", "¿Esta seguro que desea cerrar sesión?", QMessageBox::Yes | QMessageBox::No);
    if(confirmarLogout == QMessageBox::Yes){
        Client& client = Client::getInstance();  // Obtener la instancia del singleton del cliente
        close();
        client.disconnectFromServer();
        this->deleteLater();
    }
}



