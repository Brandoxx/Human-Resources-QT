#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>    // Libreria para manipular matrices de bytes
#include <QDataStream>   // Libreria para la serialización de datos
#include <QString>

class Message
{
public:
    enum MessageType {
        Text,   // Tipo de mensaje: Texto
        Autenticacion,
        CrearUsuarioFS,
        CrearUsuarioDB,
        EditarUsuarioDB,
        EliminarUsuarioDB,
        ConsultarSalarioBase,
        ConsultarSaldoVacaciones,
        SolicitudVacaciones,
        SolicitudConstanciaSalarial,
        SolicitudConstanciaLaboral,
        ConsultarEstadoSolicitudesHechas,
        ConsultarAnotacionesAlExpediente,
        ConsultarComprobantesDePago,
        ConsultarSolicitudesPendientes,
        ModificarEstadoConstancia,
        AgregarAnotacionAlExpediente,
        ConsultarConstancia,
        EnlistarUsuarios,
        ConsultarDatosDeUsuario,
        ConsultarSolicitudVacaciones,
        ConsultarListaDeUsuarios,
        SolicitarAnotacionesAlExpediente,
        EliminarUsuarioFS
    };

    enum Niveles_de_acceso {
        EMPLEADO = 1,
        SUPERVISOR,
        RRHH,
        ADMIN_USUARIOS,
        ADMIN_CONFIG,
        DEBUG,
    };

     enum estadoSolicitud {
        PENDIENTE,
        APROBADASUPERVISOR,
        APROBADA,
        RECHAZADA };

    QByteArray data;    // Datos del mensaje
    MessageType type;   // Tipo de mensaje
    quint32 transactionID;

    Message()
        : type(Text)    // Inicializa el tipo de mensaje como Texto por defecto
    {
    }

    /**
     * @brief Constructor de la clase Message.
     * @param data Los datos del mensaje.
     * @param type El tipo de mensaje.
     */
    Message(const QByteArray& data, MessageType type)
        : data(data), type(type)
    {
    }

    /**
     * @brief Sobrecarga del operador << para la serialización de Message en un flujo de datos.
     * @param stream El flujo de datos de salida.
     * @param message El objeto Message a serializar.
     * @return El flujo de datos de salida después de la serialización.
     */
    friend QDataStream& operator<<(QDataStream& stream, const Message& message)
    {
        stream << message.transactionID;
        stream << message.data;    // Serializa los datos en el flujo de datos
        stream << static_cast<quint32>(message.type);    // Serializa el tipo de mensaje como un entero de 32 bits
        return stream;
    }

    /**
     * @brief Sobrecarga del operador >> para la deserialización de Message desde un flujo de datos.
     * @param stream El flujo de datos de entrada.
     * @param message El objeto Message a deserializar.
     * @return El flujo de datos de entrada después de la deserialización.
     */
    friend QDataStream& operator>>(QDataStream& stream, Message& message)
    {
        quint32 messageType;

        stream >> message.transactionID;
        stream >> message.data;    // Deserializa los datos desde el flujo de datos
        stream >> messageType;    // Deserializa el tipo de mensaje como un entero de 32 bits
        message.type = static_cast<MessageType>(messageType);    // Convierte el entero en el tipo de mensaje correspondiente

        // Si los datos se interpretan como una cadena de caracteres Unicode, se realiza una conversión adecuada
        if (stream.version() >= QDataStream::Qt_5_7) {
            message.data = QString::fromUtf8(message.data).toUtf8();
        }

        return stream;
    }
};

#endif // MESSAGE_H


