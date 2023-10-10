#ifndef ACTUALIZARUSUARIOS_H
#define ACTUALIZARUSUARIOS_H

#include <QWidget>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>

#include "AdministradorMenu.h"
#include "Client.h"

namespace Ui {
class ActualizarUsuarios;
}

class ActualizarUsuarios : public QWidget
{
    Q_OBJECT

public:
    explicit ActualizarUsuarios(QWidget *parent = nullptr, AdministradorMenu *AdministradorMenuParentWindow = nullptr, Client& client = Client::getInstance());
    ~ActualizarUsuarios();

private slots:
    void on_updateUserButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();

    void editarUsuario(QString username);

    void procesarUsuarios(const Message& message);

signals:
    void usuariosProcesados(qint16 error);

private:
    Ui::ActualizarUsuarios *ui;
    QWidget *parentWindow;
    AdministradorMenu *AdministradorMenuParentWindow;
    Client& client; // Referencia al singleton del cliente
    QListWidget* listaWidget;
    QList<QString> usuarios;

    void mostrarWidgets();
};

#endif // ACTUALIZARUSUARIOS_H
