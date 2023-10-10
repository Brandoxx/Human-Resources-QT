#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardItemModel>


class DbManager {
public:
    const int CONSTANCIALABORAL = 1;
    const int CONSTANCIASALARIAL = 2;

    /**
     * @brief Construct a new Db Manager object
     */
    DbManager();

//    int agregarDatosPersonales(QString usuario, QString nombre,
//                               QString apellido, QString correo,
//                               QString telefono, QString cedula);
//    int agregarDatosLaborales(QString usuario, QString puesto,
//                              int salarioBruto, int salarioNeto,
//                              QString fechaInicial, QString fechaFinal = "");
//    int agregarConstancia(QString usuario, QString fechaSolicitud,
//                          int salarioBruto, int salarioNeto,
//                          int tipoConstancia);

    /**
     * @brief Consulta los datos personales de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con los datos encontrados en la tabla
     */
    QString consultarDatosPersonales(QString usuario);

    /**
     * @brief Consulta los datos laborales de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con los datos encontrados en la tabla
     */
    QString consultarDatosLaborales(QString usuario);

    /**
     * @brief Consulta los datos de una constancia solicitada
     * @param idSolicitud Id de la solicitud a buscar en la base de datos
     * @return QString String con los datos encontrados en la tabla
     */
    QString consultarConstancia(int idSolicitud);

private:
    QSqlDatabase db; // Variable utilizada para la conexion con la base de datos
};

#endif // DBMANAGER_H
