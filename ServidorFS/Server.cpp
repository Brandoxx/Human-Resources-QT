#include "Server.h"
#include <QSslSocket>
#include <QDataStream>
#include <string>

Server::Server(QObject *parent, AdministradorDeArchivos *admin, quint16 port) :
    QTcpServer(parent),
    admin(admin),
    port(port)
{
   // Constructor de la clase Server
    this->admin = admin;
    this->admin->crearArchivo("Usuario.dat", "Grupazo");
    char datos[] = "jorge.mora,78efbfbd045d684aefbfbd2eefbfbdefbfbd2375efbfbd3cd78148efbfbd,3";
    this->admin->agregarDatosArchivo("Usuario.dat", datos);

    // Lee de user.txt y crea los usuarios
    this->leerUsuarios("../ServidorFS/user.txt");
}

bool Server::startServer()
{

    if(!this->listen(QHostAddress::AnyIPv4, port)) // QHostAddress::Any es para escuchar en cualquier puerto
    {
        // No se pudo iniciar el servidor, muestra un mensaje de error
        qDebug() << "Could not start server";
        return false;
    }
    else
    {
        // El servidor se ha iniciado correctamente, muestra el puerto al que se está escuchando
        qDebug() << "Listening to port " << port << "...";
        return true;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // Tenemos una nueva conexión
    // Muestra en la consola el descriptor del socket que se está conectando
    qDebug() << "\n" << socketDescriptor << "Connecting...";
    // Maneja la nueva conexión en un método separado
    handleNewConnection(socketDescriptor);
}

void Server::handleNewConnection(qintptr socketDescriptor)
{
    // Crea un nuevo socket TCP para la conexión entrante
    QSslSocket* socket = new QSslSocket(this);
    // Asigna el descriptor de socket al nuevo socket TCP
    socket->setSocketDescriptor(socketDescriptor);
    // Conecta la señal readyRead() del socket a la ranura onSocketReadyRead()
    connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    // Conecta la señal disconnected() del socket directamente a su ranura deleteLater()
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslError(QList<QSslError>)));
    socket->setPrivateKey("../ServidorFS/server.key", QSsl::Rsa);
    socket->setLocalCertificate("../ServidorFS/server.crt");
    socket->startServerEncryption();
     // Se obtiene la información del cliente para poder imprimirlo en la consola
    qDebug() << "New connection from " << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void Server::onSocketReadyRead()
{
    // Obtiene el socket TCP que ha emitido la señal
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());

    if (socket)
    {
        // Crea un flujo de datos de entrada asociado al socket
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_0);
        // Lee los datos disponibles en el flujo hasta que no haya más datos por leer
        while (!in.atEnd())
        {
            // Crea un objeto Message para almacenar el mensaje recibido
            Message message;

            // Deserializa el mensaje desde el flujo de datos
            in >> message;

            // Maneja el mensaje recibido
            handleReceivedMessage(socket, message);
        }
    }
}


void Server::sendMessage(QSslSocket* socket, const Message& message)
{
    // Crea un arreglo de bytes para almacenar los datos serializados
    QByteArray block;

    // Crea un flujo de datos de salida asociado al arreglo de bytes
    QDataStream out(&block, QIODevice::WriteOnly);

    // Establece la versión del protocolo de serialización del flujo de datos
    out.setVersion(QDataStream::Qt_5_0);

    // Serializa el mensaje en el flujo de datos
    out << message;

    // Escribe el arreglo de bytes en el socket para enviar el mensaje al cliente
    socket->write(block);
}


void Server::handleReceivedMessage(QSslSocket* socket, const Message& message)
{
    Message::MessageType type = message.type;
    QByteArray recievedDatagram = message.data;
    QString username;
    QString password;
    quint32 transactionID = message.transactionID;
    // Se crea un objeto de mensaje llamado "response"
    Message response;
    bool userCreated = false;
    int accessRights = 0;

    qDebug() << "Received message: Type = " << type << ", Content = " << recievedDatagram;

    // Handle the message based on its type
    switch (type)
    {
    case Message::Text: // Maneja texto
        qDebug() << "Received text message";
        break;
    case Message::Autenticacion: // Maneja archivo pdf
        qDebug() << "Received Autentication request";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        password = recievedDatagram.mid(10, 30).toHex();
        qDebug() << "User = " << username << ", Password = " << password;
        // Se establece el tipo del mensaje como "Autenticacion"
        response.type = Message::Autenticacion;
        // Se asigna el contenido del mensaje como "los permisos"
        accessRights = admin->autenticarUsuario(username.toStdString(), password.toStdString());
        response.data.append(std::to_string(accessRights).c_str());
        break;
    case Message::CrearUsuarioFS: // Crea un usuario
        qDebug() << "Received Create User request";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        password = recievedDatagram.mid(10, 30).toHex();
        accessRights = recievedDatagram.right(1).toInt();
        qDebug() << "[FS] Creating user: " << username << ", Password = " << password
         << ", Access Rights = " << accessRights;
        // Se establece el tipo del mensaje como "CrearUsuario"
        response.type = Message::CrearUsuarioFS;
        // Se asigna el contenido del mensaje como "los permisos"
        userCreated = admin->crearUsuario(username.toStdString(), password.toStdString(), accessRights);
        if(userCreated)
        {
            response.data.append("1");
        }
        else
        {
            response.data.append("0");
        }
        break;
    case Message::EliminarUsuarioFS:
        {qDebug() << "Received Eliminate User request";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        qDebug() << "[FS] Eliminating user: " << username;
        response.type = Message::EliminarUsuarioFS;
        int error = admin->borrarUsuario(username.toStdString());
        if(error == 0)
        {
            response.data.append("0");
        }
        else
        {
            response.data.append("1");
        }
        break;}
    default:
        qDebug() << "Unknown message type";
        break;
    }
    
    // Se asigna el TransactionID al mensaje de respuesta
    response.transactionID = transactionID;

    qDebug() << "[FS] Sending response. ID = " << transactionID;
    // Se llama a la función "sendMessage" para enviar el mensaje al socket
    sendMessage(socket, response);
}

void Server::leerUsuarios(QString archivo) {
    // Se lee el archivo de usuarios
    QFile file(archivo);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[FS] Error: could not open file" << archivo;
        return;
    }

    qDebug() << "[FS] Reading users from file:" << archivo;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() == 3) {
            QString username = fields[0];
            QString password = fields[1];
            QString accessRights = fields[2];
            qDebug() << " Hash " << password;
            // Hash the password using Sha-224
            QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha224);
            // Anadelos ceros que faltan para hacer 30 bytes
            hashedPassword.append(QByteArray(30 - hashedPassword.size(), 0));
            password = hashedPassword.toHex();

            qDebug() << "[FS] Adding user:" << username << "with access rights:" << accessRights;
            qDebug() << " Hash " << password;
            // Add the user to the file system
            admin->crearUsuario(username.toStdString(), password.toStdString(), accessRights.toInt());
        }
    }

    qDebug() << "[FS] Finished reading users from file:" << archivo;

    file.close();
}

void Server::sslError(const QList<QSslError> &errors) {
    qDebug() << "SSL Errors: " << errors;
}

