#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <QCryptographicHash>
#include "Client.h"

enum permisos {
    USUARIO_EMPLEADO = 1,
    USUARIO_SUPERVISOR,
    USUARIO_RRHH,
    ADMINISTRADOR_USUARIOS,
    ADMINISTRADOR_CONFIG,
    USUARIO_DEBUG,
    USUARIO_AUDITOR,
};

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString SI_IP = "127.0.0.1", qint16 SI_Port = 1678);
    ~Login();

public slots:
    void onShowPasswordToggled(bool checked);

signals:
    void access_rights_read(qint8 accessRights);

private slots:
    void on_loginButton_clicked();
    void connection_to_server_failed(QString error);
    void connection_to_server_success();
    void handle_auth_message(const Message& message);

    void cleanAfterDisconnect();

private:
    Ui::Login *ui;
    Client& client; // Referencia al singleton del cliente
    QString SI_IP;
    qint16 SI_Port;

    void show_next_screen(int permisos, QString username);
    void start_connection_with_server(QString ipAddress, qint16 port);


};
#endif // LOGIN_H
