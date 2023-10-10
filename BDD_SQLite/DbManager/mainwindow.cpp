#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->db = new DbManager();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db;
}

void MainWindow::on_BuscarDatos_clicked()
{    
    QString datos = db->consultarConstancia(3);
    ui->textBrowser->setText(datos);
}
