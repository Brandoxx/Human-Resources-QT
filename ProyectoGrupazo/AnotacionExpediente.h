#ifndef ANOTACIONEXPEDIENTE_H
#define ANOTACIONEXPEDIENTE_H

#include <QWidget>
#include "Client.h"

#include <QListWidget>

struct UsuarioExpediente {
    QString nombre;
    QString anotacion;
};

namespace Ui {
class AnotacionExpediente;
}

class AnotacionExpediente : public QWidget
{
    Q_OBJECT

public:
    explicit AnotacionExpediente(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "");
    ~AnotacionExpediente();
    void agregarUsuario(const QString& nombre, const QString& anotacion);

private slots:
    void on_logout_clicked();

    void on_cancelButton_clicked();

    void on_acceptButton_clicked();

    void messageReceived(const Message &message);

signals: 
    void readAllUsers(qint8 error);
    void receivedAnotacion(qint8 error);
private:
    Ui::AnotacionExpediente *ui;
    QListWidget* listaWidget;
    QWidget *parentWindow;
    Client& client; // Se agrega una referencia Client
    QString username;
    QList<UsuarioExpediente> usuarios;

    void requestUsers();
    void fillUsers(QString users);
    void sendAnotacion(QString anotacion, QString nombre);
};

#endif // ANOTACIONEXPEDIENTE_H
