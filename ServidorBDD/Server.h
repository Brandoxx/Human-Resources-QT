#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>    // Librería para crear un servidor TCP
#include <QSslSocket>    // Librería para la comunicación a través de sockets TCP

#include "Message.h"    // Incluye la clase Message
#include "DbManager.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    // Constructor de la clase Server
    explicit Server(QObject *parent = nullptr, DbManager *dbmanager = new DbManager(), quint16 port = 1338);
    // Método para iniciar el servidor.
    bool startServer();


protected:
    // Se invoca cuando se establece una nueva conexión entrante.
    void incomingConnection(qintptr socketDescriptor);
private:

    // Maneja la lógica relacionada con una nueva conexión.
    void handleNewConnection(qintptr socketDescriptor);
    // Envia un mensaje al Socket
    void sendMessage(QSslSocket* socket, const Message& message);
    // Maneja el mensaje recibido (solicitud) desde el socket.
    void handleReceivedMessage(QSslSocket* socket, const Message& message);

    Message handleConsultaSupervisor(quint8 estado, QString username = "");

    Message handleConsultaComprobantes(QString username = "");
    // El administrador de archivos
    DbManager *dbmanager;
    // El puerto en el que el servidor escucha las conexiones entrantes.
    quint16 port;

private slots:
    // Slot que se ejecuta cuando el socket está listo para leer datos.
    void onSocketReadyRead();
    void sslError(const QList<QSslError> &errors);
};

#endif // SERVER_H


