#include <QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    if (server.startServer()) {
        qDebug() << "Server started successfully.";
    } else {
        qDebug() << "Failed to start server.";
        return 1;
    }

    return a.exec();
}
