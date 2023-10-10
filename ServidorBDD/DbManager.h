#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardItemModel>
#include "Message.h"


class DbManager {
public:
    const int CONSTANCIALABORAL = 1;
    const int CONSTANCIASALARIAL = 2;

    enum estadoSolicitud {
        PENDIENTE,
        APROBADASUPERVISOR,
        APROBADA,
        RECHAZADA };

    /**
     * @brief Construct a new Db Manager object
     */
    DbManager();
    /**
     * @brief Destroy the Db Manager object
     */
    ~DbManager();

    /**
     * @brief Metodo para agregar datos personales de un usuario
     * @param usuario nombre de usuario
     * @param nombre nombre de pila del usuario
     * @param apellido apellido del usuario
     * @param correo correo del usuario
     * @param telefono telefono del usuario
     * @param cedula cedula del usuario
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int agregarDatosPersonales(QString usuario, QString nombre,
                               QString apellido, QString correo,
                               QString telefono, QString cedula);

    /**
     * @brief Metodo para agregar datos laborales de un usuario
     * @param usuario nombre de usuario
     * @param puesto puesto del usuario
     * @param fechaInicial fecha donde empezo a laborar el usuario
     * @param salarioBruto salario bruto del usuario
     * @param salarioNeto salario neto del usuario
     * @param vacaciones vacaciones disponibles
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int agregarDatosLaborales(QString usuario, QString puesto,
                                QString fechaInicial, int salarioBruto,
                                int salarioNeto, int vacaciones, QString area);

    /**
     * @brief Metodo para agregar una constancia
     * @param idSolicitud identificador de la constancia
     * @param usuario nombre de usuario
     * @param fechaSolicitud fecha dde solicitud de la constancia
     * @param salarioBruto salario bruto del usuario
     * @param salarioNeto salario neto del usuario
     * @param tipoConstancia tipo de constancia, 1 laboral, 2 salarial
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int agregarConstancia(QString usuario, QString fechaSolicitud,
                          int salarioBruto, int salarioNeto,
                          int tipoConstancia);

    /**
     * @brief Metodo para registrar una solicitud de vacaciones
     * @param usuario nombre del usuario solicitante
     * @param motivo motivo de la solicitud
     * @param fechaSolicitud fecha de la solicitud
     * @param fechaInicioVacaciones fecha de inicio de vacaciones
     * @param fechaFinalVacaciones fecha de fin de vacaciones
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int agregarSolicitudVacaciones(QString usuario, QString motivo,
                                       QString fechaSolicitud,
                                       QString fechaInicioVacaciones,
                                       QString fechaFinalVacaciones);

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
     * @return QByteArray QByteArray con los datos encontrados en la tabla
     */
    QByteArray consultarConstancia(int idSolicitud);

    /**
     * @brief Consulta el saldo de vacaciones de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con nombre, apellido y saldo de vacaciones
     */
    QByteArray consultarSaldoVacaciones(QString usuario);

    /**
     * @brief Consulta el saldo de vacaciones de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con nombre, apellido, salario y fecha de ingreso
     */
    QByteArray consultarSalarioBase(QString usuario);

    /**
     * @brief Consulta las solicitudes hechas por un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con informacion de las solicitudes realizadas
     */
    QString consultarEstadoDeSolicitudes(QString usuario);

    /**
     * @brief Consulta las solicitudes hechas por un usuario que faltan de ser revisadas
     * @param usuario Nombre de usuario a utilizar en el query
     * @return QString String con informacion de las solicitudes
     */
    QString consultarSolicitudesPendientes(QString usuario);

    /**
     * @brief Metodo para modificar el salario de un usuario
     * @param usuario usuario a modificar salario
     * @param nuevoSalarioBruto nuevo salario bruto
     * @param nuevoSalarioNeto nuevo salario neto
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int modificarSalario(QString usuario, double nuevoSalarioBruto,
                                            double nuevoSalarioNeto);

    /**
     * @brief Modifica el saldo de vacaciones de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @param vacaciones Cantidad de vacaciones a agregar o descontar
     * @param descontar Indica si se debe descontar o agregar las vacaciones
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int modificarVacaciones(QString usuario, int vacaciones, bool descontar);

    /**
     * @brief Metodo para obtener todas las solicitudes siguiendo ciertos filtros
     * @param estado estado de la solicitud
     * @param tipo tipo de solicitud
     * @param fecha fecha de la solicitud
     * @return QString String con informacion de las solicitudes realizadas
     */
    QList<QString> consultarSolicitudes(QString ID_Solicitud = "%", QString usuario = "%",
                                 QString estado = "%", QString tipo = "%",
                                 QString fecha = "%", QString supervisor_username = "");

    /**
     * @brief Consulta las solicitudes de vacaciones de un usuario
     * 
     * Esta función consulta la base de datos para obtener las solicitudes de vacaciones de un usuario.
     * 
     * @param username El nombre de usuario cuyas solicitudes se están consultando
     * @param estado El estado de las solicitudes a consultar
     * @return Una QList<QString> que contiene información de las solicitudes de vacaciones
     */
    QList<QString> consultarSolicitudesVacaciones(QString username = "%", QString estado = "%", QString supervisor_username = "");

    /**
     * @brief Cambia el estado de una solicitud
     * @param idSolicitud id de la solicitud a cambiar
     * @param estado nuevo estado de la solicitud
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int cambiarEstadoSolicitud(int idSolicitud, estadoSolicitud estado, quint8 tipoRegistro);

    /**
     * @brief Metodo para saber si un usuario ya pidio una constancia en la ultima semana
     * @param usuario nombre de usuario a utilizar en el query
     * @param tipoConstancia tipo de constancia a consultar
     * @return bool true si ya pidio una constancia, false si no
     */
    bool pidioConstancia(QString usuario, int tipoConstancia);

        /**
     * @brief Consulta las anotaciones de un expediente de usuario
     * 
     * Esta función consulta la base de datos para obtener las anotaciones de un expediente de usuario.
     * 
     * @param usuario El nombre del usuario cuyo expediente se está consultando
     * @return Una QString que contiene las anotaciones del expediente del usuario
     */
    QString consultarAnotacionesExpediente(QString usuario);

    /**
     * @brief Actualiza las anotaciones de un expediente de usuario
     * 
     * Esta función actualiza las anotaciones de un expediente de usuario en la base de datos.
     * 
     * @param usuario El nombre del usuario cuyo expediente se está actualizando
     * @param anotaciones Las nuevas anotaciones del expediente del usuario
     * @return true si la actualización fue exitosa, false si no
     */
    bool actualizarAnotacionesExpediente(QString usuario, QString anotaciones);

    /**
     * @brief Consulta los datos de un usuario
     * @param username nombre de usuario del quien se va a consultar informacion
     * @return QByteArray informacion del usuario
     */
    QByteArray consultarDatosDeUsuario(QString username);

    /**
     * @brief Actualiza los datos laborales en DB
     * @param username nombre de usuario a actualizar
     * @param job puesto a actualizar
     * @param grossPay salario bruto a actualizar
     * @param netPay salario neto a actualizar
     * @return int 0 si el query fue exitoso, 1 sino
     */
    int actualizarDatosLaborales(QString username, QString job,
                                 QString grossPay, QString netPay);

    /**
     * @brief Actualiza los datos personales en DB
     * @param username nombre de usuario a actualizar
     * @param name nombre a actualizar
     * @param lastname apellido a actualizar
     * @param email correo a actualizar
     * @param phone telefono a actualizar
     * @param personalID cedula a actualizar
     * @return int 0 si el query fue exitoso, 1 sino
     */
    int actualizarDatosPersonales(QString username, QString name, QString lastname,
                                  QString email, QString phone, QString personalID);

    /**
     * @brief Devuelve una lista con los usuarios en sistema
     * @return QList<QString> lista de los usuarios
     */
    QList<QString> listarUsuarios();

    /**
     * @brief Consulta los datos de una solicitud de vacaciones dada
     * @param idSolicitud Id de la solicitud a buscar en la base de datos
     * @return QByteArray QByteArray con los datos encontrados en la tabla
     */
    QByteArray consultarSolicitudVacaciones(int idSolicitud);

    /**
     * @brief Obtiene el area de trabajo de un usuario
     *
     * @param usuario El usuario
     * @return el area de trabajo del usuario
     */
    QString consultarArea(QString usuario);

    /**
     * @brief Obtiene cada usuario junto con sus anotaciones al expediente.
     * @return Un string con cada usuario\tanotaciones
    */
    QString consultarUsuariosAnotaciones(QString usuario = "%");

    /**
     * @brief Consulta los comprobantes de pago de un usuario
     * @param usuario Nombre de usuario a utilizar en el query
     * @return Una QList<QString> que contiene la lista de comprobantes (información de usuario, fecha de pago y cantidad pagada)
     */
    QList<QString> consultarComprobantesPago(QString usuario);

    /**
     * @brief Elimina un usuario
     * @param username usuario a eliminar
     * @return 0 si se hizo el query, 1 sino
     */
    int eliminarUsuario(QString username);
    
private:
    QSqlDatabase db; // Variable utilizada para la conexion con la base de datos

    /**
     * @brief Metodo que actualiza el salario de un usuario en
     *                                   la tabla de constancias
     * @param usuario nombre de usuario a utilizar en el query
     * @return int 0 si el query fue efectivo, 1 si no
     */
    int actualizarSalarios(QString usuario);

    /**
     * @brief Metodo toString para el enum estadoSolicitud
     * @param estado estado de la solicitud
     * @return QString string con el estado de la solicitud
     */
    QString estadoSolicitudToString(estadoSolicitud estado);

    int manejarCambioEstadoSolicitud(int idSolicitud, estadoSolicitud estado, quint8 tipoRegistro);


};
#endif // DBMANAGER_H
