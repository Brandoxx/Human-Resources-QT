#include "EmpleadoMenu.h"
#include "ui_EmpleadoMenu.h"
#include "UsuarioConsultas.h"
#include "UsuarioSolicitudes.h"
#include "Login.h"

EmpleadoMenu::EmpleadoMenu(QWidget *parent, Client& client, QString username) :
    QWidget(parent),
    ui(new Ui::EmpleadoMenu),
    client(client),
    username(username)
{
    ui->setupUi(this);
}

EmpleadoMenu::~EmpleadoMenu()
{
    delete ui;
}

void EmpleadoMenu::on_consultaButton_clicked()
{
    this->hide();
    UsuarioConsultas *ventanaConsultas = new UsuarioConsultas(this, this->client.getInstance(), this->username);
    ventanaConsultas->show();
}


void EmpleadoMenu::on_solicitudButton_clicked()
{
    this->hide();
    UsuarioSolicitudes *ventanaSolicitudes = new UsuarioSolicitudes(this, this->client.getInstance(), this->username);
    ventanaSolicitudes->show();
}


void EmpleadoMenu::on_logout_clicked()
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


