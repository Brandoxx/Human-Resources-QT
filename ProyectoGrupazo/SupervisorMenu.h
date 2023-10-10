#ifndef SUPERVISORMENU_H
#define SUPERVISORMENU_H

#include <QWidget>
#include <QMessageBox>

#include "Client.h"

namespace Ui {
class SupervisorMenu;
}

class SupervisorMenu : public QWidget
{
    Q_OBJECT

public:
    explicit SupervisorMenu(QWidget *parent = nullptr,Client& client = Client::getInstance(), QString username = "");
    ~SupervisorMenu();

private slots:
    void on_logout_clicked();

    void on_consultaButton_clicked();

    void on_solicitudButton_clicked();

    void on_revisionButton_clicked();

    void on_expedienteButton_clicked();

private:
    Ui::SupervisorMenu *ui;
    Client& client; // Agregar una referencia a Client
    QString username;
};

#endif // SUPERVISORMENU_H
