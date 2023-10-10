#include "Login.h"
#include "ui_Login.h"

#include "AdministradorMenu.h"
#include "EmpleadoMenu.h"
#include "SupervisorMenu.h"
#include "Auditor.h"

constexpr qint16 NOT_ANSWERED = -99;

Login::Login(QWidget *parent, Client& client, QString SI_IP, qint16 SI_Port)
    : QWidget(parent)
    , ui(new Ui::Login)
    , client(client)
    , SI_IP(SI_IP)
    , SI_Port(SI_Port)

{
    ui->setupUi(this);
    connect(ui->checkBoxShowPassword, &QCheckBox::toggled, this, &Login::onShowPasswordToggled);
    ui->password->setEchoMode(QLineEdit::Password); // Ocultar contrasena
    this->start_connection_with_server(SI_IP, SI_Port);
}

Login::~Login()
{
    delete ui;
}

void Login::onShowPasswordToggled(bool checked)
{
    if (checked) {
        ui->password->setEchoMode(QLineEdit::Normal);
    } else {
        ui->password->setEchoMode(QLineEdit::Password);
    }
}

void Login::start_connection_with_server(QString ipAddress, qint16 port) {
    // Conectar socket error a connection_to_server_failed
    connect(&client, &Client::error, this, &Login::connection_to_server_failed);
    connect(&client, &Client::connected, this, &Login::connection_to_server_success);
    connect(&client, &Client::dataReceived, this, &Login::handle_auth_message);
    connect(&client, &Client::disconnected, this, &Login::cleanAfterDisconnect);
    // Establecer la dirección IP y el puerto del servidor
    this->client.connectToServer(ipAddress, port);
}

void Login::handle_auth_message(const Message& message) {
    qint8 accessRights = NOT_ANSWERED;
    if (message.type == Message::Autenticacion) {
        qDebug() << "Received message: Type =" << message.type << ", Content =" << message.data;
        const char* constData = message.data.constData();
        accessRights =  std::stoi(constData);
        qDebug() << "Permisos" << accessRights;
        emit access_rights_read(accessRights);
    }
}

void Login::connection_to_server_failed(QString error) {
    qDebug() << "Failed connecting to server";

    QMessageBox::warning(this, tr("Error de conexión"), "No se pudo conectar al servidor");
        this->close();

    QCoreApplication::quit();
}

void Login::connection_to_server_success() {
    qDebug() << "Server connected successfully";
}


void Login::show_next_screen(int permisos, QString username) //Al llamar a esta funcion se asume que ya el usuario esta autenticado
{
    disconnect(&client, &Client::dataReceived, this, &Login::handle_auth_message);
    if(permisos == USUARIO_EMPLEADO){
        EmpleadoMenu *ventanaEmpleado = new EmpleadoMenu(nullptr, this->client.getInstance(), username);
        ventanaEmpleado->show();
    } else if(permisos == USUARIO_SUPERVISOR){
        SupervisorMenu *ventanaSupervisor = new SupervisorMenu(nullptr, this->client.getInstance(), username);
        ventanaSupervisor->show();
    } else if(permisos == USUARIO_RRHH){
        AdministradorMenu *ventanaRRHH = new AdministradorMenu(nullptr, this->client.getInstance(), username);
        ventanaRRHH->show();
    } else if(permisos == USUARIO_AUDITOR){
        Auditor *ventanaAuditor = new Auditor(nullptr, this->client.getInstance());
        ventanaAuditor->show();
    }
}


void Login::on_loginButton_clicked()
{
    // QString username = ui->username->text();
    // QString password = ui->password->text();

    QString username = ui->username->text();
    QString password = ui->password->text();
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha224);
    qDebug() << "hash " << hashedPassword;
    QEventLoop wait_for_answer;

    qint8 accessRights;
    if (username.isEmpty() || password.isEmpty() || username.length() > 10) {
        QMessageBox::warning(this, tr("Error"), tr("Nombre de usuario o contraseña inválidos"));
    } else {

        QByteArray datagrama(40, '\0');
        datagrama.replace(0, qMin(username.length(), 10), username.toUtf8());
        datagrama.replace(10, qMin(hashedPassword.length(), 30), hashedPassword);
        //Se estableció la conexión, enviar un mensaje al servidor
        Message message;
        message.data = datagrama;
        message.type = Message::Autenticacion;

        qDebug() << "[UI] Sending authentication request to server for user" << username << " , data" << datagrama;
        // Enviar el mensaje al servidor
        client.sendMessage(message);

        connect(this, &Login::access_rights_read, &wait_for_answer, &QEventLoop::exit);
        // Esperamos el recibimiento del mensaje

        accessRights = wait_for_answer.exec();


        if (accessRights == -99) {
            QMessageBox::warning(this, tr("Error"), tr("No se lograron obtener los permisos del servidor"));
            this->close();
        }
        if (accessRights == -1 || accessRights == -3)
        {
            QMessageBox::warning(this, tr("Error"), tr("Nombre de usuario o contraseña incorrectos"));
        }
        else {
            this->close();
            this->show_next_screen(accessRights, username);
        }
    }
}

void Login::cleanAfterDisconnect() {
    this->client.connectToServer(SI_IP, SI_Port);
    connect(&client, &Client::dataReceived, this, &Login::handle_auth_message);
    this->ui->password->clear();
    this->ui->username->clear();
    this->show();
}


