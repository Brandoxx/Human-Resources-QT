#ifndef USUARIOSOLICITUDES_H
#define USUARIOSOLICITUDES_H

#include <QWidget>
#include <QMessageBox>

#include "Client.h"

namespace Ui {
class UsuarioSolicitudes;
}

class UsuarioSolicitudes : public QWidget
{
    Q_OBJECT

public:
    explicit UsuarioSolicitudes(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "");
    ~UsuarioSolicitudes();

signals:
    void response_read(qint8 response);

private slots:
    void on_cancelButton_clicked();
    void on_logout_clicked();

    void on_acceptButton_clicked();
    void receiveResponseFromServer(const Message& message);

private:
    Ui::UsuarioSolicitudes *ui;
    QWidget* parentWindow;
    Client& client; // Referencia al singleton del cliente
    QString username;
    QByteArray informacion;

};

#endif // USUARIOSOLICITUDES_H
