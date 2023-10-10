#ifndef SERVIDORINTERMEDIARIO_H
#define SERVIDORINTERMEDIARIO_H

#include <QTcpServer>    // Librería para crear un servidor TCP
#include "Message.h"    // Incluye la clase Message
#include "Cliente/Client.h"

class ServidorIntermediario : public QTcpServer
{
    Q_OBJECT

public:
    // Constructor de la clase Server
    explicit ServidorIntermediario(QObject *parent = nullptr, quint16 port = 1678, quint16 FS_ServerPort = 1337, quint16 DB_ServerPort = 1338, QString FS_ServerIP = "127.0.0.1"
                                   , QString DB_ServerIP = "127.0.0.1");
    // Método para iniciar el servidor.
    bool startServer();

protected:
    // Se invoca cuando se establece una nueva conexión entrante.
    void incomingConnection(qintptr socketDescriptor);
private:
    QMap<QSslSocket*, quint64> m_clients;
    QMap<quint64, QSslSocket*> m_transactions;
    Client* FS_Server;
    Client* DB_Server;
    // El puerto en el que el servidor escucha las conexiones entrantes.
    quint16 port;
    // El puerto FS_Server
    quint16 FS_ServerPort;
    // El puerto DB_Server
    quint16 DB_ServerPort;
    // El IP FS_Server
    QString FS_ServerIP;
    // El IP DB_Server
    QString DB_ServerIP;

    quint64 m_transactionID = 0;

    // Maneja la lógica relacionada con una nueva conexión.
    void handleNewConnection(qintptr socketDescriptor);
    // Envia un mensaje al Socket
    void sendMessage(QSslSocket* socket, const Message& message);
    // Maneja el mensaje recibido (solicitud) desde el socket.
    void handleReceivedMessage(QSslSocket* socket, const Message& message);
    // Maneja el mensaje recibido (respuesta) desde el socket.
    Message handleRequest(QSslSocket *socket, const QByteArray &message, Message::MessageType type);
    // Asigna un TransactionID a un mensaje
    quint64 assignTransactionID();

private slots:
    // Slot que se ejecuta cuando el socket está listo para leer datos.
    void onSocketReadyRead();
    // Slot para los mensajes provenientes del servidor de archivos
    void onFS_ServerMessageReceived(const Message& message);
    // Slot para los mensajes provenientes del servidor de base de datos
    void onDB_ServerMessageReceived(const Message& message);

    // Manejo de Errores en Socket de FS_Server
    void onFS_ServerError(QAbstractSocket::SocketError socketError);
    // Manejo de Errores en Socket de DB_Server
    void onDB_ServerError(QAbstractSocket::SocketError socketError);
};
#endif // SERVIDORINTERMEDIARIO_H
