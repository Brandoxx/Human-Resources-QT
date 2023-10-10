#include <QCoreApplication>
#include "servidorintermediario.h"
#include "Logger.hpp"

QPair<QString,quint16> readIPAndPortFromFile(QString serverName) {
    QFile file("../network.config");
    QString IP;
    quint16 port = 0;
    QString linePort;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error al abrir el archivo de configuración, no se pudo abrir el archivo";
        return QPair<QString,quint16>("",0);
    } else {
        qDebug() << "[SI] Archivo de configuración abierto correctamente";
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        // Si encontramos uns linea que diga --DB Server--, leemos las proximas dos lineas
        if (in.readLine() == serverName) {
            IP = in.readLine();
            linePort = in.readLine();
            // Cortamos port:
            if (linePort.startsWith("port:")) {
                linePort.remove(0, 5);
            } else {
                qCritical() << "[SI] Error al leer el archivo de configuración, puerto no encontrado";
                return QPair<QString,quint16>("",0);
            }   
            if (IP.startsWith("IP:")) {
                IP.remove(0, 3);
            } else {
                qCritical() << "[SI] Error al leer el archivo de configuración, IP no encontrada";
                return QPair<QString,quint16>("",0);
            }
            qDebug() << "[SI] IP: " << IP << " port: " << linePort;
            break;
        }
    }

    file.close();

    return QPair<QString,quint16>(IP,linePort.toUShort());
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger::getInstance();
    qInstallMessageHandler(myMessageHandler);
    quint16 port = readIPAndPortFromFile("--SI Server--").second;
    qDebug() << "[SI] Port: " << port;
    QPair<QString,quint16> ipAndPortDB = readIPAndPortFromFile("--DB Server--");
    qDebug() << "[SI] Database IP: " << ipAndPortDB.first << " Port: " << ipAndPortDB.second;

    QPair<QString,quint16> ipAndPortFS = readIPAndPortFromFile("--FS Server--");
    qDebug() << "[SI] File System IP: " << ipAndPortFS.first << " Port: " << ipAndPortFS.second;

    ServidorIntermediario server(nullptr, port, ipAndPortFS.second, ipAndPortDB.second, ipAndPortFS.first, ipAndPortDB.first);

    if (server.startServer()) {
        qDebug() << "[SI] Server started successfully.";
        qDebug() << "[SI]" << QSslSocket::sslLibraryBuildVersionString();
    } else {
        qDebug() << "[SI] Failed to start server.";
        return 1;
    }

    return a.exec();
}
