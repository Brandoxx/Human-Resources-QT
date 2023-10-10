#include "Auditor.h"
#include "ui_Auditor.h"
#include "ShowLogs.h"

Auditor::Auditor(QWidget *parent, Client& client) :
    QDialog(parent),
    ui(new Ui::Auditor),
    client(client)
{
    ui->setupUi(this);
}

Auditor::~Auditor()
{
    delete ui;
}

void Auditor::on_mostrarButton_clicked()
{
    this->hide();
    QString nombreArchivo = "";
    if(this->ui->comboBox->currentIndex() == 0){
        nombreArchivo = "../UILog.txt";
    } else if(this->ui->comboBox->currentIndex() == 1){
        nombreArchivo = "../FSLog.txt";
    } else if(this->ui->comboBox->currentIndex() == 2){
        nombreArchivo = "../DBLog.txt";
    } else {
        nombreArchivo = "../SILog.txt";
    }
    ShowLogs *showLogs = new ShowLogs(nullptr, this, Client::getInstance(), nombreArchivo);
    showLogs->show();
}


void Auditor::on_logout_clicked()
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

