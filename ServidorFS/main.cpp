#include <QCoreApplication>
#include "Server.h"
#include "Logger.hpp"

quint16 readPortFromFile()
{
    QFile file("../network.config");
    quint16 port = 0;
    QString IP;
    QString linePort;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error al abrir el archivo de configuración, no se pudo abrir el archivo";
        return 0;
    } else {
        qDebug() << "Archivo de configuración abierto correctamente";
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        // Si encontramos uns linea que diga --DB Server--, leemos las proximas dos lineas
        if (in.readLine() == "--FS Server--") {
            IP = in.readLine();
            linePort = in.readLine();
            // Cortamos port:
            if (linePort.startsWith("port:")) {
                linePort.remove(0, 5);
            } else {
                qCritical() << "Error al leer el archivo de configuración, puerto no encontrado";
                return 0;
            }   
            qDebug() << "IP: " << IP << " port: " << linePort;
            break;
        }
    }

    file.close();

    return linePort.toUShort();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger::getInstance();
    qInstallMessageHandler(myMessageHandler);

    Server server(nullptr, new AdministradorDeArchivos(), readPortFromFile());
    if (server.startServer()) {
        qDebug() << "Server started successfully.";
    } else {
        qDebug() << "Failed to start server.";
        return 1;
    }

    return a.exec();
}
