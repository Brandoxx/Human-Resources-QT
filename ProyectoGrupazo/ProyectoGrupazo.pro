QT       += widgets core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ActualizarDatosUsuario.cpp \
    ActualizarUsuarios.cpp \
    AdministradorMenu.cpp \
    AgregarUsuarios.cpp \
    AnotacionExpediente.cpp \
    BorrarUsuarios.cpp \
    Client.cpp \
    ConstanciaUsuarios.cpp \
    EmpleadoMenu.cpp \
    EstadoSolicitudesHechas.cpp \
    GeneradorConstancias.cpp \
    RevisionSolicitudes.cpp \
    ShowLogs.cpp \
    SolicitudVacaciones.cpp \
    SupervisorMenu.cpp \
    UsuarioConsultas.cpp \
    UsuarioSolicitudes.cpp \
    main.cpp \
    Login.cpp \
    Auditor.cpp

HEADERS += \
    ActualizarDatosUsuario.h \
    ActualizarUsuarios.h \
    AdministradorMenu.h \
    AgregarUsuarios.h \
    AnotacionExpediente.h \
    BorrarUsuarios.h \
    Client.h \
    ConstanciaUsuarios.h \
    EmpleadoMenu.h \
    EstadoSolicitudesHechas.h \
    GeneradorConstancias.h \
    Login.h \
    Message.h \
    RevisionSolicitudes.h \
    ShowLogs.h \
    SolicitudVacaciones.h \
    SupervisorMenu.h \
    UsuarioConsultas.h \
    UsuarioSolicitudes.h \
    Auditor.h \ 
    Logger.hpp

FORMS += \
    ActualizarDatosUsuario.ui \
    ActualizarUsuarios.ui \
    AdministradorMenu.ui \
    AgregarUsuarios.ui \
    AnotacionExpediente.ui \
    Auditor.ui \
    BorrarUsuarios.ui \
    ConstanciaUsuarios.ui \
    EmpleadoMenu.ui \
    EstadoSolicitudesHechas.ui \
    Login.ui \
    RevisionSolicitudes.ui \
    ShowLogs.ui \
    SolicitudVacaciones.ui \
    SupervisorMenu.ui \
    UsuarioConsultas.ui \
    UsuarioSolicitudes.ui \
    Auditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
