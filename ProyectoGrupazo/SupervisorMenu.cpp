#include "SupervisorMenu.h"
#include "ui_SupervisorMenu.h"
#include "Login.h"
#include "UsuarioConsultas.h"
#include "UsuarioSolicitudes.h"
#include "RevisionSolicitudes.h"
#include "AnotacionExpediente.h"

SupervisorMenu::SupervisorMenu(QWidget *parent, Client& client, QString username) :
    QWidget(parent),
    ui(new Ui::SupervisorMenu),
    client(client),
    username(username)
{
    ui->setupUi(this);
}

SupervisorMenu::~SupervisorMenu()
{
    delete ui;
}

void SupervisorMenu::on_consultaButton_clicked()
{
    this->hide();
    UsuarioConsultas *ventanaConsultas = new UsuarioConsultas(this, this->client.getInstance(), this->username);
    ventanaConsultas->show();
}

void SupervisorMenu::on_solicitudButton_clicked()
{
    this->hide();
    UsuarioSolicitudes *ventanaSolicitudes = new UsuarioSolicitudes(this, this->client.getInstance(), this->username);
    ventanaSolicitudes->show();
}

void SupervisorMenu::on_logout_clicked()
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

void SupervisorMenu::on_revisionButton_clicked()
{
    this->hide();
    RevisionSolicitudes *revisionSolicitudes = new RevisionSolicitudes(this, client.getInstance(), this->username, Message::PENDIENTE);
    revisionSolicitudes->show();
}


void SupervisorMenu::on_expedienteButton_clicked()
{
    this->hide();
    AnotacionExpediente *anotacionExpediente = new AnotacionExpediente(this, client.getInstance(), this->username);
    anotacionExpediente->show();
}

