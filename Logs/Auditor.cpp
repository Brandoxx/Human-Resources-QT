#include "Auditor.h"
#include "ui_Auditor.h"
#include "ShowLogs.h"

Auditor::Auditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Auditor)
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
    ShowLogs *showLogs = new ShowLogs(nullptr, this, nombreArchivo);
    showLogs->show();
}



