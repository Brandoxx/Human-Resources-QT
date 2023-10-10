#ifndef ADMINISTRADORMENU_H
#define ADMINISTRADORMENU_H

#include <QWidget>
#include <QMessageBox>

#include "Client.h"

namespace Ui {
class AdministradorMenu;
}

class AdministradorMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AdministradorMenu(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "");

    ~AdministradorMenu();

private slots:
    void on_logout_clicked();
    void on_agregarButton_clicked();
    void on_confeccionarButton_clicked();
    void on_actualizarButton_clicked();
    void on_borrarButton_clicked();

private:
    Ui::AdministradorMenu *ui;
    Client& client; // Agregar una referencia a Client
    QString username;
};

#endif // ADMINISTRADORMENU_H
