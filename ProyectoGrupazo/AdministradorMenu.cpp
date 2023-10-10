#include "AdministradorMenu.h"
#include "ui_AdministradorMenu.h"

#include "Login.h"
#include "AgregarUsuarios.h"
#include "ActualizarUsuarios.h"
#include "RevisionSolicitudes.h"
#include "BorrarUsuarios.h"

AdministradorMenu::AdministradorMenu(QWidget *parent, Client& client, QString username) :
    QWidget(parent),
    ui(new Ui::AdministradorMenu),
    client(client),
    username(username)
{
    ui->setupUi(this);
}

AdministradorMenu::~AdministradorMenu()
{
    delete ui;
}

void AdministradorMenu::on_logout_clicked()
{
    QMessageBox::StandardButton confirmarLogout;
    confirmarLogout = QMessageBox::question(this, "Cerrar sesión ", "¿Esta seguro que desea cerrar sesión?", QMessageBox::Yes | QMessageBox::No);
    if(confirmarLogout == QMessageBox::Yes)
    {
        Client& client = Client::getInstance();  // Obtener la instancia del singleton del cliente
        close();
        client.disconnectFromServer();
        QEventLoop closed;
        connect(&client, &Client::disconnected, &closed, &QEventLoop::quit);
        closed.exec();
        this->deleteLater();
    }
}


void AdministradorMenu::on_agregarButton_clicked()
{
    this->hide();
    AgregarUsuarios *ventanaAgregarUsuarios = new AgregarUsuarios(this, this);
    ventanaAgregarUsuarios->show();
}


void AdministradorMenu::on_actualizarButton_clicked()
{
    this->hide();
    ActualizarUsuarios *ventanaActualizarUsuarios = new ActualizarUsuarios(this, this);
    ventanaActualizarUsuarios->show();
}



void AdministradorMenu::on_confeccionarButton_clicked()
{
    this->hide();
    RevisionSolicitudes *revisionSolicitudes = new RevisionSolicitudes(this, client.getInstance(), this->username, Message::APROBADASUPERVISOR);
    revisionSolicitudes->show();
}


void AdministradorMenu::on_borrarButton_clicked()
{
    this->hide();
    BorrarUsuarios* ventanaListaUsuarios = new BorrarUsuarios(this, this);
    ventanaListaUsuarios->show();
}

