#include "RevisionSolicitudes.h"
#include "ui_RevisionSolicitudes.h"
#include <QMessageBox>
#include "Login.h"

RevisionSolicitudes::RevisionSolicitudes(QWidget *parent, Client& client, QString username, quint8 estadoSolicitud) :
    QWidget(parent, Qt::Window),
    ui(new Ui::RevisionSolicitudes),
    parentWindow(parent),
    client(client),
    username(username),
    estadoSolicitud(estadoSolicitud)
{
    ui->setupUi(this);
    // Se obtiene una referencia al QVBoxLayout existente en el diseño principal
    QVBoxLayout *verticalLayout = ui->verticalLayout_2; // Cambio aquí al QVBoxLayout deseado

    // Se crea un QHBoxLayout adicional para centrar listaWidget
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    // horizontalLayout->setAlignment(Qt::AlignCenter);

    // Se agrega listaWidget al QHBoxLayout
    listaWidget = new QListWidget();
    // Activar las barras de desplazamiento vertical y horizontal en QListWidget
    listaWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // listaWidget->setMaximumHeight(200); // Ajusta la altura máxima según sea necesario
    listaWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    horizontalLayout->addWidget(listaWidget);

    // Se agrega el QHBoxLayout al QVBoxLayout
    verticalLayout->addLayout(horizontalLayout);

    connect(&client, &Client::dataReceived, this, &RevisionSolicitudes::procesar_solicitudes);

    this->mostrarWidgets(estadoSolicitud);
}

void RevisionSolicitudes::verDetallesUsuario(int index) {
    if (index >= 0 && index < listaWidget->count()) {
        /**
         * QString solicitud = idSolicitud + "\t" + usuario + "\t" + fechaSolicitud +
                 "\t" + fechaAprobacion + "\t" + stringBruto + "\t" + stringNeto + "\t"
                  + tipoConstancia + "\t" + estado;
        */
        QStringList solicitud = this->solicitudes[index].split("\t");
        quint8 tipoRegistro = solicitud[0].toUInt();
        if (tipoRegistro == 0) {
            QString nombre = solicitud[2];
            QString fecha = solicitud[3];
            QString idSolicitud = solicitud[1];
            QString fechaAprobacion = solicitud[4];
            QString tipoConstancia;

            if (solicitud[6] == "1") {
                tipoConstancia = "Constancia Laboral";
            } else if (solicitud[6] == "2") {
                tipoConstancia = "Constancia Salarial";
            } else if(solicitud[6] == "3"){
                tipoConstancia = "Anotaciones al expediente";;
            }
            QMessageBox::information(this, "Detalles del Usuario", QString("Nombre: %1\nFecha: %2\nTipo de constancia: %3\nFecha de aprobación: %4").arg(nombre).arg(fecha).arg(tipoConstancia).arg(fechaAprobacion));

        } else { // Es una solicitud de vacaciones
            QString usuario = solicitud[1];
            QString motivo = solicitud[2];
            QString fechaSolicitud = solicitud[3];
            QString fechaInicio = solicitud[4];
            QString fechaFin = solicitud[5];
            QString idSolicitud = solicitud[6];

            QMessageBox::information(this, "Detalles del Usuario",
             QString("Usuario: %1\nMotivo: %2\nFecha de solicitud: %3\nFecha de inicio: %4\nFecha de fin: %5").arg(usuario).arg(motivo).arg(fechaSolicitud).arg(fechaInicio).arg(fechaFin));
        }
        // Mostrar detalles del usuario en un mensaje
    } else {
        QMessageBox::warning(this, "Error", "Índice inválido");
    }
}

void RevisionSolicitudes::aceptarUsuario(int index) {
    if (index >= 0 && index < listaWidget->count()) {
        QStringList solicitud = this->solicitudes[index].split("\t");
        quint8 tipoRegistro = solicitud[0].toUInt();
        QString idSolicitud;
        QString nombre;
        if (tipoRegistro == 0) {
            idSolicitud = solicitud[1];
            nombre = solicitud[2];
        } else { // Es una solicitud de vacaciones
            idSolicitud = solicitud[7];
            nombre = solicitud[1];
        }
        if(solicitud[7] == "3"){ // Si es revisar anotaciones, entonces no tiene que pasar por RRHH
            this->enviarModificarEstadoSolicitud(idSolicitud, this->estadoSolicitud + 2, tipoRegistro);
        } else {
            this->enviarModificarEstadoSolicitud(idSolicitud, this->estadoSolicitud + 1, tipoRegistro);
        }
        // Mostrar mensaje de solicitud aprobada
        QMessageBox::information(this, "Solicitud Aprobada", QString("La solicitud de %1 ha sido aprobada.").arg(nombre));
    } else {
        QMessageBox::warning(this, "Error", "Índice inválido");
    }
}

void RevisionSolicitudes::rechazarUsuario(int index) {
    if (index >= 0 && index < listaWidget->count()) {
        QStringList solicitud = this->solicitudes[index].split("\t");
        quint8 tipoRegistro = solicitud[0].toUInt();
        QString idSolicitud;
        QString nombre;
        if (tipoRegistro == 0) {
            idSolicitud = solicitud[1];
            nombre = solicitud[2];
        } else { // Es una solicitud de vacaciones
            idSolicitud = solicitud[7];
            nombre = solicitud[1];
        }
        this->enviarModificarEstadoSolicitud(idSolicitud, 3, tipoRegistro);
        // Mostrar mensaje de solicitud rechazada
        QMessageBox::information(this, "Solicitud Rechazada", QString("La solicitud de %1 ha sido rechazada.").arg(nombre));
    } else {
        QMessageBox::warning(this, "Error", "Índice inválido");
    }
}

void RevisionSolicitudes::mostrarWidgets(quint8 estadoSolicitud) {
    // Pedir lista de solicitudes a serverBD
    QByteArray datagrama(11, '\0');
    QEventLoop loop;
    QString nombre;
    QString fecha;
    QString tipoConstancia;

    Message request;
    request.type = Message::ConsultarSolicitudesPendientes;
    // Agregar cada solicitud a un vector de solicitudes
    datagrama.replace(0, 10, username.toUtf8());
    datagrama.replace(10, 1, QByteArray::number(estadoSolicitud));
    // Enviar area igual
    request.data = datagrama;

    connect(this, &RevisionSolicitudes::solicitudesProcesadas, &loop, &QEventLoop::quit);

    client.sendMessage(request);

    loop.exec();

    QObject::disconnect(this, &RevisionSolicitudes::solicitudesProcesadas, &loop, &QEventLoop::quit);

    for (qsizetype i = 0; i < this->solicitudes.size(); ++i) {
        QStringList solicitud = this->solicitudes[i].split("\t");
        if (solicitud[0].toUShort() == 0) {
            nombre = solicitud[2];
            fecha = solicitud[3];
            if (solicitud[7] == "1") {
                tipoConstancia = "Constancia Laboral";
            } else if (solicitud[7] == "2") {
                tipoConstancia = "Constancia Salarial";
            } else if (solicitud[7] == "3") {
                tipoConstancia = "Anotaciones al expediente";
            }
        } else {
            nombre = solicitud[1];
            fecha = solicitud[3];
            tipoConstancia = "Vacaciones";
        }

        // Crea el widget personalizado para cada usuario
        QWidget* widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(widget);

        // Crea una etiqueta para mostrar la información del usuario con una fuente elegante
        // QLabel* infoLabel = new QLabel(QString("De %1 en %2").arg(nombre).arg(fecha));
        QLabel* infoLabel = new QLabel(QString("Hola, solicitud de %1 para %2").arg(tipoConstancia).arg(nombre));
        infoLabel->setObjectName("infoLabel");
        QFont infoFont("Arial");
        infoFont.setPixelSize(12); // Establece el tamaño en píxeles
        infoFont.setWeight(QFont::Bold); // Establece el grosor de la fuente como negrita
        infoLabel->setFont(infoFont);
        layout->addWidget(infoLabel);


        // Crea un layout horizontal para los botones y los alinea al centro
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->setAlignment(Qt::AlignCenter);

        // Crea los botones para ver detalles, aceptar y rechazar con colores pasteles
        QPushButton* detallesButton = new QPushButton("Detalles");
        QPushButton* aceptarButton = new QPushButton("Aceptar");
        QPushButton* rechazarButton = new QPushButton("Rechazar");

        detallesButton->setObjectName("detallesButton");
        aceptarButton->setObjectName("aceptarButton");
        rechazarButton->setObjectName("rechazarButton");

        detallesButton->setStyleSheet("background-color: #E0E0E0; font-weight: bold; border-radius: 4px; color: #333333; border: none; padding: 8px 16px;");
        aceptarButton->setStyleSheet("background-color: #82C485; font-weight: bold; border-radius: 4px; color: white; border: none; padding: 8px 16px;");
        rechazarButton->setStyleSheet("background-color: #E57373; font-weight: bold; border-radius: 4px; color: white; border: none; padding: 8px 16px;");

        // Conecta la señal de clic de los botones con sus respectivas funciones, y establece el índice como userData
        connect(detallesButton, &QPushButton::clicked, this, [=]() { verDetallesUsuario(i); });
        connect(aceptarButton, &QPushButton::clicked, this, [=]() { aceptarUsuario(i); });
        connect(rechazarButton, &QPushButton::clicked, this, [=]() { rechazarUsuario(i); });

        // Agrega los botones al layout horizontal
        buttonLayout->addWidget(detallesButton);
        buttonLayout->addWidget(aceptarButton);
        buttonLayout->addWidget(rechazarButton);

        // Agrega el layout horizontal al layout vertical principal
        layout->addLayout(buttonLayout);

        // Crea una etiqueta para mostrar el tipo de constancia con una descripción elegante
        QLabel* infoLabel2 = new QLabel(QString("De %1 en %2").arg(nombre).arg(fecha));
        infoLabel2->setObjectName("infoLabel2");
        QFont infoFont2("Arial");
        infoFont2.setPixelSize(11); // Establece el tamaño en píxeles
        infoFont2.setStyleHint(QFont::Cursive); // Establece el estilo como cursiva
        infoLabel2->setFont(infoFont2);
        layout->addWidget(infoLabel2);

        // Crea una línea separadora con un color elegante
        QFrame* separatorLine = new QFrame();
        separatorLine->setObjectName("separatorLine");
        separatorLine->setFrameShape(QFrame::HLine);
        separatorLine->setFrameShadow(QFrame::Sunken);
        separatorLine->setLineWidth(2);
        separatorLine->setMidLineWidth(0);
        separatorLine->setStyleSheet("color: #CCCCCC;");
        layout->addWidget(separatorLine);

        // Establece el widget personalizado como el elemento de lista
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(widget->sizeHint());
        listaWidget->addItem(item);
        listaWidget->setItemWidget(item, widget);
    }
}

void RevisionSolicitudes::enviarModificarEstadoSolicitud(const QString &idSolicitud, quint8 estadoSolicitud, quint8 tipoRegistro)
{
    qint32 error = 0;
    QByteArray datagrama(12, '\0');
    QEventLoop loop;

    Message request;
    request.type = Message::ModificarEstadoConstancia;
    datagrama.replace(0, 1, QByteArray::number(tipoRegistro));
    datagrama.replace(1, 10, idSolicitud.toUtf8());
    datagrama.replace(11, 1, QByteArray::number(estadoSolicitud));

    request.data = datagrama;

    connect(this, &RevisionSolicitudes::solicitudesProcesadas, &loop, &QEventLoop::quit);

    client.sendMessage(request);

    error = loop.exec();

    if (error == 0) {
    } else {
        QMessageBox::warning(this, "Error", "No se pudo enviar la solicitud a revisión.");
    }

    QObject::disconnect(this, &RevisionSolicitudes::solicitudesProcesadas, &loop, &QEventLoop::quit);
}

RevisionSolicitudes::~RevisionSolicitudes()
{
    delete ui;
}

void RevisionSolicitudes::on_acceptButton_clicked()
{
    // QMessageBox::information(this, "Solicitud a revisión", "La solicitud ha sido enviada para su revisión.");
    // Arma el mensaje para modificar el estado de la solicitud
    // Envia el mensaje al servidor
    // Recibe la respuesta del servidor
    // Muestra el mensaje de respuesta
    // Cierra la ventana

    // Se obtiene el id de la solicitud seleccionada

}

void RevisionSolicitudes::on_cancelButton_clicked()
{
    parentWindow->show();
    this->close();
    this->~RevisionSolicitudes();
}

void RevisionSolicitudes::on_logout_clicked()
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

void RevisionSolicitudes::procesar_solicitudes(const Message& message)
{
    QString data = QString::fromUtf8(message.data);
    if (message.type == Message::ConsultarSolicitudesPendientes) {
        if (message.data == "0") {
            QMessageBox::information(this, "Solicitudes", "No hay solicitudes pendientes.");
        } else {
            QStringList solicitudes = QString(message.data).split("\n", Qt::SkipEmptyParts);
            for (QString solicitud : solicitudes) {
                this->solicitudes.append(solicitud);
            }
        }
        emit solicitudesProcesadas(0);
    } else if (message.type == Message::ModificarEstadoConstancia) {
        qint16 error = data.toInt();
        emit solicitudesProcesadas(error);
    }
}
