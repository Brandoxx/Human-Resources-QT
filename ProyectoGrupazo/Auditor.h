#ifndef AUDITOR_H
#define AUDITOR_H

#include <QWidget>
#include <QMessageBox>

#include "Client.h"

namespace Ui {
class Auditor;
}

class Auditor : public QDialog
{
    Q_OBJECT

public:
    explicit Auditor(QWidget *parent = nullptr, Client& client = Client::getInstance());
    ~Auditor();

private slots:
    void on_mostrarButton_clicked();

    void on_logout_clicked();

private:
    Ui::Auditor *ui;
    Client& client; // Agregar una referencia a Client
};

#endif // AUDITOR_H
