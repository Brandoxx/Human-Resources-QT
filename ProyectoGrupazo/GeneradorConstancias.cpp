#include "GeneradorConstancias.h"

GeneradorConstancias::GeneradorConstancias()
{
}

QString GeneradorConstancias::ConstanciaSalarialAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& cedula, const QString& SalarioNeto)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Constancia Salarial</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Por medio de la presente hago constar que " + nombre + " " + apellido + " quien se identifica con cédula de identidad No " + cedula + ", labora para la empresa Grupazo Corporation, devengando un salario mensual de " + SalarioNeto + ".</p>";
    informacion += "<p style='text-align: justify;'>Sin más a que hacer referencia extiendo la presente para fines que estime conveniente el interesado en la fecha de " + FechaActual + ".</p>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::ConstanciaSalarialRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Constancia Salarial</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Estimado(a) " + nombre + " " + apellido + ",</p>";
    informacion += "<p style='text-align: justify;'>Mediante el presente damos respuesta a su solicitud con fecha del FECHA INGRESO, mediante la cual solicita al Departamento de Recursos Humanos la constancia salarial</p>";
    informacion += "<p style='text-align: justify;'>Lamentamos informarle que su solicitud de constancia salarial ha sido rechazada, agradecemos su comprensión.</p>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::ConstanciaLaboralAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& cedula, const QString& SalarioNeto, const QString& Puesto, const QString& FechaInicioLabores)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Constancia Laboral</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";
        informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Por medio de la presente hago constar que " + nombre + " " + apellido + " quien se identifica con cédula de identidad No " + cedula + ", presta sus servicios como " + Puesto + " para la empresa Grupazo Corporation desde la fecha " + FechaInicioLabores+ " hasta la fecha actual, devengando un salario mensual de " + SalarioNeto + ".</p>";
        informacion += "<p style='text-align: justify;'>Sin más a que hacer referencia extiendo la presente para fines que estime conveniente el interesado en la fecha de " + FechaActual + ".</p>";
        informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::ConstanciaLaboralRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>>Constancia Laboral</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";;
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Estimado(a) " + nombre + " " + apellido + ",</p>";
    informacion += "<p style='text-align: justify;'>Mediante el presente damos respuesta a su solicitud con fecha del " + FechaIngresoSolicitud + ", mediante la cual solicita al Departamento de Recursos Humanos la constancia laboral</p>";
    informacion += "<p style='text-align: justify;'>Lamentamos informarle que su solicitud de constancia laboral ha sido rechazada, agradecemos su comprensión.</p>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}



QString GeneradorConstancias::SolicitudVacacionesAprobada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud, const QString& DiaIngreso, const QString& MesIngreso, const QString& AnnoIngreso, const QString& DiaFin, const QString& MesFin, const QString& AnnoFin, const QString& Dias, const QString& DiaSiguienteFin, const QString& MesSiguienteFin, const QString& AnnoSiguienteFin)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Solicitud de Vacaciones</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";;
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Estimado(a) " + nombre + " " + apellido + ",</p>";
    informacion += "<p style='text-align: justify;'>Mediante el presente damos respuesta a su solicitud con fecha del " + FechaIngresoSolicitud + ", mediante la cual solicita al Departamento de Recursos Humanos vacaciones a partir del día " + DiaIngreso + " del mes " +   MesIngreso + " del año " + AnnoIngreso + " hasta el día " + DiaFin + " del mes " + MesFin + " del año " + AnnoFin+ " </p>";
    informacion += "<p style='text-align: justify;'>Me complace informarle que su solicitud de vacaciones ha sido aprobada del periodo de " + Dias +  " días, debiendo reintegrarse a sus labores el día " + DiaSiguienteFin + " del mes " + MesSiguienteFin + " del año " + AnnoSiguienteFin + " en su jornada habitual.</p>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::SolicitudVacacionesRechazada(const QString& FechaActual, const QString& nombre, const QString& apellido, const QString& FechaIngresoSolicitud, const QString& DiaIngreso, const QString& MesIngreso, const QString& AnnoIngreso, const QString& DiaFin, const QString& MesFin, const QString& AnnoFin)
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Solicitud de Vacaciones</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";;
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Estimado(a) " + nombre + " " + apellido + ",</p>";
    informacion += "<p style='text-align: justify;'>Mediante el presente damos respuesta a su solicitud con fecha del " + FechaIngresoSolicitud + ", mediante la cual solicita al Departamento de Recursos Humanos vacaciones a partir del día " + DiaIngreso + " del mes " +   MesIngreso + " del año " + AnnoIngreso + " hasta el día " + DiaFin + " del mes " + MesFin + " del año " + AnnoFin+ " </p>";
    informacion += "<p style='text-align: justify;'>Lamentamos informarle que su solicitud de vacaciones ha sido rechazada, agradecemos su comprensión.</p>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::informeSaldoVacaciones()
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h1 style='text-align: center;'>Saldo de Vacaciones</h1>";
    informacion += "<hr>";
    informacion += "<p><strong>Nombre:</strong> David</p>";
    informacion += "<p><strong>Apellido:</strong> Jimenez</p>";
    informacion += "<p><strong>Permisos:</strong> Empleado</p>";
    informacion += "<p><strong>Área:</strong> Central</p>";
    informacion += "<p><strong>Sucursal:</strong> Cartago</p>";
    informacion += "<hr>";
    informacion += "<p><strong>Días disponibles:</strong> 3</p>";
    informacion += "<p><strong>Días utilizados:</strong> 0</p>";
    informacion += "<p><strong>Días acumulados:</strong> 3</p>";
    informacion += "<hr>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}

QString GeneradorConstancias::informeSalarioBase()
{
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h1 style='text-align: center;'>Salario Base</h1>";
    informacion += "<hr>";
    informacion += "<p><strong>Nombre:</strong> Laura</p>";
    informacion += "<p><strong>Apellido:</strong> Gómez</p>";
    informacion += "<p><strong>Permisos:</strong> Supervisor</p>";
    informacion += "<p><strong>Área:</strong> Central</p>";
    informacion += "<p><strong>Sucursal:</strong> San José</p>";
    informacion += "<hr>";
    informacion += "<p><strong>Salario Base:</strong> ₡800 000</p>";
    informacion += "<p><strong>Fecha de ingreso:</strong> 01/01/2023</p>";
    informacion += "<hr>";
    informacion += "</body></html>";
    return informacion;
}


QString GeneradorConstancias::informeComprobantesDePago(const QString& FechaActual, const QString& nombre, const QString& fechaPago, const QString& cantidadPago) {
    QString informacion = "<html>";
    informacion += "<body style='font-family: Arial, sans-serif;'>";
    informacion += "<h2 style='text-align: center;'>Comprobante de Pago</h2>";
    informacion += "<h4 style='text-align: center;'>Grupazo Corporation</h4>";
    informacion += "<h4 style='text-align: center;'>San José Costa Rica," + FechaActual + "</h4>";;
    informacion += "<h4 style='text-align: center;'>Contacto: <a href='mailto:grupazocorp@grupazo.cr'>grupazocorp@grupazo.cr</a></h4>";
    informacion += "<h4 style='text-align: center;'>TEL: 2296507</h4>";
    informacion += "<p style='text-align: justify;'>Estimado(a) " + nombre + ",</p>";
    informacion += "<p style='text-align: justify;'>Este comprobante de pago certifica que se ha realizado el pago correspondiente al usuario " + nombre + " en la fecha " + fechaPago + ". La cantidad pagada es de " + cantidadPago + ". </p>";
    informacion += "<hr>";
    informacion += "</body>";
    informacion += "</html>";
    return informacion;
}
