#include "Server.h"
#include <QSslSocket>
#include <QDataStream>
#include <string>

Server::Server(QObject *parent, DbManager *dbmanager, quint16 port) :
    QTcpServer(parent),
    dbmanager(dbmanager),
    port(port)
{
}

Message Server::handleConsultaSupervisor(quint8 estado, QString username)
{
    Message response;
    quint8 tipoDeRegistro = 0; // 0 es para las solicitudes de constancia
    QList<QString> listaDeSolicitudesARevisar = this->dbmanager->consultarSolicitudes("%", "%", QString::number(estado), "%", "%", username);
    QString solicitudesARevisar = "";

    for (qsizetype i = 0; i < listaDeSolicitudesARevisar.size(); i++) {
        solicitudesARevisar.append(QString::number(tipoDeRegistro) + "\t");
        solicitudesARevisar.append(listaDeSolicitudesARevisar.at(i));
        solicitudesARevisar.append("\n");
    }
    tipoDeRegistro = 1; // 1 es para las solicitudes de vacaciones

    QList<QString> listaDeSolicitudesVacaciones = this->dbmanager->consultarSolicitudesVacaciones("%", QString::number(estado), username);
    for (qsizetype i = 0; i < listaDeSolicitudesVacaciones.size(); i++) {
        solicitudesARevisar.append(QString::number(tipoDeRegistro) + "\t");
        solicitudesARevisar.append(listaDeSolicitudesVacaciones.at(i));
        solicitudesARevisar.append("\n");
    }
    response.data = solicitudesARevisar.toUtf8();
    response.type = Message::ConsultarSolicitudesPendientes;
    
    return response;
}

Message Server::handleConsultaComprobantes(QString username) {
    Message response;
    QList<QString> listaDeComprobantes = this->dbmanager->consultarComprobantesPago(username);
    QString comprobantes = "";

    for (qsizetype i = 0; i < listaDeComprobantes.size(); i++) {
        comprobantes.append(listaDeComprobantes.at(i));
        comprobantes.append("\n");
    }

    response.data = comprobantes.toUtf8();
    response.type = Message::ConsultarComprobantesDePago;

    return response;
}

bool Server::startServer()
{
    if(!this->listen(QHostAddress::AnyIPv4, this->port)) // QHostAddress::Any es para escuchar en cualquier puerto
    {
        // No se pudo iniciar el servidor, muestra un mensaje de error
        qCritical() << "Could not start server";
        return false;
    }
    else
    {
        // El servidor se ha iniciado correctamente, muestra el puerto al que se está escuchando
        qDebug() << "Listening to port " << port << "...";
        return true;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // Tenemos una nueva conexión
    // Muestra en la consola el descriptor del socket que se está conectando
    qDebug() << "\n" << socketDescriptor << "Connecting...";
    // Maneja la nueva conexión en un método separado
    handleNewConnection(socketDescriptor);
}

void Server::handleNewConnection(qintptr socketDescriptor)
{
    // Crea un nuevo socket TCP para la conexión entrante
    QSslSocket* socket = new QSslSocket(this);
    // Asigna el descriptor de socket al nuevo socket TCP
    socket->setSocketDescriptor(socketDescriptor);
    // Conecta la señal readyRead() del socket a la ranura onSocketReadyRead()
    connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    // Conecta la señal disconnected() del socket directamente a su ranura deleteLater()
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslError(QList<QSslError>)));
    socket->setPrivateKey("../ServidorFS/server.key", QSsl::Rsa);
    socket->setLocalCertificate("../ServidorFS/server.crt");
    socket->startServerEncryption();
     // Se obtiene la información del cliente para poder imprimirlo en la consola
    qDebug() << "New connection from " << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void Server::onSocketReadyRead()
{
    // Obtiene el socket TCP que ha emitido la señal
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());

    if (socket)
    {
        // Crea un flujo de datos de entrada asociado al socket
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_0);
        // Lee los datos disponibles en el flujo hasta que no haya más datos por leer
        while (!in.atEnd())
        {
            // Crea un objeto Message para almacenar el mensaje recibido
            Message message;

            // Deserializa el mensaje desde el flujo de datos
            in >> message;

            // Maneja el mensaje recibido
            handleReceivedMessage(socket, message);
        }
    }
}


void Server::sendMessage(QSslSocket* socket, const Message& message)
{
    // Crea un arreglo de bytes para almacenar los datos serializados
    QByteArray block;

    // Crea un flujo de datos de salida asociado al arreglo de bytes
    QDataStream out(&block, QIODevice::WriteOnly);

    // Establece la versión del protocolo de serialización del flujo de datos
    out.setVersion(QDataStream::Qt_5_0);

    // Serializa el mensaje en el flujo de datos
    out << message;

    // Escribe el arreglo de bytes en el socket para enviar el mensaje al cliente
    socket->write(block);
}


void Server::handleReceivedMessage(QSslSocket* socket, const Message& message)
{
    Message::MessageType type = message.type;
    QByteArray recievedDatagram = message.data;
    QString username, name, lastname, email, phone, personalId, job,
            initDate, grossPay, netPay;
    quint32 transactionID = message.transactionID;

    // Se crea un objeto de mensaje llamado "response"
    Message response;

    int datosPersonales, datosLaborales;
    bool userCreated = false;
    QString fechaSolicitud;
    QString fechaInicioVacaciones;
    QString fechaFinalVacaciones;
    QString anotacion;
    QString area;
    quint8 tipoRegistro;
    quint64 idConstancia;
    bool yaPidio = false;
    quint8 estado;
    int error;
    qDebug() << "[DB] Received message: Type = " << type << " from " <<
     socket->peerAddress().toString() << ":" << socket->peerPort();

    // Handle the message based on its type
    switch (type)
    {
    case Message::Text: // Maneja texto
        qDebug() << "[DB] Received text message";
        break;
    case Message::CrearUsuarioDB: // Crea un usuario
        qDebug() << "Received crear usuario DB";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        name = QString::fromUtf8(recievedDatagram.mid(10, 15).constData());
        lastname = QString::fromUtf8(recievedDatagram.mid(25, 15).constData());
        email = QString::fromUtf8(recievedDatagram.mid(40, 31).constData());
        phone = QString::fromUtf8(recievedDatagram.mid(71, 8).constData());
        personalId = QString::fromUtf8(recievedDatagram.mid(79, 9).constData());
        job = QString::fromUtf8(recievedDatagram.mid(88, 15).constData());
        initDate = QString::fromUtf8(recievedDatagram.mid(103, 10).constData());
        grossPay = QString::fromUtf8(recievedDatagram.mid(113, 10).constData());
        netPay = QString::fromUtf8(recievedDatagram.mid(123,10).constData());
        area = QString::fromUtf8(recievedDatagram.right(15).constData());
        response.type = Message::CrearUsuarioDB;
        qDebug() << area;
        datosPersonales = this->dbmanager->agregarDatosPersonales(username,
                                name, lastname, email, phone, personalId);
        datosLaborales = this->dbmanager->agregarDatosLaborales(username, job,
                                initDate, grossPay.toInt(), netPay.toInt(), 0, area);
        if (datosPersonales == EXIT_SUCCESS && datosLaborales == EXIT_SUCCESS) {
            response.data.append("0");
        } else {
            response.data.append("1");
        }
        qDebug() << "Sent Crear Usuario en BDD";
        break;
    case Message::EditarUsuarioDB:
            qDebug() << "Received Editar Usuario DB";
            username = QString::fromUtf8(recievedDatagram.left(10).constData());
            name = QString::fromUtf8(recievedDatagram.mid(10, 15).constData());
            lastname = QString::fromUtf8(recievedDatagram.mid(25, 15).constData());
            email = QString::fromUtf8(recievedDatagram.mid(40, 31).constData());
            phone = QString::fromUtf8(recievedDatagram.mid(71, 9).constData());
            personalId = QString::fromUtf8(recievedDatagram.mid(80, 11).constData());
            job = QString::fromUtf8(recievedDatagram.mid(91, 30).constData());
            grossPay = QString::fromUtf8(recievedDatagram.mid(121, 10).constData());
            netPay = QString::fromUtf8(recievedDatagram.right(10).constData());
            response.type = Message::EditarUsuarioDB;
            datosPersonales = this->dbmanager->actualizarDatosPersonales(username,
                                                                         name, lastname, email, phone, personalId);
            datosLaborales = this->dbmanager->actualizarDatosLaborales(username, job,
                                                                       grossPay, netPay);
            if (datosPersonales == EXIT_SUCCESS && datosLaborales == EXIT_SUCCESS) {
                response.data.append("0");
            } else {
                response.data.append("1");
            }
            qDebug() << "Sent Editar Usuario en BDD";
            break;
    case Message::ConsultarSalarioBase: // Consulta el salarioBase
        qDebug() << "Received Consulta Salario Base";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.type = Message::ConsultarSalarioBase;
        response.data = this->dbmanager->consultarSalarioBase(username);
        qDebug() << "Sent Consulta Salario Base";
        break;
    case Message::SolicitudVacaciones: // Ingresa la solicitud de vacaciones del usuario a la base de datos
        qDebug() << "[DB] Received Solicitud de Vacaciones";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        fechaSolicitud = QString::fromUtf8(recievedDatagram.mid(10,10));
        fechaInicioVacaciones = QString::fromUtf8(recievedDatagram.mid(20,10).constData());
        fechaFinalVacaciones = QString::fromUtf8(recievedDatagram.right(10));
        response.type = Message::SolicitudVacaciones;
        error = this->dbmanager->agregarSolicitudVacaciones(username, "N/A",fechaSolicitud, fechaInicioVacaciones, fechaFinalVacaciones);
        response.data.append(std::to_string(error).c_str());
        break;
    case Message::ConsultarSaldoVacaciones:
        qDebug() << "[DB] Received Consulta Saldo de Vacaciones";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.type = Message::ConsultarSaldoVacaciones;
        response.data = this->dbmanager->consultarSaldoVacaciones(username);
        qDebug() << "[DB] Sent consulta saldo de vacaciones: ";
        break;
    case Message::SolicitudConstanciaSalarial:
        qDebug() << "[DB] Received Solicitud de Constancia Salarial";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        fechaSolicitud = QString::fromUtf8(recievedDatagram.mid(10,10));
        response.type = Message::SolicitudConstanciaSalarial;
        yaPidio = this->dbmanager->pidioConstancia(username, 2);
        if (yaPidio) {
            error = -1;
        } else {
            error = this->dbmanager->agregarConstancia(username, fechaSolicitud, 0, 0, 2);
        }
        response.data.append(std::to_string(error).c_str());
        break;
    case Message::SolicitudConstanciaLaboral:
        qDebug() << "[DB] Received Solicitud de Constancia Laboral";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        fechaSolicitud = QString::fromUtf8(recievedDatagram.mid(10,10));
        response.type = Message::SolicitudConstanciaSalarial;
        yaPidio = this->dbmanager->pidioConstancia(username, 1);
        if (yaPidio) {
            error = -1;
        } else {
            error = this->dbmanager->agregarConstancia(username, fechaSolicitud, 0, 0, 1);
        }
        response.data.append(std::to_string(error).c_str());
        break;
    case Message::ConsultarEstadoSolicitudesHechas:
        qDebug() << "[DB] Received Consulta Estado de Solicitudes Hechas";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.type = Message::ConsultarEstadoSolicitudesHechas;
        response.data = this->dbmanager->consultarEstadoDeSolicitudes(username).toUtf8();
        qDebug() << "[DB] Sent consulta estado de solicitudes hechas para " << username;
        break;
    case Message::ConsultarSolicitudesPendientes:
        qDebug() << "[DB] Received Consulta de Solicitudes Pendientes";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        estado = QString::fromUtf8(recievedDatagram.right(1).constData()).toUShort();
        response = this->handleConsultaSupervisor(estado, username);
        qDebug() << "[DB] Sent consulta solicitudes pendientes: ";
        break;
    case Message::ModificarEstadoConstancia:
        qDebug() << "[DB] Received Modificar Estado de Constancia";
        tipoRegistro = QString::fromUtf8(recievedDatagram.left(1).constData()).toUShort();
        idConstancia = QString::fromUtf8(recievedDatagram.mid(1, 10).constData()).toULongLong();
        estado = QString::fromUtf8(recievedDatagram.right(1).constData()).toUShort();
        response.type = Message::ModificarEstadoConstancia;
        error = this->dbmanager->cambiarEstadoSolicitud(idConstancia, DbManager::estadoSolicitud(estado), tipoRegistro);
        response.data.append(std::to_string(error).c_str());
        break;
    case Message::ConsultarConstancia:
        qDebug() << "[DB] Received Consultar constancia";
        idConstancia = QString::fromUtf8(recievedDatagram.left(2).constData()).toULongLong();
        response.data = this->dbmanager->consultarConstancia(idConstancia);
        qDebug() << "[DB] Datos al Consultar constancia: ";
        response.type = Message::ConsultarConstancia;
        break;
    case Message::ConsultarAnotacionesAlExpediente:
        qDebug() << "[DB] Received Consultar anotaciones al expediente";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.data = this->dbmanager->consultarAnotacionesExpediente(username).toUtf8();
        qDebug() << "[DB] Solicitud tramitada, enviando respuesta...";
        response.type = Message::ConsultarAnotacionesAlExpediente;
        break;
    case Message::AgregarAnotacionAlExpediente:
        qDebug() << "[DB] Received Agregar anotacion al expediente";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        anotacion = QString::fromUtf8(recievedDatagram.right(200).constData());
        response.type = Message::AgregarAnotacionAlExpediente;
        error = this->dbmanager->actualizarAnotacionesExpediente(username, anotacion);
        response.data.append(error); // int error
        break;
    case Message::ConsultarSolicitudVacaciones:
        qDebug() << "[DB] Received Consultar solicitud de vacaciones";
        idConstancia = QString::fromUtf8(recievedDatagram.left(2).constData()).toULongLong();
        response.data = this->dbmanager->consultarSolicitudVacaciones(idConstancia);
        qDebug() << "[DB] Datos al Consultar solicitud de vacaciones: ";
        response.type = Message::ConsultarSolicitudVacaciones;
        break;
    case Message::ConsultarListaDeUsuarios:
        qDebug() << "[DB] Received Consultar lista de usuarios";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.data = this->dbmanager->consultarUsuariosAnotaciones(username).toUtf8();
        qDebug() << "[DB] Datos al Consultar lista de usuarios: ";
        response.type = Message::ConsultarListaDeUsuarios;
        break;
    case Message::SolicitarAnotacionesAlExpediente:
        qDebug() << "[DB] Received Solicitud anotaciones al expediente";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        fechaSolicitud = QString::fromUtf8(recievedDatagram.mid(10,10));
        error = this->dbmanager->agregarConstancia(username, fechaSolicitud, 0, 0, 3);
        response.type = Message::SolicitarAnotacionesAlExpediente;
        response.data.append(std::to_string(error).c_str());
        break;
    case Message::EnlistarUsuarios:
        {qDebug() << "[DB] Received Solicitud de consultar Usuarios";
        QList<QString> listaDeUsuarios = this->dbmanager->listarUsuarios();
        QString usuarios = "";
        for (qsizetype i = 0; i < listaDeUsuarios.size(); i++) {
            usuarios.append(listaDeUsuarios.at(i));
            usuarios.append("\n");
        }
        response.type = Message::EnlistarUsuarios;
        response.data = usuarios.toUtf8();
        break;}
    case Message::ConsultarDatosDeUsuario:
        qDebug() << "Received consulta de datos personales";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.type = Message::ConsultarDatosDeUsuario;
        response.data = this->dbmanager->consultarDatosDeUsuario(username);
        qDebug() << "Sent Consulta de datos personales: " << response.data;
        break;
    case Message::ConsultarComprobantesDePago: // Consulta el salarioBase
        qDebug() << "Received Consulta Comprobantes de Pago";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response = this->handleConsultaComprobantes(username);
        qDebug() << "Sent Consulta Comprobantes de Pago";
        break;
    case Message::EliminarUsuarioDB:
        {qDebug() << "Received eliminar usuario DB";
        username = QString::fromUtf8(recievedDatagram.left(10).constData());
        response.type = Message::EliminarUsuarioDB;
        int usuarioEliminado = this->dbmanager->eliminarUsuario(username);
        if (usuarioEliminado == EXIT_SUCCESS) {
            response.data.append("0");
        } else {
            response.data.append("1");
        }
        qDebug() << "Sent usuario eliminado en BDD";
        break;}
    default:
        qDebug() << "[DB] Unknown message type";
        break;
    }
    
    // Se asigna el TransactionID al mensaje de respuesta
    response.transactionID = transactionID;

    qDebug() << "[DB] Sending response. ID = " << transactionID;
    // Se llama a la función "sendMessage" para enviar el mensaje al socket
    sendMessage(socket, response);
}

void Server::sslError(const QList<QSslError> &errors) {
    qDebug() << "SSL Errors: " << errors;
}
