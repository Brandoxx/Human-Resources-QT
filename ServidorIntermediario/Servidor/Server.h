#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>    // Librería para crear un servidor TCP
#include "Message.h"    // Incluye la clase Message

class Server : public QTcpServer
{
    Q_OBJECT
public:
    // Constructor de la clase Server
    explicit Server(QObject *parent = nullptr);
    // Método para iniciar el servidor.
    bool startServer();

protected:
    // Se invoca cuando se establece una nueva conexión entrante.
    void incomingConnection(qintptr socketDescriptor);
private:
    // Maneja la lógica relacionada con una nueva conexión.
    void handleNewConnection(qintptr socketDescriptor);
    // Envia un mensaje al Socket
    void sendMessage(QTcpSocket* socket, const Message& message);
    // Maneja el mensaje recibido (solicitud) desde el socket.
    void handleReceivedMessage(QTcpSocket* socket, const Message& message);

private slots:
    // Slot que se ejecuta cuando el socket está listo para leer datos.
    void onSocketReadyRead();
};

#endif // SERVER_H


