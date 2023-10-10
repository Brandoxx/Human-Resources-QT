#include "Login.h"
#include "qicon.h"
#include "Client.h"
#include "Logger.hpp"

#include <QApplication>

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
    QApplication a(argc, argv);
    
    Logger::getInstance();
    qInstallMessageHandler(myMessageHandler);

    QPair<QString,quint16> ipAndPortDB = readIPAndPortFromFile("--SI Server--");

    a.setWindowIcon(QIcon(":/logo/Images/Logo4.png"));
    Login w(nullptr, Client::getInstance(), ipAndPortDB.first, ipAndPortDB.second);
    w.show();
    return a.exec();
}
