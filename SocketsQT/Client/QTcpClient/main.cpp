#include <QCoreApplication> // Librería principal de Qt para aplicaciones sin interfaz gráfica
#include <QDebug> // Librería para imprimir mensajes de depuración en la consola
#include "Client.h" // Se incluye el encabezado de client

int main(int argc, char *argv[])
{
    // Se crea una instancia de la aplicación Qt sin interfaz gráfica
    QCoreApplication a(argc, argv);

    // Se crea un objeto de la clase Client
    Client client;

     // Se conectan señales y slots para manejar eventos

    // Establecer la dirección IP y el puerto del servidor
    client.connectToServer("127.0.0.1", 1337); // Utilizar esta version si se compila client y server en la misma computadora
    //Si es en dos computadoras distintas cambiar la direccion ip con la de su maquina local en el que esta el servidor
    QObject::connect(&client, &Client::connected, [&client]() {
        qDebug() << "Connected to the server.";

        // Se estableció la conexión, enviar un mensaje al servidor
        Message message;
        message.data = "Hello, server!";
        message.type = Message::Text;

        client.sendMessage(message);
    });



    QObject::connect(&client, &Client::disconnected, []() {
        qDebug() << "Disconnected from the server.";
    });

    QObject::connect(&client, &Client::error, [](const QString& errorMessage) {
        qDebug() << "Error:" << errorMessage;
    });

    QObject::connect(&client, &Client::dataReceived, [](const Message& message) {
        qDebug() << "Received message: Type =" << message.type << ", Content =" << message.data;
    });

    // Se agrega una impresión para verificar que se haya establecido la conexión antes de enviar mensajes
    qDebug() << "Waiting for connection...";

    return a.exec();
}
