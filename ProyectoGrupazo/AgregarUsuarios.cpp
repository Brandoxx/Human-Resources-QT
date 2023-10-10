#include "AgregarUsuarios.h"
#include "ui_AgregarUsuarios.h"
#include "Login.h"
#include <QRegularExpressionValidator>
#include <QDate>


AgregarUsuarios::AgregarUsuarios(QWidget *parent, AdministradorMenu *AdministradorMenuParent, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::AgregarUsuarios),
    parentWindow(parent),
    AdministradorMenuParentWindow(AdministradorMenuParent),
    client(client)
{
    ui->setupUi(this);
    ui->cedula->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{9}")));
    ui->telefono->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{8}")));
    ui->salarioBruto->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,10}")));
    ui->salarioNeto->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,10}")));
    this->setTabOrder();
    QObject::connect(&client, &Client::dataReceived, this, &AgregarUsuarios::receiveResponseFromServer);
}

AgregarUsuarios::~AgregarUsuarios()
{
    delete ui;
}

void AgregarUsuarios::on_addUserButton_clicked()
{
    QString username, password, name, lastname, email,
            phone, personalId, job, grossSalary, netSalary, area;
    username = ui->username->text();
    password = ui->password->text();
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha224);

    int rol = ui->rol->currentIndex() + 1;
    name = ui->nombre->text();
    lastname = ui->apellido->text();
    email = ui->correo->text();
    phone = ui->telefono->text();
    personalId = ui->cedula->text();
    job = ui->puesto->text();
    grossSalary = ui->salarioBruto->text();
    netSalary = ui->salarioNeto->text();
    area = ui->area->text();
    int usuarioCreadoFS = 0;
    int usuarioCreadoDB = 0;

    QDate currentDate = QDate::currentDate();
    QString stringDate = currentDate.toString("dd-MM-yyyy");

    this->sendUserToFSServer(username, hashedPassword, rol);
    QEventLoop loop;
    connect(this, &AgregarUsuarios::response_readFS, &loop, &QEventLoop::exit);
    usuarioCreadoFS = loop.exec();


    this->sendUserToDBServer(username, name, lastname, email,
                             phone, personalId, job, stringDate,
                             grossSalary, netSalary, area);
    QEventLoop dbWait;
    connect(this, &AgregarUsuarios::response_readDB, &dbWait, &QEventLoop::exit);
    usuarioCreadoDB = dbWait.exec();

    // Crea el QEventLoop para esperar la respuesta del servidor
    // Enviar datos extra al db server

    if(usuarioCreadoFS == EXIT_SUCCESS) {
        qDebug() << "[UI] Recibido exito al crear usuario en FS";
        QString mensaje = "El usuario " + ui->username->text() + " se ha creado con éxito en FS!";
        QMessageBox::information(this,"Éxito!", mensaje);
    } else {
        qDebug() << "[UI] Recibido error al crear usuario en FS";
        QString mensaje = "Hubo un error al crear el usuario " + ui->username->text() + " en FS!";
        QMessageBox::information(this,"Error!", mensaje);
    }

    if(usuarioCreadoDB == EXIT_SUCCESS) {
        qDebug() << "[UI] Recibido exito al crear usuario en BDD";
        QString mensaje = "El usuario " + ui->username->text() + " se ha creado con éxito en BDD!";
        QMessageBox::information(this,"Éxito!", mensaje);
    } else {
        qDebug() << "[UI] Recibido error al crear usuario en BDD";
        QString mensaje = "Hubo un error al crear el usuario " + ui->username->text() + " en BDD!";
        QMessageBox::information(this,"Error!", mensaje);
    }
    this->clearInputFields();
}

void AgregarUsuarios::receiveResponseFromServer(const Message &message)
{
    if (message.type == Message::CrearUsuarioFS){
        // Interpreta el booleano enviado por el server
        const char* fsCreated = message.data.constData();
        int userCreatedFS =  std::stoi(fsCreated);
        qDebug() << "[UI] Recibido mensaje de respuesta del FS";

        emit response_readFS(userCreatedFS);
    } else {
        const char* dbCreated = message.data.constData();
        int userCreatedDB =  std::atoi(dbCreated);
        qDebug() << "[UI] Recibido mensaje de respuesta de la BDD";
        emit response_readDB(userCreatedDB);
    }
}

void AgregarUsuarios::sendUserToFSServer(QString username, QByteArray password, qint16 role)
{
    QByteArray datagrama(41, '\0');
    datagrama.replace(0, qMin(username.length(), 10), username.toUtf8());
    datagrama.replace(10, qMin(password.length(), 30), password);
    datagrama.replace(41, 1, QString::number(role).toUtf8());
    //Se estableció la conexión, enviar un mensaje al servidor
    Message message;
    message.data = datagrama;
    message.type = Message::CrearUsuarioFS;

    qDebug() << "[UI] Enviado mensaje de creacion de usuario al FS. Username: " << username;
    // Enviar el mensaje al servidor
    client.sendMessage(message);
}

void AgregarUsuarios::sendUserToDBServer(QString username, QString name, QString lastname,
                        QString email, QString phone, QString personalId,
                        QString job, QString initDate, QString grossPay,
                        QString netPay, QString area) {
    // username 10, name 15, lastname 15, email 31, phone 8, personalId 9,
    // job 15, initDate 10, grossPay 10, netPay 10, area 15
    QByteArray datagrama(148, '\0');
    datagrama.replace(0, qMin(username.length(), 10), username.toUtf8());
    datagrama.replace(10, qMin(name.length(), 15), name.toUtf8());
    datagrama.replace(25, qMin(lastname.length(), 15), lastname.toUtf8());
    datagrama.replace(40, qMin(email.length(), 31), email.toUtf8());
    datagrama.replace(71, qMin(phone.length(), 8), phone.toUtf8());
    datagrama.replace(79, qMin(personalId.length(), 9), personalId.toUtf8());
    datagrama.replace(88, qMin(job.length(), 15), job.toUtf8());
    datagrama.replace(103, qMin(initDate.length(), 10), initDate.toUtf8());
    datagrama.replace(113, qMin(grossPay.length(), 10), grossPay.toUtf8());
    datagrama.replace(123, qMin(netPay.length(), 10), netPay.toUtf8());
    datagrama.replace(133, qMin(area.length(), 15), area.toUtf8());

    Message message;
    message.data = datagrama;
    message.type = Message::CrearUsuarioDB;
    qDebug() << "[UI] Enviado mensaje de creacion de usuario a la DB. Username: " << username;

    // Enviar el mensaje al servidor
    client.sendMessage(message);
}

void AgregarUsuarios::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void AgregarUsuarios::on_logout_clicked()
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

void AgregarUsuarios::setTabOrder() {
    // nombre, apellido, correo, telefono, cedula,
    // contra, puesto, bruto, neto, rol
    QWidget::setTabOrder(ui->username, ui->nombre);
    QWidget::setTabOrder(ui->nombre, ui->apellido);
    QWidget::setTabOrder(ui->apellido, ui->correo);
    QWidget::setTabOrder(ui->correo, ui->telefono);
    QWidget::setTabOrder(ui->telefono, ui->cedula);
    QWidget::setTabOrder(ui->cedula, ui->password);
    QWidget::setTabOrder(ui->password, ui->puesto);
    QWidget::setTabOrder(ui->puesto, ui->salarioBruto);
    QWidget::setTabOrder(ui->salarioBruto, ui->salarioNeto);
    QWidget::setTabOrder(ui->salarioNeto, ui->rol);
}

void AgregarUsuarios::clearInputFields() {
    ui->username->clear();
    ui->password->clear();
    ui->rol->clear();
    ui->nombre->clear();
    ui->apellido->clear();
    ui->correo->clear();
    ui->telefono->clear();
    ui->cedula->clear();
    ui->puesto->clear();
    ui->salarioBruto->clear();
    ui->salarioNeto->clear();
}


