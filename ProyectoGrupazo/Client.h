#ifndef CLIENT_H
#define CLIENT_H

#include <QObject> // Librería para la programación orientada a objetos en Qt
#include <QSslSocket> // Librería para la comunicación a través de sockets TCP
#include <QAbstractSocket> // Librería base para los sockets en Qt
#include "Message.h" // Se incluye el encabezado de Message

class Client : public QObject
{
    Q_OBJECT
public:
    // Constructor de la clase Client
    explicit Client(QObject *parent = nullptr);

public slots:
    // Slot para conectarse al servidor
    void connectToServer(const QString& ipAddress, quint16 port);
    // Slot para enviar un mensaje al servidor
    void sendMessage(const Message& message);
    // Slot para enviar diferentes tipos de mensaje al servidor (ejemplo específico)
    void sendMessages();
    static Client& getInstance();
    // Slot para desconectarse del servidor
    void disconnectFromServer();

signals:
    // Señal emitida cuando se establece la conexión con el servidor
    void connected();
    // Señal emitida cuando se cierra la conexión con el servidor
    void disconnected();
    // Señal emitida cuando ocurre un error
    void error(const QString& errorMessage);
    // Señal emitida cuando se recibe un mensaje del servidor
    void dataReceived(const Message& message);

private slots:
    // Slot llamado cuando se establece la conexión con el servidor
    void onSocketConnected();
    // Slot llamado cuando se cierra la conexión con el servidor
    void onSocketDisconnected();
    // Slot llamado cuando hay datos listos para leer desde el servidor
    void onSocketReadyRead();
    // Slot llamado cuando ocurre un error en el socket
    void socketError(QAbstractSocket::SocketError socketError);

    void sslError(const QList<QSslError> &errors);

private:
    // Objeto QTcpSocket utilizado para la comunicación con el servidor
    QSslSocket* socket;
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
};

#endif // CLIENT_H
