#ifndef ACTUALIZARDATOSUSUARIO_H
#define ACTUALIZARDATOSUSUARIO_H

#include <QWidget>
#include "ActualizarUsuarios.h"
#include "Client.h"

namespace Ui {
class ActualizarDatosUsuario;
}

class ActualizarDatosUsuario : public QWidget
{
    Q_OBJECT

public:
    explicit ActualizarDatosUsuario(QWidget *parent = nullptr,
                                    Client& client = Client::getInstance(),
                                    QString username = "");
    ~ActualizarDatosUsuario();

private slots:
    void on_addUserButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();

    void consultarDatosDeUsuario(QString username);

    void receiveResponseFromServer(const Message& message);
signals:
    void read_information(qint8 information);
private:
    Ui::ActualizarDatosUsuario *ui;
    QWidget* parentWindow;
    ActualizarUsuarios* ActualizarUsuariosParentWindow;
    Client& client; // Agregar una referencia a Client
    QString username;
    QByteArray informacion;

    void fillInputInfo();
};

#endif // ACTUALIZARDATOSUSUARIO_H
