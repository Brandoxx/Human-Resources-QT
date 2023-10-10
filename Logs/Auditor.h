#ifndef AUDITOR_H
#define AUDITOR_H

#include <QWidget>
#include <QMessageBox>


namespace Ui {
class Auditor;
}

class Auditor : public QDialog
{
    Q_OBJECT

public:
    explicit Auditor(QWidget *parent = nullptr);
    ~Auditor();

private slots:
    void on_mostrarButton_clicked();


private:
    Ui::Auditor *ui;

};

#endif // AUDITOR_H
