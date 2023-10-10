#include "Client.h" // Se incluye el encabezado de Client
#include <QDataStream> // Librería para el flujo de datos

Client::Client(QObject *parent) : QObject(parent)
{
    // Se crea un nuevo objeto QTcpSocket y se asigna  al puntero "socket"
    socket = new QSslSocket(this);
    // Se conecta la señal connected() del socket al slot onSocketConnected() de la clase Client
    connect(socket, &QSslSocket::encrypted, this, &Client::onSocketConnected);
    // Se conecta  la señal disconnected() del socket al slot onSocketDisconnected() de la clase Client
    connect(socket, &QSslSocket::disconnected, this, &Client::onSocketDisconnected);
    // Se conecta  la señal readyRead() del socket al slot onSocketReadyRead() de la clase Client
    connect(socket, &QSslSocket::readyRead, this, &Client::onSocketReadyRead);
    // Se conecta  la señal errorOccurred() del socket al slot socketError() de la clase Client
    connect(socket, &QAbstractSocket::errorOccurred, this, &Client::socketError);
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslError(const QList<QSslError> &)));
}

void Client::connectToServer(const QString& ipAddress, quint16 port)
{
    // Se conecta  el socket al servidor utilizando la dirección IP y el puerto especificado
    socket->connectToHostEncrypted(ipAddress, port);
}

void Client::disconnectFromServer()
{
    // Se desconecta el socket del servidor
    socket->disconnectFromHost();
}

void Client::sendMessage(const Message& message)
{
    // Se crea un QByteArray para almacenar los datos del mensaje
    QByteArray block;
    // Se crea un flujo de datos QDataStream y asociarlo con el QByteArray en modo de escritura
    QDataStream out(&block, QIODevice::WriteOnly);
    // Se establece la versión del flujo de datos
    out.setVersion(QDataStream::Qt_5_0);
    // Se escribe el objeto de mensaje en el flujo de datos
    out << message;
    // Se escribe los datos en el socket
    socket->write(block);
    // Se espera a que los bytes sean escritos en el socket antes de continuar
    if (!socket->waitForBytesWritten(3000)) {
        // Ocurrió un error al escribir los datos en el socket
        qDebug() << "Error writing data to socket:" << socket->errorString();
    }
    // Vaciar el búfer del socket para asegurar que los datos se envíen inmediatamente
    //socket->flush();
    // Se inprime un mensaje de depuración
    qDebug() << "Message sent.";
}

void Client::onSocketConnected()
{
    // Se emite la señal connected() para indicar que se ha establecido la conexión con el servidor
    emit connected();
}

void Client::onSocketDisconnected()
{
    // Se emite la señal disconnected() para indicar que se ha cerrado la conexión con el servidor
    emit disconnected();
    exit(0);
}


void Client::onSocketReadyRead()
{
    // Se crea un flujo de datos QDataStream y se asocia con el socket
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);
    // Leer todos los datos disponibles en el flujo de datos
    while (!in.atEnd())
    {
        // Crear un objeto de mensaje para almacenar los datos recibidos
        Message message;
        // Leer el objeto de mensaje desde el flujo de datos
        in >> message;
        // Se emite la señal dataReceived() con el objeto de mensaje recibido
        emit dataReceived(message);
    }
}

void Client::socketError(QAbstractSocket::SocketError socketError)
{
    // Se emite la señal error() con el mensaje de error del socket
    emit error(socket->errorString());
}

QSslSocket* Client::getSocket() const
{
    return socket;
}

void Client::sslError(const QList<QSslError> &errors) {
    qDebug() << "SSL Errors: " << errors;
    socket->ignoreSslErrors();
}

