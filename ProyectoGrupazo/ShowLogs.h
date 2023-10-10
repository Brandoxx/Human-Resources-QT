#ifndef SHOWLOGS_H
#define SHOWLOGS_H

#include <QWidget>

#include "Client.h"
#include "Auditor.h"

namespace Ui {
class ShowLogs;
}

class ShowLogs : public QWidget
{
    Q_OBJECT

public:
    explicit ShowLogs(QWidget *parent = nullptr, Auditor *AuditorMenuParentWindow = nullptr, Client& client = Client::getInstance(), QString nombreArchivo = "");
    ~ShowLogs();

    void setupTextFormat();

private slots:
    void on_logout_clicked();

    void on_cancelButton_clicked();

    void on_acceptButton_clicked();

private:
    Ui::ShowLogs *ui;
    QWidget *parentWindow;
    Auditor *AuditorMenuParentWindow;
    Client& client; // Referencia al singleton del cliente
    QString archivo;
};

#endif // SHOWLOGS_H
