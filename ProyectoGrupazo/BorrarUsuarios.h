#ifndef BORRARUSUARIOS_H
#define BORRARUSUARIOS_H

#include <QWidget>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>

#include "AdministradorMenu.h"
#include "Client.h"

namespace Ui {
class BorrarUsuarios;
}

class BorrarUsuarios : public QWidget
{
    Q_OBJECT

public:
    explicit BorrarUsuarios(QWidget *parent = nullptr, AdministradorMenu *AdministradorMenuParentWindow = nullptr, Client& client = Client::getInstance());
    ~BorrarUsuarios();

signals:
    void usuariosProcesados(qint16 error);
    void response_readFS(qint8 response);
    void response_readDB(qint8 response);

private slots:
    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();

    void receiveResponseFromServer(const Message& message);

    void eliminarUsuario(QString username);

    void procesarUsuarios(const Message& message);

private:
    Ui::BorrarUsuarios *ui;
    QWidget *parentWindow;
    AdministradorMenu *AdministradorMenuParentWindow;
    Client& client; // Referencia al singleton del cliente
    QListWidget* listaWidget;
    QList<QString> usuarios;

    void mostrarWidgets();
};

#endif // BORRARUSUARIOS_H
