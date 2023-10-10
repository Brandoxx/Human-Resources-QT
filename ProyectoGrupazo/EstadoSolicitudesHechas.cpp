#include "EstadoSolicitudesHechas.h"
#include "ui_EstadoSolicitudesHechas.h"

#include <QMessageBox>
#include <QDate>
#include "Login.h"
#include "ConstanciaUsuarios.h"
#include "GeneradorConstancias.h"

EstadoSolicitudesHechas::EstadoSolicitudesHechas(QWidget *parent, UsuarioConsultas *UsuarioConsultasParent, Client& client, QString username) :
    QWidget(parent, Qt::Window),
    ui(new Ui::EstadoSolicitudesHechas),
    parentWindow(parent),
    UsuarioConsultasParentWindow(UsuarioConsultasParent),
    client(client),
    username(username)
{
    ui->setupUi(this);

    // Se obtiene una referencia al QVBoxLayout existente en el diseño principal
    QVBoxLayout *verticalLayout = ui->verticalLayout_2; // Cambio aquí al QVBoxLayout deseado

    // Se crea un QHBoxLayout adicional para centrar listaWidget
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    // horizontalLayout->setAlignment(Qt::AlignCenter);

    connect(&client, &Client::dataReceived, this, &EstadoSolicitudesHechas::solicitudes_recibidas);

    // Se agrega listaWidget al QHBoxLayout
    listaWidget = new QListWidget();
    // Activar las barras de desplazamiento vertical y horizontal en QListWidget
    listaWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // listaWidget->setMaximumHeight(200); // Ajusta la altura máxima según sea necesario
    listaWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    horizontalLayout->addWidget(listaWidget);

    // Se agrega el QHBoxLayout al QVBoxLayout
    verticalLayout->addLayout(horizontalLayout);

    // Se agrega usuarios de prueba
    agregarUsuariosDePrueba();
    conectarBotonesDetalles();
}

EstadoSolicitudesHechas::~EstadoSolicitudesHechas()
{
    delete ui;
}

void EstadoSolicitudesHechas::verDetallesUsuario()
{
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());

    QString nombreSolicitud = senderButton->property("nombreSolicitud").toString();
    int idSolicitud = senderButton->property("idSolicitud").toInt();
    QString estadoSolicitud = senderButton->property("estadoSolicitud").toString();
    qDebug() << "Estado solicitud: " << estadoSolicitud;
    // Mostrar los detalles del usuario seleccionado
    QString detalles = QString("Nombre de solicitud: %1\nID: %2\nEstado: %3")
                           .arg(nombreSolicitud)
                           .arg(idSolicitud)
                           .arg(estadoSolicitud);
    GeneradorConstancias informe;
    if(nombreSolicitud == "Constancia Salarial" || nombreSolicitud == "Constancia Laboral"){
        if(estadoSolicitud == "Aprobada") {
            QEventLoop loop;
            Message solicitudMessage;
            solicitudMessage.type = Message::ConsultarConstancia;
            QByteArray datagrama(2, '\0');
            datagrama.replace(0, 2, (std::to_string(idSolicitud).c_str()));
            solicitudMessage.data = datagrama;

            connect(this, &EstadoSolicitudesHechas::leidas_solicitudes, &loop, &QEventLoop::exit);

            client.sendMessage(solicitudMessage);

            loop.exec();
            if(nombreSolicitud == "Constancia Salarial"){
                this->hide();
                QDate fechaActual = QDate::currentDate();
                QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
                ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.ConstanciaSalarialAprobada(formattedActualDate, datosConstancia.Nombre, datosConstancia.Apellido,datosConstancia.Cedula, datosConstancia.SalarioNeto ));
                constancia->show();
            } else if(nombreSolicitud == "Constancia Laboral"){
                this->hide();
                QDate fechaActual = QDate::currentDate();
                QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
                ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.ConstanciaLaboralAprobada(formattedActualDate,datosConstancia.Nombre, datosConstancia.Apellido, datosConstancia.Cedula, datosConstancia.SalarioNeto,datosConstancia.Puesto,datosConstancia.FechaInicioLabores));
                constancia->show();
            }
        } else if(estadoSolicitud == "Rechazada") {
            QEventLoop loop;
            Message solicitudMessage;
            solicitudMessage.type = Message::ConsultarConstancia;
            QByteArray datagrama(2, '\0');
            datagrama.replace(0, 2, (std::to_string(idSolicitud).c_str()));
            solicitudMessage.data = datagrama;

            connect(this, &EstadoSolicitudesHechas::leidas_solicitudes, &loop, &QEventLoop::exit);

            client.sendMessage(solicitudMessage);

            loop.exec();
            if(nombreSolicitud == "Constancia Salarial"){
                this->hide();
                QDate fechaActual = QDate::currentDate();
                QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
                ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.ConstanciaSalarialRechazada(formattedActualDate, datosConstancia.Nombre, datosConstancia.Apellido, datosConstancia.FechaEnvio));
                constancia->show();
            } else if(nombreSolicitud == "Constancia Laboral"){
                this->hide();
                QDate fechaActual = QDate::currentDate();
                QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
                ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.ConstanciaLaboralRechazada(formattedActualDate, datosConstancia.Nombre, datosConstancia.Apellido, datosConstancia.FechaEnvio));
                constancia->show();
            }
        } else{
            QMessageBox::information(this, "Detalles del Usuario", detalles);
        }
    } else if(nombreSolicitud == "Solicitud de vacaciones "){
        if(estadoSolicitud == "Aprobada") {
            QEventLoop loop;
            Message solicitudMessage;
            solicitudMessage.type = Message::ConsultarSolicitudVacaciones;
            QByteArray datagrama(2, '\0');
            datagrama.replace(0, 2, (std::to_string(idSolicitud).c_str()));
            solicitudMessage.data = datagrama;

            connect(this, &EstadoSolicitudesHechas::leidas_solicitudes, &loop, &QEventLoop::exit);

            client.sendMessage(solicitudMessage);

            loop.exec();

            this->hide();

            QDate fechaActual = QDate::currentDate();
            QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
            QDate fechaInicioVacaciones = QDate::fromString(datosConstancia.FechaInicioVacaciones, "dd/MM/yyyy");
            QString diaI;
            QString mesI;
            QString annoI;
            if (fechaInicioVacaciones.isValid()) {
                diaI = QString::number(fechaInicioVacaciones.day());
                mesI = QString::number(fechaInicioVacaciones.month());
                annoI = QString::number(fechaInicioVacaciones.year());
            }

            QDate fechaFinVacaciones = QDate::fromString(datosConstancia.FechaFinalVacaciones, "dd/MM/yyyy");
            QString diaF;
            QString mesF;
            QString annoF;
            if (fechaFinVacaciones.isValid()) {
                diaF = QString::number(fechaFinVacaciones.day());
                mesF = QString::number(fechaFinVacaciones.month());
                annoF = QString::number(fechaFinVacaciones.year());
            }

            int dias = fechaInicioVacaciones.daysTo(fechaFinVacaciones);

            QString diasEntreFechas = QString::number(dias);

            QDate diaSiguienteFin = fechaFinVacaciones.addDays(1);

            QString diaSF;
            QString mesSF;
            QString annoSF;
            if (diaSiguienteFin.isValid()) {
                diaSF = QString::number(diaSiguienteFin.day());
                mesSF = QString::number(diaSiguienteFin.month());
                annoSF = QString::number(diaSiguienteFin.year());
            }

            ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.SolicitudVacacionesAprobada(formattedActualDate, datosConstancia.Nombre, datosConstancia.Apellido, datosConstancia.FechaEnvio, diaI, mesI, annoI, diaF, mesF, annoF, diasEntreFechas, diaSF, mesSF, annoSF));
            constancia->show();
        } else if(estadoSolicitud == "Rechazada"){
            QEventLoop loop;
            Message solicitudMessage;
            solicitudMessage.type = Message::ConsultarSolicitudVacaciones;
            QByteArray datagrama(2, '\0');
            datagrama.replace(0, 2, (std::to_string(idSolicitud).c_str()));
            solicitudMessage.data = datagrama;

            connect(this, &EstadoSolicitudesHechas::leidas_solicitudes, &loop, &QEventLoop::exit);

            client.sendMessage(solicitudMessage);

            loop.exec();

            this->hide();

            QDate fechaActual = QDate::currentDate();
            QString formattedActualDate = fechaActual.toString("dd/MM/yyyy");
            QDate fechaInicioVacaciones = QDate::fromString(datosConstancia.FechaInicioVacaciones, "dd/MM/yyyy");
            QString diaI;
            QString mesI;
            QString annoI;
            if (fechaInicioVacaciones.isValid()) {
                diaI = QString::number(fechaInicioVacaciones.day());
                mesI = QString::number(fechaInicioVacaciones.month());
                annoI = QString::number(fechaInicioVacaciones.year());
            }

            QDate fechaFinVacaciones = QDate::fromString(datosConstancia.FechaFinalVacaciones, "dd/MM/yyyy");
            QString diaF;
            QString mesF;
            QString annoF;
            if (fechaFinVacaciones.isValid()) {
                diaF = QString::number(fechaFinVacaciones.day());
                mesF = QString::number(fechaFinVacaciones.month());
                annoF = QString::number(fechaFinVacaciones.year());
            }


            ConstanciaUsuarios* constancia = new ConstanciaUsuarios(this, this, informe.SolicitudVacacionesRechazada(formattedActualDate, datosConstancia.Nombre, datosConstancia.Apellido, datosConstancia.FechaEnvio, diaI, mesI, annoI, diaF, mesF, annoF));
            constancia->show();
        } else {
            QMessageBox::information(this, "Detalles del Usuario", detalles);
        }
    }
}


void EstadoSolicitudesHechas::agregarUsuariosDePrueba()
{
    Usuario usuario;
    usuario.nombre = this->username;

    generarWidgetsSolicitudes(usuario);
}

QList<Usuario> EstadoSolicitudesHechas::obtenerUsuariosDePrueba()
{
    QList<Usuario> usuarios;
    usuarios.append(Usuario{"Juan D.", "29 Julio del 2023"});
    // usuarios.append(Usuario{"Laura G.", "14 Diciembre del 2023"});
    // usuarios.append(Usuario{"Usuario 1", "1 Enero del 2023"});

    return usuarios;
}

void EstadoSolicitudesHechas::generarWidgetsSolicitudes(const Usuario& usuario)
{
    QList<Solicitud> solicitudes = obtenerSolicitudesDeUsuario(usuario);

    for (const Solicitud& solicitud : solicitudes) {
        // Crear un nuevo QListWidgetItem
        QListWidgetItem* listItem = new QListWidgetItem();

        // Crear un widget para el contenido de cada elemento de la lista
        QWidget* listItemWidget = new QWidget();

        // Crear un layout vertical para el widget del elemento de la lista
        QVBoxLayout* listItemLayout = new QVBoxLayout(listItemWidget);

        QLabel* solicitudLabel = new QLabel(QString("<b>Solicitud de %1</b>").arg(usuario.nombre));
        solicitudLabel->setStyleSheet("font-weight: bold; font-size: 15px;");
        listItemLayout->addWidget(solicitudLabel);

        QLabel* nombreSolicitudLabel = new QLabel(QString("<i>Nombre de Solicitud:</i> %1").arg(solicitud.nombre));
        nombreSolicitudLabel->setStyleSheet("color: #555555;");
        QLabel* idSolicitudLabel = new QLabel(QString("<i>ID: %1</i>").arg(solicitud.id));
        idSolicitudLabel->setStyleSheet("color: #555555;");
        QLabel* estadoSolicitudLabel = new QLabel(QString("Estado: %1").arg(solicitud.estado));
        estadoSolicitudLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(solicitud.color));
        listItemLayout->addWidget(nombreSolicitudLabel);
        listItemLayout->addWidget(idSolicitudLabel);
        listItemLayout->addWidget(estadoSolicitudLabel);

        QPushButton* detallesButton = new QPushButton("Generar Informe");
        detallesButton->setObjectName("detallesButton");
        detallesButton->setProperty("nombreSolicitud", solicitud.nombre);
        detallesButton->setProperty("idSolicitud", solicitud.id);
        detallesButton->setProperty("estadoSolicitud", solicitud.estado);
        detallesButton->setProperty("colorSolicitud", solicitud.color);
        detallesButton->setFixedWidth(200);  // Ajusta el ancho del botón a 200 píxeles
        detallesButton->setStyleSheet("background-color: lightgray; padding: 6px 12px; font-weight: bold; border: none; border-radius: 4px; ");
        listItemLayout->addWidget(detallesButton);
        listItemLayout->addWidget(detallesButton, 0, Qt::AlignCenter);  // Alinea el botón al centro

        // Se Agrega separador
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("margin: 8px 0;");
        listItemLayout->addWidget(line);

        // Establecer el layout en el widget del elemento de la lista
        listItemWidget->setLayout(listItemLayout);

        // Establecer el widget del elemento de la lista en el QListWidgetItem
        listItem->setSizeHint(listItemWidget->sizeHint());
        listaWidget->addItem(listItem);
        listaWidget->setItemWidget(listItem, listItemWidget);
    }
}


QList<Solicitud> EstadoSolicitudesHechas::obtenerSolicitudesDeUsuario(const Usuario& usuario)
{
    QEventLoop loop;
    Message solicitudMessage;
    solicitudMessage.type = Message::ConsultarEstadoSolicitudesHechas;
    QByteArray datagrama(10, '\0');
    datagrama.replace(0, qMin(this->username.length(), 10), this->username.toUtf8());
    solicitudMessage.data = datagrama;

    connect(this, &EstadoSolicitudesHechas::leidas_solicitudes, &loop, &QEventLoop::exit);

    client.sendMessage(solicitudMessage);

    loop.exec();
    return solicitudes;
}

void EstadoSolicitudesHechas::conectarBotonesDetalles()
{
    QList<QPushButton*> detallesButtons = findChildren<QPushButton*>("detallesButton");
    for (QPushButton* button : detallesButtons) {
        connect(button, &QPushButton::clicked, this, &EstadoSolicitudesHechas::verDetallesUsuario);
    }
}

QString EstadoSolicitudesHechas::obtenerColorEstado(const QString &estado)
{
    if (estado == "Aprobada") {
        return "green";
    } else if (estado == "Rechazada") {
        return "red";
    } else if (estado == "Pendiente") {
        return "orange";
    } else {
        return "yellow";
    }
}

void EstadoSolicitudesHechas::on_acceptButton_clicked()
{
    // QMessageBox::information(this, "Solicitud a revisión", "La solicitud ha sido enviada para su revisión.");
}

void EstadoSolicitudesHechas::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}

void EstadoSolicitudesHechas::solicitudes_recibidas(const Message& message) {
    if (message.type == Message::ConsultarEstadoSolicitudesHechas) {
        qDebug() << "[UI] Received solicitudes from server";
        // Se deserializan los datos recibidos
        QString solicitudes = QString::fromUtf8(message.data);
        qDebug() << "[UI] solicitudes: " << solicitudes;
        // Se separan los datos, \t separador de datos, \n separador de solicitudes
        QStringList solicitudesList = solicitudes.split("\n", Qt::SkipEmptyParts);
        qDebug() << "[UI] solicitudesList: " << solicitudesList;
        // Se separan los datos de cada solicitud
        for (const QString& solicitud : solicitudesList) {
            QStringList solicitudList = solicitud.split("\t");
            qDebug() << "[UI] solicitudList: " << solicitudList;
            // Se crea una nueva solicitud
            Solicitud nuevaSolicitud;
            nuevaSolicitud.nombre = solicitudList[0];
            nuevaSolicitud.id = solicitudList[4].toLongLong();
            nuevaSolicitud.estado = solicitudList[3];
            nuevaSolicitud.color = obtenerColorEstado(nuevaSolicitud.estado);
            // Se agrega la solicitud a la lista de solicitudes
            this->solicitudes.append(nuevaSolicitud);
        }
        emit leidas_solicitudes(0);
    } else if(message.type == Message::ConsultarConstancia){
        qDebug() << "[UI] Received respuesta consulta de constancia from server";
        datosConstancia.Nombre = QString::fromUtf8(message.data.mid(10,15));
        datosConstancia.Apellido = QString::fromUtf8(message.data.mid(25, 15).constData());
        datosConstancia.Cedula = QString::fromUtf8(message.data.mid(40, 11).constData());
        datosConstancia.SalarioBruto = QString::fromUtf8(message.data.mid(51, 10).constData());
        datosConstancia.SalarioNeto = QString::fromUtf8(message.data.mid(61, 10).constData());
        datosConstancia.FechaInicioLabores = QString::fromUtf8(message.data.mid(71, 10).constData());
        datosConstancia.FechaEnvio = QString::fromUtf8(message.data.mid(81, 10).constData());
        datosConstancia.FechaAprobacion = QString::fromUtf8(message.data.mid(91, 10).constData());
        datosConstancia.Puesto = QString::fromUtf8(message.data.mid(101, 30).constData());
        emit leidas_solicitudes(0);
    } else if(message.type == Message::ConsultarSolicitudVacaciones){
        qDebug() << "[UI] Received respuesta consulta de solicitude de vacaciones from server";
        datosConstancia.Nombre = QString::fromUtf8(message.data.mid(10,15));
        datosConstancia.Apellido = QString::fromUtf8(message.data.mid(25, 15).constData());
        datosConstancia.FechaEnvio = QString::fromUtf8(message.data.mid(40, 10).constData());
        datosConstancia.FechaInicioVacaciones = QString::fromUtf8(message.data.mid(50, 10).constData());
        datosConstancia.FechaFinalVacaciones = QString::fromUtf8(message.data.mid(60, 10).constData());
        emit leidas_solicitudes(0);
    }
}

void EstadoSolicitudesHechas::on_logout_clicked()
{
    QMessageBox::StandardButton confirmarLogout;
    confirmarLogout = QMessageBox::question(this, "Cerrar sesión ", "¿Esta seguro que desea cerrar sesión?", QMessageBox::Yes | QMessageBox::No);
    if(confirmarLogout == QMessageBox::Yes){
        Client& client = Client::getInstance();  // Obtener la instancia del singleton del cliente
        close();
        client.disconnectFromServer();
        QEventLoop closed;
        connect(&client, &Client::disconnected, &closed, &QEventLoop::quit);
        closed.exec();
        this->deleteLater();
    }
}
