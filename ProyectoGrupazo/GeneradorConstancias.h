#ifndef GENERADORCONSTANCIAS_H
#define GENERADORCONSTANCIAS_H

#include <QString>

class GeneradorConstancias
{
public:
    GeneradorConstancias();
    QString ConstanciaSalarialAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& cedula, const QString& SalarioNeto);
    QString ConstanciaSalarialRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud);
    QString ConstanciaLaboralAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& cedula, const QString& SalarioNeto, const QString& Puesto, const QString& FechaInicioLabores);
    QString ConstanciaLaboralRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud);
    QString SolicitudVacacionesAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud, const QString& DiaIngreso, const QString& MesIngreso, const QString& AnnoIngreso, const QString& DiaFin, const QString& MesFin, const QString& AnnoFin, const QString& Dias, const QString& DiaSiguienteFin, const QString& MesSiguienteFin, const QString& AnnoSiguienteFin);
    QString SolicitudVacacionesRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud, const QString& DiaIngreso, const QString& MesIngreso, const QString& AnnoIngreso, const QString& DiaFin, const QString& MesFin, const QString& AnnoFin);
    QString informeComprobantesDePago(const QString& FechaActual, const QString& nombre, const QString& fechaPago, const QString& cantidadPago);
    QString informeSaldoVacaciones();
    QString informeSalarioBase();
};

#endif // GENERADORCONSTANCIAS_H
