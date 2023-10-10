#include "DbManager.h"
#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QLocale>
#include <iostream>
#include <QDir>
#include <iostream>
#include <QDate>


DbManager::DbManager()
{
    QString currentDirPath = QDir::currentPath();
    QString parentDirPath = QDir(currentDirPath).filePath("..");
    QString databaseFilePath = QDir(parentDirPath).filePath("BDDGrupazo");
    qDebug() << databaseFilePath;
    this->db =
        QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databaseFilePath);

    if (!db.open()){
        qDebug() << "[DB] Fallo en conexi?n";
    } else {
        qDebug() << "[DB] Conexion exitosa";
    }
}

DbManager::~DbManager() {
    if (db.isOpen()) {
        db.close();
    }
}

int DbManager::agregarDatosPersonales(QString usuario, QString nombre,
                           QString apellido, QString correo,
                           QString telefono, QString cedula) {
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("INSERT INTO DatosPersonales (Usuario,Nombre,Apellido,"
                "Correo,Num_teléfono,Num_cedula) VALUES (:usuario,"
                ":nombre,:apellido,:correo,:telefono,:cedula)"
    );
    q.bindValue(":usuario", usuario);
    q.bindValue(":nombre", nombre);
    q.bindValue(":apellido", apellido);
    q.bindValue(":correo", correo);
    q.bindValue(":telefono", telefono);
    q.bindValue(":cedula", cedula);
    if(!q.exec()) {
        qDebug() << "[DB] Error agregando datos personales: " << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}

int DbManager::agregarDatosLaborales(QString usuario, QString puesto,
                                     QString fechaInicial, int salarioBruto,
                                     int salarioNeto, int vacaciones, QString area) {
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("INSERT INTO DatosLaborales (Usuario,Puesto,Fecha_Inicial,"
            "Fecha_Final,Salario_Bruto,Salario_Neto,Vacaciones_Disponibles,Anotaciones,Area)"
            " VALUES(:usuario,:puesto,:fechaInicial,NULL,:salarioBruto,"
            ":salarioNeto,:vacaciones,NULL,:area)"
    );
    q.bindValue(":usuario", usuario);
    q.bindValue(":puesto", puesto);
    q.bindValue(":fechaInicial", fechaInicial);
    q.bindValue(":salarioBruto", salarioBruto);
    q.bindValue(":salarioNeto", salarioNeto);
    q.bindValue(":vacaciones", vacaciones);
    q.bindValue(":area", area);
    if(!q.exec()) {
        qDebug() << "[DB] Error agregando datos laborales: " << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}

int DbManager::agregarConstancia(QString usuario, QString fechaSolicitud,
                                 int salarioBruto, int salarioNeto,
                                 int tipoConstancia) {
    QString area = this->consultarArea(usuario);
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("INSERT INTO Constancias (Usuario, Fecha_Solicitud, Fecha_Aprobacion, Salario_Bruto, Salario_Neto, Tipo_Constancia,Estado, Area) "
              "VALUES (:usuario, :fechaSolicitud, NULL, :salarioBruto, :salarioNeto, :tipoConstancia, 0, :area )");
    q.bindValue(":usuario", usuario);
    q.bindValue(":fechaSolicitud", fechaSolicitud);
    q.bindValue(":salarioBruto", salarioBruto);
    q.bindValue(":salarioNeto", salarioNeto);
    q.bindValue(":tipoConstancia", tipoConstancia);
    q.bindValue(":area", area);

    if (!q.exec()) {
        qDebug() << "[DB] Error agregando constancia: " << q.lastError();
        error = EXIT_FAILURE;
    }

    q.clear();
    this->actualizarSalarios(usuario);
    return error;
}

int DbManager::agregarSolicitudVacaciones(QString usuario, QString motivo,
                                                QString fechaSolicitud,
                                                QString fechaInicioVacaciones,
                                                QString fechaFinalVacaciones) {
    QString area = this->consultarArea(usuario);
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("INSERT INTO RegistroVacaciones (Usuario,Motivo,Fecha_Solicitud,"
                "FechaInicioVacaciones,FechaFinalVacaciones,Estado,Area)"
                "VALUES (:usuario,:motivo,:fechaSolicitud,"
                ":fechaInicioVacaciones,:fechaFinalVacaciones,0, :area )"
    );
    q.bindValue(":usuario", usuario);
    q.bindValue(":motivo", motivo);
    q.bindValue(":fechaSolicitud", fechaSolicitud);
    q.bindValue(":fechaInicioVacaciones", fechaInicioVacaciones);
    q.bindValue(":fechaFinalVacaciones", fechaFinalVacaciones);
    q.bindValue(":area", area);
    if(!q.exec()) {
        qDebug() << "[DB] Error agregando registro de vacaciones: "
                 << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}

QString DbManager::consultarDatosPersonales(QString usuario) {
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
    return data;
}

QString DbManager::consultarDatosLaborales(QString usuario) {
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
    return data;
}

QByteArray DbManager::consultarConstancia(int idSolicitud) {
    QSqlQuery q;
    QString data = "";
    QByteArray datagrama(131, '\0');

    q.prepare("SELECT * FROM Constancias WHERE ID_Solicitud = (:idSolicitud)");
    q.bindValue(":idSolicitud", idSolicitud);
    q.exec();
    QString usuario;

    if (q.next()) {
        QString idSolicitud = q.value(0).toString();
        usuario = q.value(1).toString();
        QString fechaSolicitud = q.value(2).toString();
        QString fechaAprobacion = q.value(3).toString();
        double salarioBruto = q.value(4).toDouble();
        QString stringBruto = QString::number(salarioBruto, 'f', 2);
        double salarioNeto = q.value(5).toDouble();
        QString stringNeto = QString::number(salarioNeto, 'f', 2);
        QString tipoConstancia = q.value(6).toString();
        datagrama.replace(0, qMin(usuario.length(), 10), usuario.toUtf8());
        datagrama.replace(51, 10, stringBruto.toUtf8());
        datagrama.replace(61, 10, stringNeto.toUtf8());
        datagrama.replace(81, 10, fechaSolicitud.toUtf8());

        data += "ID Solicitud: " + idSolicitud + "\nUsuario: " + usuario;
        data += "\nFecha de Solicitud: " + fechaSolicitud;
        data += "\nFecha de Aprobacion: ";
        if (fechaAprobacion == "") {
            data += "No Aprobado";
        } else {
            data += fechaAprobacion;
            datagrama.replace(91, 8, fechaAprobacion.toUtf8());
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
    q.prepare("SELECT * FROM DatosPersonales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString nombre = q.value(1).toString();
            QString apellido = q.value(2).toString();
            QString cedula = q.value(5).toString();
            datagrama.replace(10, qMin(nombre.length(), 15), nombre.toUtf8());
            datagrama.replace(25, qMin(apellido.length(), 15), apellido.toUtf8());
            datagrama.replace(40, qMin(cedula.length(), 11), cedula.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    q.prepare("SELECT * FROM DatosLaborales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString fechaInicioLabores = q.value(2).toString();
            QString puesto = q.value(1).toString();
            datagrama.replace(71, 10, fechaInicioLabores.toUtf8());
            datagrama.replace(101, 30, puesto.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    return datagrama;
}

QByteArray DbManager::consultarSolicitudVacaciones(int idSolicitud){
    QSqlQuery q;
    QByteArray datagrama(70, '\0');

    q.prepare("SELECT * FROM RegistroVacaciones WHERE ID_Solicitud = (:idSolicitud)");
    q.bindValue(":idSolicitud", idSolicitud);
    q.exec();

    QString usuario;
    if (q.next()) {
        QString idSolicitud = q.value(6).toString();
        usuario = q.value(0).toString();
        QString fechaSolicitud = q.value(2).toString();
        QString fechaInicio = q.value(3).toString();
        QString fechaFin = q.value(4).toString();
        datagrama.replace(0, qMin(usuario.length(), 10), usuario.toUtf8());
        datagrama.replace(40, 10,fechaSolicitud.toUtf8());
        datagrama.replace(50, 10,fechaInicio.toUtf8());
        datagrama.replace(60, 10,fechaFin.toUtf8());
    } else {
        qDebug() << "[DB] Error, no existe la solicitud buscada: "
                 << q.lastError();
    }
    q.clear();
    q.prepare("SELECT * FROM DatosPersonales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString nombre = q.value(1).toString();
            QString apellido = q.value(2).toString();
            datagrama.replace(10, qMin(nombre.length(), 15), nombre.toUtf8());
            datagrama.replace(25, qMin(apellido.length(), 15), apellido.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    return datagrama;
}

QByteArray DbManager::consultarSaldoVacaciones(QString usuario) {
    QSqlQuery q;
    QByteArray datagrama(33, '\0');

    // nombre, apellido, vacaciones
    q.prepare("SELECT personal.Nombre, personal.Apellido,"
              " laboral.Vacaciones_Disponibles"
              " FROM DatosPersonales personal"
              " INNER JOIN DatosLaborales laboral"
              " ON personal.Usuario = (:usuario)"
              " AND laboral.Usuario = (:usuario)"
              " AND laboral.Fecha_Final is null");
    q.bindValue(":usuario", usuario);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString nombre = q.value(0).toString();
            QString apellido = q.value(1).toString();
            QString vacaciones = q.value(2).toString();

            datagrama.replace(0, qMin(nombre.length(), 15), nombre.toUtf8());
            datagrama.replace(15, qMin(apellido.length(), 15), apellido.toUtf8());
            datagrama.replace(30, qMin(vacaciones.length(), 3), vacaciones.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    return datagrama;
}

QByteArray DbManager::consultarSalarioBase(QString usuario) {
    QSqlQuery q;
    QByteArray datagrama(50, '\0');

    // nombre, apellido, vacaciones
    q.prepare("SELECT personal.Nombre, personal.Apellido,"
              " laboral.Salario_Neto, laboral.Fecha_Inicial"
              " FROM DatosPersonales personal"
              " INNER JOIN DatosLaborales laboral"
              " ON personal.Usuario = (:usuario)"
              " AND laboral.Usuario = (:usuario)"
              " AND laboral.Fecha_Final is null");
    q.bindValue(":usuario", usuario);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString nombre = q.value(0).toString();
            QString apellido = q.value(1).toString();
            QString salarioNeto = q.value(2).toString();
            QString fechaIngreso = q.value(3).toString();
            datagrama.replace(0, qMin(usuario.length(), 10), usuario.toUtf8());
            datagrama.replace(10, qMin(nombre.length(), 15), nombre.toUtf8());
            datagrama.replace(25, qMin(apellido.length(), 15), apellido.toUtf8());
            datagrama.replace(40, qMin(salarioNeto.length(), 10), salarioNeto.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    return datagrama;
}

QString DbManager::consultarEstadoDeSolicitudes(QString usuario) {
    QSqlQuery q;
    QString data = "";

    // nombre, apellido, vacaciones
    q.prepare("SELECT Tipo_Constancia,Fecha_Solicitud,Fecha_Aprobacion,Estado,ID_Solicitud"
              " FROM Constancias WHERE Usuario = (:usuario)"
    );
    q.bindValue(":usuario", usuario);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString tipoConstancia = "";
            int numConstancia = 0;
            QString fechaSolicitud = "";
            QString fechaAprobacion = "";
            estadoSolicitud estado = estadoSolicitud::PENDIENTE;
            quint64 idSolicitud = 0;
            do {
                tipoConstancia = q.value(0).toString();
                numConstancia = tipoConstancia.toInt();
                fechaSolicitud = q.value(1).toString();
                fechaAprobacion = q.value(2).toString();
                estado = estadoSolicitud(q.value(3).toInt());
                idSolicitud = q.value(4).toLongLong();

               if (numConstancia == CONSTANCIALABORAL) {
                   data += "Constancia Laboral\t";
               } else {
                   data += "Constancia Salarial\t";
               }
               data += fechaSolicitud + "\t";
               if (fechaAprobacion == "") {
                   data += "NULL\t";
               } else {
                   data += fechaAprobacion + "\t";
               }
               data += this->estadoSolicitudToString(estado) + "\t";
                data += QString::number(idSolicitud) + "\n";
            }
            while (q.next());
        } else {
            qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                     << q.lastError();
        }
    }
    q.clear();

    q.prepare("SELECT * FROM RegistroVacaciones WHERE Usuario = (:usuario)"
              );
    q.bindValue(":usuario", usuario);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString tipoConstancia = "";
            QString fechaSolicitud = "";
            QString fechaAprobacion = "";
            QString fechaInicial = "";
            QString fechaFinal = "";
            estadoSolicitud estado = estadoSolicitud::PENDIENTE;
            quint64 idSolicitud = 0;
            do {
                fechaSolicitud = q.value(2).toString();
                fechaInicial = q.value(3).toString();
                fechaFinal = q.value(4).toString();
                estado = estadoSolicitud(q.value(5).toInt());
                idSolicitud = q.value(6).toLongLong();
                data += "Solicitud de vacaciones \t";
                data += fechaSolicitud + "\t";

                if (fechaAprobacion == "") {
                   data += "NULL\t";
                } else {
                   data += fechaAprobacion + "\t";
                }
                data += this->estadoSolicitudToString(estado) + "\t";
                data += QString::number(idSolicitud) + "\n";
            }
            while (q.next());
        } else {
            qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                     << q.lastError();
        }
    }
    q.clear();
    return data;
}

QString DbManager::consultarSolicitudesPendientes(QString usuario){
    QSqlQuery q;
    QString data = "";

    // nombre, apellido, vacaciones
    q.prepare("SELECT Usuario,Tipo_Constancia,Fecha_Solicitud,Fecha_Aprobacion,Estado,ID_Solicitud "
              "FROM Constancias WHERE Usuario != :usuario AND Estado = 0");
    q.bindValue(":usuario", usuario);

    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: " << q.lastError();
    } else {
        if (q.next()) {
            QString usuario = "";
            QString tipoConstancia = "";
            int numConstancia = 0;
            QString fechaSolicitud = "";
            QString fechaAprobacion = "";
            estadoSolicitud estado = estadoSolicitud::PENDIENTE;
            quint64 idSolicitud = 0;

            do {
                usuario = q.value(0).toString();
                tipoConstancia = q.value(1).toString();
                numConstancia = tipoConstancia.toInt();
                fechaSolicitud = q.value(2).toString();
                fechaAprobacion = q.value(3).toString();
                estado = estadoSolicitud(q.value(4).toInt());
                idSolicitud = q.value(5).toULongLong();

                data += usuario;
                if (numConstancia == CONSTANCIALABORAL) {
                   data += "Constancia Laboral\t";
                } else {
                   data += "Constancia Salarial\t";
                }
                data += fechaSolicitud + "\t";
                if (fechaAprobacion == "") {
                   data += "NULL\t";
                } else {
                   data += fechaAprobacion + "\t";
                }
                data += this->estadoSolicitudToString(estado) + "\t";
                data += QString::number(idSolicitud) + "\n";
            }
            while (q.next());
        } else {
            qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                     << q.lastError();
        }
    }
    q.clear();
    return data;
}

int DbManager::modificarSalario(QString usuario, double nuevoSalarioBruto,
                                                double nuevoSalarioNeto) {
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("SELECT * FROM DatosLaborales WHERE Usuario = (:usuario)"
              " AND Fecha_Final is null");
    q.bindValue(":usuario", usuario);
    if(!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
        error = EXIT_FAILURE;
    } else {
        if (q.next()) {
            QString dbUsuario = q.value(0).toString();
            q.clear();
            q.prepare("UPDATE DatosLaborales"
                      " SET Salario_Bruto = (:nuevoSalarioBruto),"
                      " Salario_Neto = (:nuevoSalarioNeto)"
                      " WHERE Usuario = (:usuario)");
            q.bindValue(":nuevoSalarioBruto", nuevoSalarioBruto);
            q.bindValue(":nuevoSalarioNeto", nuevoSalarioNeto);
            q.bindValue(":usuario", dbUsuario);
            if(!q.exec()) {
                qDebug() << "[DB] Error al actualizar los datos laborales: "
                         << q.lastError();
                error = EXIT_FAILURE;
            } else {
                actualizarSalarios(dbUsuario);
            }
        } else {
            qDebug() << "[DB] Error, el usuario buscado no existe: "
                     << q.lastError();
            error = EXIT_FAILURE;
        }
    }
    q.clear();

    return error;
}

int DbManager::modificarVacaciones(QString usuario, int vacaciones, bool descontar)
{
    int error = EXIT_SUCCESS;
    // Buscar el usuario en la base de datos
    QSqlQuery q;
    q.prepare("SELECT * FROM DatosLaborales WHERE Usuario = (:usuario)"
              " AND Fecha_Final is null");
    q.bindValue(":usuario", usuario);

    qDebug() << "[DB] Usuario: " << usuario;
    qDebug() << "[DB] Vacaciones: " << vacaciones;
    qDebug() << "[DB] Descontar: " << descontar;
    
    if(!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
        error = EXIT_FAILURE;
    } else {
        if (q.first()) {
            QString dbUsuario = q.value(0).toString();
            int dbVacaciones = q.value(6).toInt();
            q.clear();
            // Actualizar el numero de vacaciones disponibles
            if (descontar) {
                q.prepare("UPDATE DatosLaborales"
                          " SET Vacaciones_Disponibles = (:vacaciones)"
                          " WHERE Usuario = (:usuario)");
                q.bindValue(":vacaciones", dbVacaciones - vacaciones);
            } else {
                q.prepare("UPDATE DatosLaborales"
                          " SET Vacaciones_Disponibles = (:vacaciones)"
                          " WHERE Usuario = (:usuario)");
                q.bindValue(":vacaciones", dbVacaciones + vacaciones);
            }
            q.bindValue(":usuario", dbUsuario);
            if(!q.exec()) {
                qDebug() << "[DB] Error al actualizar los datos laborales: "
                         << q.lastError();
                error = EXIT_FAILURE;
            }
        } else {
            qDebug() << "[DB] Error, el usuario buscado no existe: "
                     << q.lastError();
            error = EXIT_FAILURE;
        }
    }
    return error;
}

QList<QString> DbManager::consultarSolicitudes(QString ID_Solicitud, QString usuario, QString estado, QString tipo, QString fecha, QString supervisor_username)
{
    QString area = this->consultarArea(supervisor_username);
    QSqlQuery q;
    QString data = "";
    QList<QString> solicitudes;
    bool done_correctly = false;

    // Prepare the query to select data from the Constancias table based on the given parameters
    done_correctly = q.prepare("SELECT * FROM Constancias WHERE ID_Solicitud LIKE (:ID_Solicitud) AND Usuario LIKE (:usuario) AND Estado LIKE (:estado) \
     AND Tipo_Constancia LIKE (:tipo) AND Fecha_Solicitud LIKE (:fecha) AND Area = (:area)");

    // If the query was not prepared correctly, print an error message
    if (!done_correctly) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        // Bind the values of the parameters to the query
        q.bindValue(":ID_Solicitud", ID_Solicitud);
        q.bindValue(":usuario", usuario);
        q.bindValue(":estado", estado);
        q.bindValue(":tipo", tipo);
        q.bindValue(":fecha", fecha);
        q.bindValue(":area", area);
        
        // If the query was not executed correctly, print an error message
        if (!q.exec()) {
            qDebug() << "[DB] Error al consultar la base de datos: "
                     << q.lastError();
        } else {
            // Loop through the results of the query and append each row to the list of solicitudes
            if (q.next()) {
                do {
                    QString idSolicitud = q.value(0).toString();
                    QString usuario = q.value(1).toString();
                    QString fechaSolicitud = q.value(2).toString();
                    QString fechaAprobacion = q.value(3).toString();
                    double salarioBruto = q.value(4).toDouble();
                    QString stringBruto = QString::number(salarioBruto, 'f', 2);
                    double salarioNeto = q.value(5).toDouble();
                    QString stringNeto = QString::number(salarioNeto, 'f', 2);
                    QString tipoConstancia = q.value(6).toString();
                    QString estado = this->estadoSolicitudToString(estadoSolicitud(q.value(7).toInt()));

                    QString solicitud = idSolicitud + "\t" + usuario + "\t" + fechaSolicitud +
                    "\t" + fechaAprobacion + "\t" + stringBruto + "\t" + stringNeto + "\t"
                    + tipoConstancia + "\t" + estado;
                    solicitudes.append(solicitud);
                } while (q.next());
            } else {
                qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                         << q.lastError();
            }
        }  
    }
    q.clear();

    // Return the list of solicitudes
    return solicitudes;
}

QList<QString> DbManager::consultarSolicitudesVacaciones(QString username, QString estado, QString supervisor_username)
{
    QString area = this->consultarArea(supervisor_username);
    QSqlQuery q;
    QString data = "";
    QList<QString> solicitudes;
    bool done_correctly = false;

    // Prepare the query to select data from the RegistroVacaciones table based on the given parameters
    done_correctly = q.prepare("SELECT * FROM RegistroVacaciones WHERE Usuario LIKE (:username) AND Estado LIKE (:estado) AND Area = (:area)");
    // If the query was not prepared correctly, print an error message
    if (!done_correctly) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        // Bind the values of the parameters to the query
        q.bindValue(":username", username);
        q.bindValue(":estado", estado);
        q.bindValue(":area", area);

        // If the query was not executed correctly, print an error message
        if (!q.exec()) {
            qDebug() << "[DB] Error al consultar la base de datos: "
                     << q.lastError();
        } else {
            // Loop through the results of the query and append each row to the list of solicitudes
            if (q.next()) {
                do {
                    // Usuario, motivo, fechaSolicitud, fechaInicioVacaciones, fechaFinalVacaciones, estado, Id_Solicitud
                    QString usuario = q.value(0).toString();
                    QString motivo = q.value(1).toString();
                    QString fechaSolicitud = q.value(2).toString();
                    QString fechaInicioVacaciones = q.value(3).toString();
                    QString fechaFinalVacaciones = q.value(4).toString();
                    QString estado = this->estadoSolicitudToString(estadoSolicitud(q.value(5).toInt()));
                    QString idSolicitud = q.value(6).toString();

                    QString solicitud = usuario + "\t" + motivo +
                    "\t" + fechaSolicitud + "\t" + fechaInicioVacaciones + "\t" + fechaFinalVacaciones + "\t"
                    + estado + "\t" + idSolicitud;
                    solicitudes.append(solicitud);
                } while (q.next());
            } else {
                qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                         << q.lastError();
            }
        }  
    }
    q.clear();

    // Return the list of solicitudes
    return solicitudes;
}
int DbManager::cambiarEstadoSolicitud(int idSolicitud, estadoSolicitud estado, quint8 tipoRegistro)
{
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    bool done_correctly = false;

    // Prepare the query to update the Estado field of the Constancias table based on the given parameters
    if (tipoRegistro == 0) {
        done_correctly= q.prepare("UPDATE Constancias SET Estado = (:estado) WHERE ID_Solicitud = (:idSolicitud)");
    } else {
        done_correctly= q.prepare("UPDATE RegistroVacaciones SET Estado = (:estado) WHERE Id_Solicitud = (:idSolicitud)");
    }
    if (!done_correctly) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
        error = EXIT_FAILURE;
    } else {
        // Bind the values of the parameters to the query
        q.bindValue(":estado", estado);
        q.bindValue(":idSolicitud", idSolicitud);
        // If the query was not executed correctly, print an error message
        if (!q.exec()) {
            qDebug() << "[DB] Error al consultar la base de datos: "
                     << q.lastError();
            error = EXIT_FAILURE;
        } else {
            // If the query was executed correctly, print a success message
            qDebug() << "[DB] Estado de solicitud actualizado exitosamente";
        }
        this->manejarCambioEstadoSolicitud(idSolicitud, estado, tipoRegistro);
    }

    return error;
}

int DbManager::actualizarSalarios(QString usuario) {
    int error = EXIT_SUCCESS;
    QSqlQuery q;
     q.prepare("UPDATE Constancias"
              " SET Salario_Bruto ="
                  " (SELECT Salario_Bruto"
                  " FROM DatosLaborales"
                  " WHERE Usuario = Constancias.Usuario),"
              " Salario_Neto ="
                  " (SELECT Salario_Neto"
                  " FROM DatosLaborales"
                  " WHERE Usuario = Constancias.Usuario)"
              "WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    if(!q.exec()) {
        qDebug() << "[DB] Error actualizando salarios: " << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}

QString DbManager::estadoSolicitudToString(estadoSolicitud estado)
{
    QString estadoString = "";
    switch (estado) {
    case estadoSolicitud::PENDIENTE:
        estadoString = "Pendiente";
        break;
    case estadoSolicitud::APROBADASUPERVISOR:
        estadoString = "Aprobada por Supervisor";
        break;
    case estadoSolicitud::APROBADA:
        estadoString = "Aprobada";
        break;
    case estadoSolicitud::RECHAZADA:
        estadoString = "Rechazada";
        break;
    }
    return estadoString;
}

int DbManager::manejarCambioEstadoSolicitud(int idSolicitud, estadoSolicitud estado, quint8 tipoRegistro)
{
    int error = EXIT_SUCCESS;
    qDebug() << "[DB] Manejando cambio de estado de solicitud";
    qDebug() << "[DB] ID Solicitud: " << idSolicitud;
    qDebug() << "[DB] Estado: " << estado;
    qDebug() << "[DB] Tipo de registro: " << tipoRegistro;
    QSqlQuery q;
    // Si la solicitud fue aprobada, actualizar la fecha de aprobacion
    if (estado == estadoSolicitud::APROBADA && tipoRegistro == 0) {
        q.prepare("UPDATE Constancias SET Fecha_Aprobacion = (:fechaAprobacion) WHERE ID_Solicitud = (:idSolicitud)");
            q.bindValue(":fechaAprobacion", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
            q.bindValue(":idSolicitud", idSolicitud);

            if (!q.exec()) {
                qDebug() << "[DB] Error al consultar la base de datos: "
                         << q.lastError();
                error = EXIT_FAILURE;
            } else {
                // If the query was executed correctly, print a success message
                qDebug() << "[DB] Fecha de aprobacion actualizada exitosamente";
            }
    } else if (estado == estadoSolicitud::APROBADA && tipoRegistro == 1) { // Se aprobo y es una solicitud de vacaciones
        // Actualizar el numero de vacaciones disponibles
        q.prepare("SELECT * FROM RegistroVacaciones WHERE Id_Solicitud = (:idSolicitud)");
        q.bindValue(":idSolicitud", idSolicitud);

        if (!q.exec()) {
            qDebug() << "[DB] Error al consultar la base de datos: "
                     << q.lastError();
            error = EXIT_FAILURE;
        } else {
            if (q.next()) {
                QDateTime fechaInicio = QDateTime::fromString(q.value(3).toString(), "dd/MM/yyyy");
                QDateTime fechaFinal = QDateTime::fromString(q.value(4).toString(), "dd/MM/yyyy");
                int diasVacaciones = fechaInicio.daysTo(fechaFinal);

                this->modificarVacaciones(q.value(0).toString(), diasVacaciones, true);
            } else {
                qDebug() << "[DB] Error, no existe la solicitud de vacaciones: "
                         << q.lastError();
                error = EXIT_FAILURE;
            }
        }
    }
    return error;
}

bool DbManager::pidioConstancia(QString usuario, int tipoConstancia)
{
    bool pidio = false;
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM Constancias WHERE Usuario = (:usuario) AND Tipo_Constancia = (:tipoConstancia) AND Fecha_Solicitud >= DATE('now', '-7 days')");
    q.bindValue(":usuario", usuario);
    q.bindValue(":tipoConstancia", tipoConstancia);
    if(q.exec() && q.next()) {
        pidio = q.value(0).toInt() > 0;
    } else {
        qDebug() << "[DB] Error al consultar si el usuario pidio constancia: " << q.lastError();
    }
    q.clear();
    return pidio;
}

QString DbManager::consultarAnotacionesExpediente(QString usuario) {
    QString respuestaAnotaciones;
    QSqlQuery q;
    q.prepare("SELECT Anotaciones FROM DatosLaborales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    
    if (q.exec() && q.next()) {
        respuestaAnotaciones = q.value(0).toString();
    } else {
        qDebug() << "[DB] Error al consultar anotaciones: " << q.lastError();
    }
    q.clear();

    return respuestaAnotaciones;
}

bool DbManager::actualizarAnotacionesExpediente(QString usuario, QString anotaciones) {
    bool actualizado = false;
    QSqlQuery q;
    q.prepare("UPDATE DatosLaborales SET Anotaciones = (:anotaciones) WHERE Usuario = (:usuario)");
    q.bindValue(":anotaciones", "\n" + anotaciones);
    q.bindValue(":usuario", usuario);
    if (q.exec()) {
        actualizado = true;
    } else {
        qDebug() << "[DB] Error al actualizar anotaciones: " << q.lastError();
    }
    q.clear();
    return actualizado;
}

QList<QString> DbManager::listarUsuarios() {
    QSqlQuery q;
    QString data = "";
    QList<QString> usuarios;

    // nombre, apellido, vacaciones
    q.prepare("SELECT personal.Usuario,personal.Nombre,personal.Apellido"
              " FROM DatosPersonales personal INNER JOIN DatosLaborales laboral"
              " ON personal.Usuario = laboral.Usuario WHERE laboral.Fecha_Final IS NULL");
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString username = "";
            QString name = "";
            QString lastname = "";
            do {
                username = q.value(0).toString();
                name = q.value(1).toString();
                lastname = q.value(2).toString();

                QString usuario = username + "\t" + name + " " + lastname;
                usuarios.append(usuario);
            } while (q.next());
        } else {
            qDebug() << "[DB] Error, no existen usuarios: "
                     << q.lastError();
        }
    }
    q.clear();
    return usuarios;
}

QByteArray DbManager::consultarDatosDeUsuario(QString username) {
    //username, name, lastname, email, phone, personalId, job, grosspay, netpay
    QSqlQuery q;
    QByteArray datagrama(147, '\0');
    q.prepare("SELECT personal.Usuario,personal.Nombre,personal.Apellido,"
              "personal.Correo,personal.Num_teléfono,personal.Num_cedula,"
              "laboral.Puesto,laboral.Salario_Bruto,laboral.Salario_Neto"
              " FROM DatosPersonales personal INNER JOIN DatosLaborales"
              " laboral ON personal.Usuario = (:usuario) AND"
              " laboral.Usuario = (:usuario) WHERE laboral.Fecha_Final IS NULL");
    q.bindValue(":usuario", username);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            QString usuario = q.value(0).toString();
            QString nombre = q.value(1).toString();
            QString apellido= q.value(2).toString();
            QString correo = q.value(3).toString();
            QString telefono = q.value(4).toString();
            QString cedula = q.value(5).toString();
            QString puesto = q.value(6).toString();
            double salarioBruto = q.value(7).toDouble();
            QString stringBruto = QString::number(salarioBruto, 'f', 2);
            double salarioNeto = q.value(8).toDouble();
            QString stringNeto = QString::number(salarioNeto, 'f', 2);
            datagrama.replace(0, qMin(usuario.length(), 10), usuario.toUtf8());
            datagrama.replace(10, qMin(nombre.length(), 15), nombre.toUtf8());
            datagrama.replace(25, qMin(apellido.length(), 15), apellido.toUtf8());
            datagrama.replace(40, qMin(correo.length(), 31), correo.toUtf8());
            datagrama.replace(71, qMin(telefono.length(), 9), telefono.toUtf8());
            datagrama.replace(80, qMin(cedula.length(), 11), cedula.toUtf8());
            datagrama.replace(91, qMin(puesto.length(), 30), puesto.toUtf8());
            datagrama.replace(121, qMin(stringBruto.length(), 13), stringBruto.toUtf8());
            datagrama.replace(134, qMin(stringNeto.length(), 13), stringNeto.toUtf8());
        } else {
            qDebug() << "[DB] Error, no existe el usuario buscado: "
                     << q.lastError();
        }
    }
    q.clear();
    return datagrama;
}

int DbManager::actualizarDatosLaborales(QString username,QString job,
                                        QString grossPay, QString netPay) {
    username = username.trimmed();
    job = job.trimmed();
    grossPay = grossPay.trimmed();
    netPay = netPay.trimmed();
    
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("UPDATE DatosLaborales"
              " SET Puesto = (:puesto),"
              " Salario_Bruto = (:grossPay),"
              " Salario_Neto = (:netPay)"
              " WHERE Usuario = (:username)");
    q.bindValue(":puesto", job);
    q.bindValue(":grossPay", grossPay);
    q.bindValue(":netPay", netPay);
    q.bindValue(":username", username);
    if(!q.exec()) {
        qDebug() << "[DB] Error actualizando Datos Laborales: " << q.lastError();
        error = EXIT_FAILURE;
    } else {
        this->actualizarSalarios(username);
    }
    q.clear();
    return error;
}

int DbManager::actualizarDatosPersonales(QString username, QString name, QString lastname,
                                         QString email, QString phone, QString personalID) {
    username = username.trimmed();
    name = name.trimmed();
    lastname = lastname.trimmed();
    email = email.trimmed();
    phone = phone.trimmed();
    personalID = personalID.trimmed();

    int error = EXIT_SUCCESS;
    QSqlQuery q;
    q.prepare("UPDATE DatosPersonales"
              " SET Nombre = (:nombre),"
              " Apellido = (:apellido),"
              " Correo = (:correo),"
              " Num_teléfono = (:telefono),"
              " Num_cedula = (:cedula)"
              " WHERE Usuario = (:username)");
    q.bindValue(":nombre", name);
    q.bindValue(":apellido", lastname);
    q.bindValue(":correo", email);
    q.bindValue(":telefono", phone);
    q.bindValue(":cedula", personalID);
    q.bindValue(":username", username);
    if(!q.exec()) {
        qDebug() << "[DB] Error actualizando Datos Personales: " << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}
QString DbManager::consultarArea(QString usuario) {
    QSqlQuery q;
    QString data = "";

    q.prepare("SELECT * FROM DatosLaborales WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    q.exec();

    if (q.next()) {
        QString area = q.value(8).toString();
        data = area;
    } else {
        data += "[DB] No existe el usuario digitado";
    }
    q.clear();
    return data;
}

QString DbManager::consultarUsuariosAnotaciones(QString usuario)
{
    QString usuarios;

    // Sacamos el area del usuario que esta pidiendo los usuarios
    QString area = this->consultarArea(usuario);
    
    // Sacamos los usuarios y las anotaciones con un query

    QSqlQuery datos;
    datos.prepare("SELECT Usuario, Anotaciones FROM DatosLaborales WHERE Area = (:area) AND Usuario != (:usuario)");
    datos.bindValue(":area", area);
    datos.bindValue(":usuario", usuario);

    if (!datos.exec()) {
        qDebug() << "[DB] Error al consultar usuarios. QSqlQuery " << datos.lastError();
    } else {
        while (datos.next()) {
            QString usuario = datos.value(0).toString();
            QString anotaciones = datos.value(1).toString();
            usuarios += usuario + "\t" + anotaciones + "|";
        }

        // Delete the last character of the string
        usuarios.chop(1);
    }

    return usuarios;
}


QList<QString> DbManager::consultarComprobantesPago(QString usuario)
{
    QSqlQuery q;
    QList<QString> datos;

    // usuario, fecha de pago y cantidad pagada
    q.prepare("SELECT * FROM ComprobantesPago WHERE Usuario = (:usuario)");
    q.bindValue(":usuario", usuario);
    if (!q.exec()) {
        qDebug() << "[DB] Error al consultar la base de datos: "
                 << q.lastError();
    } else {
        if (q.next()) {
            do {
                QString usuario = q.value(0).toString();
                QString fechaPago = q.value(1).toString();
                double cantidadPagada = q.value(2).toDouble();
                QString stringCantidadPagada = QString::number(cantidadPagada, 'f', 2);
                QString dato = usuario + "\t" + fechaPago + "\t" + stringCantidadPagada;
                datos.append(dato);
            } while (q.next());
        } else {
            qDebug() << "[DB] Error, no existen solicitudes del usuario: "
                     << q.lastError();
        }
    }
    q.clear();
    return datos;
}

int DbManager::eliminarUsuario(QString username) {
    int error = EXIT_SUCCESS;
    QSqlQuery q;
    QDate currentDate = QDate::currentDate();
    QString stringDate = currentDate.toString("dd-MM-yyyy");
    q.prepare("UPDATE DatosLaborales SET Fecha_Final = (:fechaFinal) WHERE Usuario = (:usuario)");
    q.bindValue(":fechaFinal", stringDate);
    q.bindValue(":usuario", username);
    if(!q.exec()) {
        qDebug() << "[DB] Error eliminando usuario en DB: " << q.lastError();
        error = EXIT_FAILURE;
    }
    q.clear();
    return error;
}
