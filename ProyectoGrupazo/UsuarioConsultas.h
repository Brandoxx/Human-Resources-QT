#ifndef USUARIOCONSULTAS_H
#define USUARIOCONSULTAS_H

#include <QWidget>
#include <QMessageBox>
#include "Client.h"

namespace Ui {
class UsuarioConsultas;
}

class UsuarioConsultas : public QWidget
{
    Q_OBJECT

public:
    explicit UsuarioConsultas(QWidget *parent = nullptr, Client& client = Client::getInstance(), QString username = "");
    ~UsuarioConsultas();
    // QString informeSaldoVacaciones();
    QString informeSalarioBase(const QString& nombre, const QString& apellido, const QString& salarioBase);
    QString informeSaldoVacaciones(const QString& nombre, const QString& apellido, const QString& saldoVacaciones);

private slots:
    void on_cancelButton_clicked();
    void on_logout_clicked();
    void on_acceptButton_clicked();
    void handle_message(const Message& message);
signals:
    void read_information(qint8 information);

private:
    Ui::UsuarioConsultas *ui;
    QWidget* parentWindow;
    Client& client; // Referencia al singleton del cliente
    QString username;
    QByteArray informacion;
};

#endif // USUARIOCONSULTAS_H
