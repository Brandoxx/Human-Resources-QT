#include "AnotacionExpediente.h"
#include "ui_AnotacionExpediente.h"

#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>

AnotacionExpediente::AnotacionExpediente(QWidget *parent, Client& client, QString username) :
    QWidget(parent, Qt::Window),
    ui(new Ui::AnotacionExpediente),
    parentWindow(parent),
    client(client),
    username(username)
{
    ui->setupUi(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    QVBoxLayout* userLayout = new QVBoxLayout();
    QHBoxLayout* titleLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Lista de Usuarios");

    listaWidget = new QListWidget();
    listaWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listaWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    titleLayout->addWidget(titleLabel);
    userLayout->addWidget(listaWidget);
    verticalLayout->addLayout(titleLayout);
    verticalLayout->addLayout(userLayout);

    ui->verticalLayout_2->addLayout(verticalLayout);

    connect(&client, &Client::dataReceived, this, &AnotacionExpediente::messageReceived);
    
    requestUsers();
}

AnotacionExpediente::~AnotacionExpediente()
{
    delete ui;
}

void AnotacionExpediente::agregarUsuario(const QString& nombre, const QString& anotacion)
{
    // Crear el widget del usuario
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    QLabel* nameLabel = new QLabel(nombre);
    QLabel* annotationLabel = new QLabel(anotacion);
    annotationLabel->setWordWrap(true); // Permitir el ajuste automático de línea
    annotationLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft); // Alinear el texto en la parte superior izquierda

    QPushButton* button = new QPushButton("Anotaciones");

    layout->addWidget(nameLabel, 0, 0);  // Agregar la etiqueta de nombre en la primera columna y fila
    layout->addWidget(annotationLabel, 0, 1);  // Agregar la etiqueta de anotación en la primera columna y segunda fila
    layout->addWidget(button, 0, 2);  // Agregar el botón en la primera columna y tercera fila
    layout->setContentsMargins(10, 10, 10, 10); // Margen interno del layout
    layout->setSpacing(5); // Espacio entre los elementos del layout

    widget->setStyleSheet(
        "QWidget { background-color: #F2F2F2; border-radius: 5px; margin-bottom: 10px; border: 2px solid black; }"  // Agregar borde negro al widget del usuario
        "QLabel#nameLabel, QLabel#annotationLabel, QPushButton { border: none; }" // Quitar bordes para las etiquetas de nombre de usuario, anotación y botón
        "QLabel#nameLabel { font-weight: bold; }"
        "QPushButton { margin-left: auto; border: 2px solid black; }"
        );

    // Establecer los identificadores para las etiquetas
    nameLabel->setObjectName("nameLabel");
    annotationLabel->setObjectName("annotationLabel");

    // Crear un objeto UsuarioExpediente y asignar los valores
    UsuarioExpediente usuario;
    usuario.nombre = nombre;
    usuario.anotacion = anotacion;

    // Agregar el objeto UsuarioExpediente a un QListWidgetItem
    QListWidgetItem* item = new QListWidgetItem();
    item->setData(Qt::UserRole, QVariant::fromValue(usuario)); // Guardar el objeto UsuarioExpediente en los datos del elemento
    item->setSizeHint(widget->sizeHint());
    listaWidget->addItem(item);
    listaWidget->setItemWidget(item, widget);

    connect(button, &QPushButton::clicked, [item, this]() {
        UsuarioExpediente usuario = item->data(Qt::UserRole).value<UsuarioExpediente>();
        QString anotacionAnterior = usuario.anotacion;

        // Mostrar el cuadro de diálogo de entrada de texto
        bool ok;
        QInputDialog dialog(this);
        dialog.setInputMode(QInputDialog::TextInput);
        dialog.setWindowTitle("Anotar dato");
        dialog.setLabelText("Ingrese la nueva anotación:");
            dialog.setTextValue(anotacionAnterior);
        dialog.setTextEchoMode(QLineEdit::Normal);
        dialog.setOption(QInputDialog::UsePlainTextEditForTextInput); // Hacer que el cuadro de diálogo sea más grande

        if (dialog.exec() == QDialog::Accepted) {
            QString nuevaAnotacion = dialog.textValue();

            if (!nuevaAnotacion.isEmpty()) {
                // Actualizar la anotación en el objeto UsuarioExpediente
                usuario.anotacion = nuevaAnotacion;

                // Actualizar los datos del elemento en el QListWidget
                item->setData(Qt::UserRole, QVariant::fromValue(usuario));

                // Actualizar la etiqueta de anotación en el widget del usuario
                QLabel* annotationLabel = item->listWidget()->itemWidget(item)->findChild<QLabel*>("annotationLabel");
                if (annotationLabel) {
                    annotationLabel->setText(nuevaAnotacion);
                    annotationLabel->adjustSize(); // Ajustar el tamaño del QLabel para mostrar todo el texto
                }

                // Enviar la nueva anotación al servidor
                this->sendAnotacion(nuevaAnotacion, usuario.nombre);   
            }
        }
    });
}


void AnotacionExpediente::on_logout_clicked()
{
    QMessageBox::StandardButton confirmarLogout;
    confirmarLogout = QMessageBox::question(this, "Cerrar sesión ", "¿Está seguro que desea cerrar sesión?", QMessageBox::Yes | QMessageBox::No);
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

void AnotacionExpediente::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}

void AnotacionExpediente::on_acceptButton_clicked()
{

}

void AnotacionExpediente::messageReceived(const Message &message)
{
    int type = message.type;
    QString data = message.data;
    int error = EXIT_SUCCESS;
    switch (type)
    {
    case Message::ConsultarListaDeUsuarios:
        // Recibir la lista de usuarios
        fillUsers(data);

        emit readAllUsers(0);
        break;
    case Message::AgregarAnotacionAlExpediente:
        // Los datos tienen el mensaje de error
        error = data.toInt();
        emit receivedAnotacion(error);
        break;
    default:
        break;
    }
}

void AnotacionExpediente::requestUsers()
{
    Message request;
    request.type = Message::ConsultarListaDeUsuarios;
    QByteArray data(10, '\0');
    data.replace(0, qMin(username.length(), 10), username.toUtf8());
    request.data = data;

    // Enviar el mensaje al servidor
    client.sendMessage(request);

    // Esperar por la respuesta del servidor
    QEventLoop loop;

    connect(this, &AnotacionExpediente::readAllUsers, &loop, &QEventLoop::exit);

    loop.exec();

    disconnect(this, &AnotacionExpediente::readAllUsers, &loop, &QEventLoop::exit);
}

void AnotacionExpediente::fillUsers(QString users)
{
    QStringList list = users.split("|");
    for (int i = 0; i < list.size(); i++) {
        QStringList user = list[i].split("\t");
        agregarUsuario(user[0], user[1]);
    }
}

void AnotacionExpediente::sendAnotacion(QString anotacion, QString nombre)
{
    Message request;
    QByteArray data(210, '\0');
    request.type = Message::AgregarAnotacionAlExpediente;
    int error;

    data.replace(0, qMin(nombre.length(), 10), nombre.toUtf8());
    data.replace(10, qMin(anotacion.length(), 200), anotacion.toUtf8());

    request.data = data;
    // Enviar el mensaje al servidor
    client.sendMessage(request);

    // Esperar por la respuesta del servidor
    QEventLoop loop;

    connect(this, &AnotacionExpediente::receivedAnotacion, &loop, &QEventLoop::exit);

    error = loop.exec();

    disconnect(this, &AnotacionExpediente::receivedAnotacion, &loop, &QEventLoop::exit);

    // Si se recibió un mensaje de error, mostrar un cuadro de diálogo
    if (error != EXIT_SUCCESS) {
        QMessageBox::critical(this, "Error", "No se pudo agregar la anotación al expediente.");
    }
}
