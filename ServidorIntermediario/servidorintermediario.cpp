#include "servidorintermediario.h"
#include <QSslSocket>
#include <QDataStream>

ServidorIntermediario::ServidorIntermediario(QObject *parent, quint16 port, quint16 FS_ServerPort, quint16 DB_ServerPort, QString FS_ServerIP, QString DB_ServerIP) :
    QTcpServer(parent),
    port(port),
    FS_ServerPort(FS_ServerPort),
    DB_ServerPort(DB_ServerPort),
    FS_ServerIP(FS_ServerIP),
    DB_ServerIP(DB_ServerIP)
{
   // Constructor de la clase ServidorIntermediario
   this->FS_Server = new Client(this);
   this->DB_Server = new Client(this);
   this->FS_Server->connectToServer(FS_ServerIP, FS_ServerPort);
    this->DB_Server->connectToServer(DB_ServerIP, DB_ServerPort);

    // Conexión de señales y slots para el manejo de mensajes
   connect(this->FS_Server, SIGNAL(dataReceived(const Message&)), this, SLOT(onFS_ServerMessageReceived(const Message&)));
   connect(this->DB_Server, SIGNAL(dataReceived(const Message&)), this, SLOT(onDB_ServerMessageReceived(const Message&)));

   // Conexión de señales y slots para el manejo de errores
   connect(this->FS_Server->getSocket(), &QAbstractSocket::errorOccurred, this, &ServidorIntermediario::onFS_ServerError);
   connect(this->DB_Server->getSocket(), &QAbstractSocket::errorOccurred, this, &ServidorIntermediario::onDB_ServerError);

}

bool ServidorIntermediario::startServer()
{
    int port = 1678; // Puerto en el que se escucharán las conexiones

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

void ServidorIntermediario::incomingConnection(qintptr socketDescriptor)
{
    // Tenemos una nueva conexión
    // Muestra en la consola el descriptor del socket que se está conectando
    qDebug() << "\n" << socketDescriptor << "Connecting...";
    // Maneja la nueva conexión en un método separado
    handleNewConnection(socketDescriptor);
}

void ServidorIntermediario::handleNewConnection(qintptr socketDescriptor)
{
    // Crea un nuevo socket TCP para la conexión entrante
    QSslSocket* socket = new QSslSocket(this);
    // Asigna el descriptor de socket al nuevo socket TCP
    socket->setSocketDescriptor(socketDescriptor);
    // Conecta la señal readyRead() del socket a la ranura onSocketReadyRead()
    connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    // Conecta la señal disconnected() del socket directamente a su ranura deleteLater()
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(Cliente::sslError(QList<QSslError>)));
    socket->setPrivateKey("../ServidorFS/server.key", QSsl::Rsa);
    socket->setLocalCertificate("../ServidorFS/server.crt");

    socket->startServerEncryption();

    quint64 transactionID = this->assignTransactionID();
    // Se asigna el TransactionID al socket
    m_clients.insert(socket, transactionID);

    // Se asigna el socket al TransactionID
    m_transactions.insert(transactionID, socket);

    // Se obtiene la información del cliente para poder imprimirlo en la consola
    qDebug() << "New connection from " << socket->peerAddress().toString() << ":" << socket->peerPort() << " assigned TransactionID : " << transactionID;
}

void ServidorIntermediario::onSocketReadyRead()
{
    // Obtiene el socket TCP que ha emitido la señal
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());

    if (socket)
    {
        // Crea un flujo de datos de entrada asociado al socket
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_0);

        // Crea un objeto Message para almacenar el mensaje recibido
        Message message;

        // Lee los datos disponibles en el flujo hasta que no haya más datos por leer
        while (!in.atEnd())
        {


            // Deserializa el mensaje desde el flujo de datos
            in >> message;


        }
        // Maneja el mensaje recibido
        handleReceivedMessage(socket, message);
    }
}


void ServidorIntermediario::sendMessage(QSslSocket* socket, const Message& message)
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


void ServidorIntermediario::handleReceivedMessage(QSslSocket* socket, const Message& message)
{
    Message::MessageType type = message.type;
    QByteArray content = message.data;
    quint64 transaction;

    Client* receiver = nullptr;
    qDebug() << "[SI] Received message: Type = " << type << ", Content = " << content;
    
    Message response = handleRequest(socket, content, type);

    switch (type)
    {
    case Message::Autenticacion:
    case Message::CrearUsuarioFS:
        receiver = this->FS_Server;
        break;
    case Message::CrearUsuarioDB:
    case Message::EditarUsuarioDB:
        receiver = this->DB_Server;
        break;
    case Message::ConsultarSalarioBase:
    case Message::ConsultarSaldoVacaciones:
    case Message::SolicitudVacaciones:
    case Message::SolicitudConstanciaSalarial:
    case Message::SolicitudConstanciaLaboral:
    case Message::ConsultarEstadoSolicitudesHechas:
    case Message::ConsultarSolicitudesPendientes:
    case Message::ModificarEstadoConstancia:
    case Message::ConsultarConstancia:
    case Message::ConsultarAnotacionesAlExpediente:
    case Message::AgregarAnotacionAlExpediente:
    case Message::SolicitarAnotacionesAlExpediente:
    case Message::ConsultarComprobantesDePago:
    case Message::ConsultarListaDeUsuarios:
        receiver = this->DB_Server;
        break;
    case Message::ConsultarSolicitudVacaciones:
        receiver = this->DB_Server;
        break;
    case Message::EnlistarUsuarios:
        receiver = this->DB_Server;
        break;
    case Message::ConsultarDatosDeUsuario:
        receiver = this->DB_Server;
        break;
    case Message::EliminarUsuarioDB:
        receiver = this->DB_Server;
        break;
    case Message::EliminarUsuarioFS:
        receiver = this->FS_Server;
        break;
    default:
        qDebug() << "[SI] Unknown message type received";
        break;
    }

    qDebug() << "[SI] Sending message: Type = " << response.type << ", Content = " << response.data << ", TransactionID = " << response.transactionID;

    receiver->sendMessage(response);
}

Message ServidorIntermediario::handleRequest(QSslSocket* socket, const QByteArray &message, Message::MessageType type)
{
    Message response;
    response.type = type;
    response.data = message;
    response.transactionID = this->m_clients.value(socket);

    QString messageText = "Unknown message type";
    switch (type) {
        case Message::Autenticacion:
            messageText = "Autenticacion enviada al FS, con transaction ID : ";
            break;
        case Message::CrearUsuarioFS:
            messageText = "Solicitud de creacion de usuario enviada al FS";
            break;
        case Message::CrearUsuarioDB:
            messageText = "Solicitud de creacion de usuario enviada a la BDD";
            break;
        case Message::ConsultarSalarioBase:
            messageText = "Solicitud de salario base enviado a la BDD";
            break;
        case Message::ConsultarSaldoVacaciones:
            messageText = "Consulta de saldo de vacaciones enviado a la BDD";
            break;
        case Message::SolicitudVacaciones:
            messageText = "Solicitud de vacaciones enviada a la BDD";
            break;
        case Message::SolicitudConstanciaSalarial:
            messageText = "Solicitud de Constancia Salarial enviada a la BDD";
            break;
        case Message::SolicitudConstanciaLaboral:
            messageText = "Solicitud de Constancia Laboral enviada a la BDD";
            break;
        case Message::ConsultarEstadoSolicitudesHechas:
            messageText = "Consulta de estado de solicitudes hechas enviada a la BDD";
            break;
        case Message::ConsultarSolicitudesPendientes:
            messageText = "Consulta de solicitudes pendientes enviada a la BDD";
            break;
        case Message::ModificarEstadoConstancia:
            messageText = "Modificar estado de constancia enviado a la BDD";
            break;
        case Message::ConsultarConstancia:
            messageText = "Consulta de constancia enviada a la BDD";
            break;
        case Message::ConsultarAnotacionesAlExpediente:
            messageText = "Consulta de anotaciones enviada a la BDD";
            break;
        case Message::AgregarAnotacionAlExpediente:
            messageText = "Agregar anotacion enviada a la BDD";
            break;
        case Message::ConsultarSolicitudVacaciones:
            messageText = "Consulta de vacaciones enviada a la BDD";
            break;
        case Message::SolicitarAnotacionesAlExpediente:
            messageText = "Solicitud de Anotaciones al expediente enviada a la BDD";
            break;
        case Message::ConsultarListaDeUsuarios:
            messageText = "Consulta de lista de usuarios enviada a la BDD";
            break;
        case Message::ConsultarComprobantesDePago:
            messageText = "Consulta de comprobantes de pago enviada a la BDD";
            break;
        case Message::EliminarUsuarioDB:
            messageText = "Solicitud de eliminacion de usuario enviada a la BDD";
            break;
        case Message::EliminarUsuarioFS:
            messageText = "Solicitud de eliminacion de usuario enviada al FS";
            break;
    }

    qDebug() << "[SI] " << messageText << response.transactionID;
    return response;
}

void ServidorIntermediario::onFS_ServerMessageReceived(const Message &message)
{
    // Se obtiene el TransactionID del mensaje
    quint64 transactionID = message.transactionID;
    // Se obtiene el socket asociado al TransactionID
    QSslSocket* socket = m_transactions.value(transactionID);

    qDebug() << "[SI] FS_ServerMessageReceived: Type = " << message.type << ", Content = " << message.data << ", TransactionID = " << message.transactionID;

    // Se envía el mensaje al socket
    sendMessage(socket, message);
}

void ServidorIntermediario::onDB_ServerMessageReceived(const Message &message)
{
    // Se obtiene el TransactionID del mensaje
    quint64 transactionID = message.transactionID;
    // Se obtiene el socket asociado al TransactionID
    QSslSocket* socket = m_transactions.value(transactionID);

    qDebug() << "[SI] DB_ServerMessageReceived: Type = " << message.type << ", Content = " << message.data << ", TransactionID = " << message.transactionID;

    // Se envía el mensaje al socket
    sendMessage(socket, message);
}

quint64 ServidorIntermediario::assignTransactionID()
{
    return m_transactionID++;
}


void ServidorIntermediario::onFS_ServerError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "[SI] FileSystem Server: Connection Refused Error\n Please check if the server is running."
         << "\n If it is, check if the port and the IP are correct.";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "[SI] FileSystem Server: Host Not Found Error\n Please check if the server is running."
         << "\n If it is, check if the port and the IP are correct.";
        break;
    default:
        qDebug() << "[SI] FileSystem Server: " << this->FS_Server->getSocket()->errorString();
        break;
    }

    // Podriamos encolar las solicitudes para que el servidor no se caiga
    this->close();
    this->deleteLater();
}

void ServidorIntermediario::onDB_ServerError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "[SI] DataBase Server: Connection Refused Error\n Please check if the server is running."
                 << "\n If it is, check if the port and the IP are correct.";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "[SI] DataBase Server: Host Not Found Error\n Please check if the server is running."
                 << "\n If it is, check if the port and the IP are correct.";
        break;
    default:
        qDebug() << "[SI] DataBase Server: " << this->DB_Server->getSocket()->errorString();
        break;
    }

    // Podriamos encolar las solicitudes para que el servidor no se caiga
    this->close();
    this->deleteLater();
}
