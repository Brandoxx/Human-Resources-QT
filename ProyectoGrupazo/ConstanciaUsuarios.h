#ifndef CONSTANCIAUSUARIOS_H
#define CONSTANCIAUSUARIOS_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include "UsuarioSolicitudes.h"
#include "UsuarioConsultas.h"
#include "EstadoSolicitudesHechas.h"
#include "Client.h"

namespace Ui {
class ConstanciaUsuarios;
}

class ConstanciaUsuarios : public QWidget
{
    Q_OBJECT

public:
    explicit ConstanciaUsuarios(QWidget *parent = nullptr, UsuarioSolicitudes *UsuarioSolicitudesParent = nullptr, QString constancia = "", Client& client = Client::getInstance());
    explicit ConstanciaUsuarios(QWidget *parent = nullptr, UsuarioConsultas *UsuarioConsultasParent = nullptr, QString constancia = "", Client& client = Client::getInstance());
    explicit ConstanciaUsuarios(QWidget *parent = nullptr, EstadoSolicitudesHechas *EstadoSolicitudesHechasParent = nullptr, QString constancia = "", Client& client = Client::getInstance());
    ~ConstanciaUsuarios();


private slots:
    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

    void on_logout_clicked();

private:
    Ui::ConstanciaUsuarios *ui;
    QWidget *parentWindow;
    UsuarioSolicitudes *UsuarioSolicitudesParentWindow;
    UsuarioConsultas *UsuarioConsultasParentWindow;
    EstadoSolicitudesHechas *EstadoSolicitudesHechasParentWindow;
    Client& client; // Agrega una referencia a Client
    void setupTextFormat(QString information);
};

#endif // CONSTANCIAUSUARIOS_H

