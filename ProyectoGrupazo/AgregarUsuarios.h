#ifndef AGREGARUSUARIOS_H
#define AGREGARUSUARIOS_H

#include <QWidget>

#include "AdministradorMenu.h"
#include "Client.h"


namespace Ui {
class AgregarUsuarios;
}

class AgregarUsuarios : public QWidget
{
    Q_OBJECT

public:
    explicit AgregarUsuarios(QWidget *parent = nullptr, AdministradorMenu *AdministradorMenuParentWindow = nullptr, Client& client = Client::getInstance());
    ~AgregarUsuarios();

signals:
    void response_readFS(qint8 response);
    void response_readDB(qint8 response);

private slots:

    void on_cancelButton_clicked();

    void on_logout_clicked();

    void on_addUserButton_clicked();

    void receiveResponseFromServer(const Message& message);

private:
    Ui::AgregarUsuarios *ui;
    QWidget *parentWindow;
    AdministradorMenu *AdministradorMenuParentWindow;
    Client& client; // Referencia al singleton del cliente

    void sendUserToFSServer(QString username, QByteArray password, qint16 role);
    void sendUserToDBServer(QString username, QString name, QString lastname,
                            QString email, QString phone, QString personalId,
                            QString job, QString initDate, QString grossPay,
                            QString netPay, QString Area);
    void setTabOrder();
    void clearInputFields();
};

#endif // AGREGARUSUARIOS_H
