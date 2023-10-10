#include "DbManager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QLocale>
#include <iostream>


DbManager::DbManager()
{
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("BDDGrupazo.db");
    if (!db.open()){
        qDebug() << "[DB] Fallo en conexi?n";
    } else {
        qDebug() << "[DB] Conexion exitosa";
    }
    db.close();
}

//int DbManager::agregarDatosPersonales(QString usuario, QString nombre,
//                           QString apellido, QString correo,
//                           QString telefono, QString cedula) {

//}
//int DbManager::agregarDatosLaborales(QString usuario, QString puesto,
//                          int salarioBruto, int salarioNeto,
//                          QString fechaInicial, QString fechaFinal = "") {

//}

//int DbManager::agregarConstancia(QString usuario, QString fechaSolicitud,
//                      int salarioBruto, int salarioNeto,
//                      int tipoConstancia) {

//}

QString DbManager::consultarDatosPersonales(QString usuario) {
    db.open();
    QSqlQuery q;
    QString data = "";

    q.prepare("SELECT * FROM DatosPersonales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();

    if (q.next()) {
        QString usuario = q.value(0).toString();
        QString nombre = q.value(1).toString();
        QString apellido = q.value(2).toString();
        QString correo = q.value(3).toString();
        QString telefono = q.value(4).toString();
        QString cedula = q.value(5).toString();

        data += "Usuario: " + usuario + "\nNombre: " + nombre;
        data += "\nApellido: " + apellido + "\nCorreo: " + correo;
        data += "\nNumero de telefono: " + telefono;
        data += "\nCedula: " + cedula;
    } else {
        data += "No existe el usuario digitado";
    }
    q.clear();
    db.close();
    return data;
}

QString DbManager::consultarDatosLaborales(QString usuario) {
    db.open();
    QSqlQuery q;
    QString data = "";

    q.prepare("SELECT * FROM DatosLaborales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();

    if (q.next()) {
        QString usuario = q.value(0).toString();
        QString puesto = q.value(1).toString();
        QString fechaInicio = q.value(2).toString();
        QString fechaFin = q.value(3).toString();
        double salarioBruto = q.value(4).toDouble();
        QString stringBruto = QString::number(salarioBruto, 'f', 2);
        double salarioNeto = q.value(5).toDouble();
        QString stringNeto = QString::number(salarioNeto, 'f', 2);
        QString vacaciones = q.value(6).toString();

        data += "Usuario: " + usuario + "\nPuesto: " + puesto;
        data += "\nFecha de Inicio de Labores: " + fechaInicio ;
        data += "\nFecha de Fin de Labores: ";
        if (fechaFin != "") {
            data += fechaFin;
        } else {
            data += "-";
        }
        data += "\nSalario Bruto: " + stringBruto;
        data += "\nSalario Neto: " + stringNeto;
        data += "\nVacaciones: " + vacaciones;
    } else {
        data += "No existe el usuario digitado";
    }
    q.clear();
    db.close();
    return data;
}

QString DbManager::consultarConstancia(int idSolicitud) {
    db.open();
    QSqlQuery q;
    QString data = "";

    q.prepare("SELECT * FROM Constancias WHERE ID_Solicitud = (:idSolicitud)");
    q.bindValue(":idSolicitud", idSolicitud);
    q.exec();

    if (q.next()) {
        QString idSolicitud = q.value(0).toString();
        QString usuario = q.value(1).toString();
        QString fechaSolicitud = q.value(2).toString();
        QString fechaAprobacion = q.value(3).toString();
        double salarioBruto = q.value(4).toDouble();
        QString stringBruto = QString::number(salarioBruto, 'f', 2);
        double salarioNeto = q.value(5).toDouble();
        QString stringNeto = QString::number(salarioNeto, 'f', 2);
        QString tipoConstancia = q.value(6).toString();

        data += "ID Solicitud: " + idSolicitud + "\nUsuario: " + usuario;
        data += "\nFecha de Solicitud: " + fechaSolicitud;
        data += "\nFecha de Aprobacion: ";
        if (fechaAprobacion == "") {
            data += "No Aprobado";
        } else {
            data += fechaAprobacion;
        }
        data += "\nSalario Bruto: " + stringBruto;
        data += "\nSalario Neto: " + stringNeto;
        data += "\nTipo de Constancia: ";
        if (tipoConstancia == "1") {
            data += "Laboral";
        } else {
            data += "Salarial";
        }
    } else {
        data += "No existe la constancia buscada";
    }
    q.clear();
    db.close();
    return data;
}
