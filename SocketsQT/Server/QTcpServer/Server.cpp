#include "Server.h"
#include <QTcpSocket>
#include <QDataStream>

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
   // Constructor de la clase Server
}

bool Server::startServer()
{
    int port = 1337; // Puerto en el que se escucharán las conexiones

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
    QTcpSocket* socket = new QTcpSocket(this);
    // Asigna el descriptor de socket al nuevo socket TCP
    socket->setSocketDescriptor(socketDescriptor);
    // Conecta la señal readyRead() del socket a la ranura onSocketReadyRead()
    connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    // Conecta la señal disconnected() del socket directamente a su ranura deleteLater()
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
     // Se obtiene la información del cliente para poder imprimirlo en la consola
    qDebug() << "New connection from " << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void Server::onSocketReadyRead()
{
    // Obtiene el socket TCP que ha emitido la señal
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (socket)
    {
        // Crea un flujo de datos de entrada asociado al socket
        QDataStream in(socket);

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


void Server::sendMessage(QTcpSocket* socket, const Message& message)
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


void Server::handleReceivedMessage(QTcpSocket* socket, const Message& message)
{
    Message::MessageType type = message.type;
    QByteArray content = message.data;

    qDebug() << "Received message: Type = " << type << ", Content = " << content;

    // Handle the message based on its type
    switch (type)
    {
    case Message::Text: // Maneja texto
        qDebug() << "Received text message";
        break;
    case Message::Pdf: // Maneja archivo pdf
        qDebug() << "Received pdf";
        break;
    case Message::Audio: // Maneja audio
        qDebug() << "Received audio";
        break;
    case Message::Video: // Maneja video
        qDebug() << "Received video";
        break;
    default:
        qDebug() << "Unknown message type";
        break;
    }
    // Se crea un objeto de mensaje llamado "response"
    Message response;
    // Se establece el tipo del mensaje como "Text"
    response.type = Message::Text;
    // Se asigna el contenido del mensaje como "Server Received your request"
    response.data = "Server Received your request";
    // Se llama a la función "sendMessage" para enviar el mensaje al socket
    sendMessage(socket, response);
}
