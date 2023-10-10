#include "ActualizarUsuarios.h"
#include "ui_ActualizarUsuarios.h"
#include "ActualizarDatosUsuario.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <iostream>

ActualizarUsuarios::ActualizarUsuarios(QWidget *parent, AdministradorMenu *AdministradorMenuParent, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ActualizarUsuarios),
    parentWindow(parent),
    AdministradorMenuParentWindow(AdministradorMenuParent),
    client(client)
{
    ui->setupUi(this);
    // Se obtiene una referencia al QVBoxLayout existente en el diseño principal
    QVBoxLayout *verticalLayout = ui->verticalLayout_2; // Cambio aquí al QVBoxLayout deseado

    // Se crea un QHBoxLayout adicional para centrar listaWidget
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    // horizontalLayout->setAlignment(Qt::AlignCenter);

    // Se agrega listaWidget al QHBoxLayout
    listaWidget = new QListWidget();
    horizontalLayout->addWidget(listaWidget);

    // Se agrega el QHBoxLayout al QVBoxLayout
    verticalLayout->addLayout(horizontalLayout);

    connect(&client, &Client::dataReceived, this, &ActualizarUsuarios::procesarUsuarios);

    this->mostrarWidgets();
}

ActualizarUsuarios::~ActualizarUsuarios()
{
    delete ui;
}

void ActualizarUsuarios::editarUsuario(QString username) {
    this->hide();
    ActualizarDatosUsuario* ventanaActualizarDatosUsuario =
        new ActualizarDatosUsuario(this, client.getInstance(), username);
    ventanaActualizarDatosUsuario->show();
}

void ActualizarUsuarios::on_updateUserButton_clicked()
{

}

void ActualizarUsuarios::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void ActualizarUsuarios::on_logout_clicked()
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

void ActualizarUsuarios::mostrarWidgets() {
    // Pedir lista de solicitudes a serverBD
    QEventLoop loop;
    QString username, fullname;

    Message request;
    request.type = Message::EnlistarUsuarios;
    // Agregar cada solicitud a un vector de solicitudes

    connect(this, &ActualizarUsuarios::usuariosProcesados, &loop, &QEventLoop::quit);

    client.sendMessage(request);

    loop.exec();

    for (size_t i = 0; i < this->usuarios.size(); i++) {

        QStringList usuario = this->usuarios[i].split("\t");
        username = usuario[0];
        fullname = usuario[1];

        // Crea el widget personalizado para cada usuario
        QWidget* widget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(widget);


        // Crea una etiqueta para mostrar la información del usuario
        QLabel* infoLabel = new QLabel(QString("%1\t%2").arg(username).arg(fullname));
        infoLabel->setObjectName("infoLabel"); // Asigna un nombre de objeto a la etiqueta
        layout->addWidget(infoLabel);

        // Crea los botones para ver detalles, aceptar y rechazar
        QPushButton* detallesButton = new QPushButton("Editar");

        // Establece los colores de fondo a los botones
        detallesButton->setStyleSheet("background-color: lightGray; width: 20px;");

        // Conecta la señal de clic de los botones con sus respectivas funciones, y establece el índice como userData
        connect(detallesButton, &QPushButton::clicked, this, [=]() { editarUsuario(username); });

        // Agrega los botones al diseño horizontal
        layout->addWidget(detallesButton);

        // Establece el widget personalizado como el elemento de lista
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(widget->sizeHint());
        listaWidget->addItem(item);
        listaWidget->setItemWidget(item, widget);
    }
}

void ActualizarUsuarios::procesarUsuarios(const Message& message)
{
    QString data = QString::fromUtf8(message.data);
    if (message.type == Message::EnlistarUsuarios) {
        if (message.data == "0") {
            QMessageBox::information(this, "Usuarios", "No hay usuarios en la DB.");
        } else {
            QStringList usuarios = QString(message.data).split("\n", Qt::SkipEmptyParts);
            for (QString usuario : usuarios) {
                this->usuarios.append(usuario);
            }
        }
        emit usuariosProcesados(0);
    }
}

