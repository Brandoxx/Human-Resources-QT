#include "BorrarUsuarios.h"
#include "ui_BorrarUsuarios.h"
#include <QMessageBox>
#include <QRegularExpression>


BorrarUsuarios::BorrarUsuarios(QWidget *parent, AdministradorMenu *AdministradorMenuParent, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::BorrarUsuarios),
    parentWindow(parent),
    AdministradorMenuParentWindow(AdministradorMenuParent),
    client(client)
{
    ui->setupUi(this);
    // Se obtiene una referencia al QVBoxLayout existente en el diseño principal
    QVBoxLayout *verticalLayout = ui->verticalLayout_2; // Cambio aquí al QVBoxLayout deseado

    // Se crea un QHBoxLayout adicional para centrar listaWidget
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    // horizontalLayout->setAlignment(Qt::AlignCenter);

    // Se agrega listaWidget al QHBoxLayout
    listaWidget = new QListWidget();
    horizontalLayout->addWidget(listaWidget);

    // Se agrega el QHBoxLayout al QVBoxLayout
    verticalLayout->addLayout(horizontalLayout);

    connect(&client, &Client::dataReceived, this, &BorrarUsuarios::procesarUsuarios);

    this->mostrarWidgets();

    QObject::connect(&client, &Client::dataReceived, this, &BorrarUsuarios::receiveResponseFromServer);
}

BorrarUsuarios::~BorrarUsuarios()
{
    delete ui;
}

void BorrarUsuarios::on_acceptButton_clicked()
{
    // Agregar accion
}


void BorrarUsuarios::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void BorrarUsuarios::on_logout_clicked()
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


void BorrarUsuarios::eliminarUsuario(QString username) {

    int usuarioEliminadoFS = 0;
    int usuarioEliminadoDB = 0;

    QByteArray datagramaFS(10, '\0');
    datagramaFS.replace(0, qMin(username.length(), 10), username.toUtf8());
    Message messageFS;
    messageFS.data = datagramaFS;
    messageFS.type = Message::EliminarUsuarioFS;

    qDebug() << "[UI] Enviado mensaje de eliminar usuario al FS. Username: " << username;
    // Enviar el mensaje al servidor
    client.sendMessage(messageFS);

    QEventLoop loop;
    connect(this, &BorrarUsuarios::response_readFS, &loop, &QEventLoop::exit);
    usuarioEliminadoFS = loop.exec();

    QByteArray datagramaDB(10, '\0');
    datagramaDB.replace(0, qMin(username.length(), 10), username.toUtf8());

    Message messageDB;
    messageDB.data = datagramaDB;
    messageDB.type = Message::EliminarUsuarioDB;
    qDebug() << "[UI] Enviado mensaje de eliminar usuario a la DB. Username: " << username;

    // Enviar el mensaje al servidor
    client.sendMessage(messageDB);
    QEventLoop dbWait;
    connect(this, &BorrarUsuarios::response_readDB, &dbWait, &QEventLoop::exit);
    usuarioEliminadoDB = dbWait.exec();

    if(usuarioEliminadoFS == EXIT_SUCCESS && usuarioEliminadoDB == EXIT_SUCCESS) {
        qDebug() << "[UI] Recibido exito al eliminar usuario";
        QString mensaje = "El usuario " + username + " fue eliminado!";
        QMessageBox::information(this,"Éxito!", mensaje);
    } else {
        qDebug() << "[UI] Recibido error al eliminar usuario";
        QString mensaje = "Hubo un error al eliminar el usuario " + username;
        QMessageBox::information(this,"Error!", mensaje);
    }
}

void BorrarUsuarios::receiveResponseFromServer(const Message &message)
{
    if (message.type == Message::EliminarUsuarioFS){
        // Interpreta el booleano enviado por el server
        const char* fsEliminated = message.data.constData();
        int userEliminatedFS =  std::stoi(fsEliminated);
        qDebug() << "[UI] Recibido mensaje de respuesta del FS";

        emit response_readFS(userEliminatedFS);
    } else {
        const char* dbEliminated = message.data.constData();
        int userEliminatedDB =  std::atoi(dbEliminated);
        qDebug() << "[UI] Recibido mensaje de respuesta de la BDD";
        emit response_readDB(userEliminatedDB);
    }
}

void BorrarUsuarios::mostrarWidgets() {
    // Pedir lista de solicitudes a serverBD
    QEventLoop loop;
    QString username, fullname;

    Message request;
    request.type = Message::EnlistarUsuarios;
    // Agregar cada solicitud a un vector de solicitudes

    connect(this, &BorrarUsuarios::usuariosProcesados, &loop, &QEventLoop::quit);

    client.sendMessage(request);

    loop.exec();

    for (size_t i = 0; i < this->usuarios.size(); i++) {

        QStringList usuario = this->usuarios[i].split("\t");
        username = usuario[0];
        fullname = usuario[1];

        // Crea el widget personalizado para cada usuario
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);


        // Crea una etiqueta para mostrar la información del usuario
        QLabel* infoLabel = new QLabel(QString("%1\t%2").arg(username).arg(fullname));
        infoLabel->setObjectName("infoLabel"); // Asigna un nombre de objeto a la etiqueta
        layout->addWidget(infoLabel);

        // Crea los botones para ver detalles, aceptar y rechazar
        QPushButton* detallesButton = new QPushButton("Eliminar");

        // Establece los colores de fondo a los botones
        detallesButton->setStyleSheet("background-color: lightGray; width: 20px;");

        // Conecta la señal de clic de los botones con sus respectivas funciones, y establece el índice como userData
        connect(detallesButton, &QPushButton::clicked, this, [=]() { eliminarUsuario(username); });

        // Agrega los botones al diseño horizontal
        layout->addWidget(detallesButton);

        // Establece el widget personalizado como el elemento de lista
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(widget->sizeHint());
        listaWidget->addItem(item);
        listaWidget->setItemWidget(item, widget);
    }
}

void BorrarUsuarios::procesarUsuarios(const Message& message)
{
    QString data = QString::fromUtf8(message.data);
    if (message.type == Message::EnlistarUsuarios) {
        if (message.data == "0") {
            QMessageBox::information(this, "Usuarios", "No hay usuarios en la DB.");
        } else {
            QStringList usuarios = QString(message.data).split("\n", Qt::SkipEmptyParts);
            for (QString usuario : usuarios) {
                this->usuarios.append(usuario);
            }
        }
        emit usuariosProcesados(0);
    }
}




