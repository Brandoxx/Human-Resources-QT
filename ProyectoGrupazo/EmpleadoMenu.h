#ifndef EMPLEADOMENU_H
#define EMPLEADOMENU_H

#include <QWidget>
#include <QMessageBox>

#include "Client.h"

namespace Ui {
class EmpleadoMenu;
}

class EmpleadoMenu : public QWidget
{
    Q_OBJECT

public:
    explicit EmpleadoMenu(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "");
    ~EmpleadoMenu();

private slots:
    void on_consultaButton_clicked();

    void on_solicitudButton_clicked();

    void on_logout_clicked();

private:
    Ui::EmpleadoMenu *ui;
    Client& client; // Agregar una referencia a Client
    QString username;
};

#endif // EMPLEADOMENU_H
