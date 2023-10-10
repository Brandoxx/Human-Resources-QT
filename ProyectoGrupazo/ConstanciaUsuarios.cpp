#include "ConstanciaUsuarios.h"
#include "ui_ConstanciaUsuarios.h"
#include "Login.h"

#include <QTextBrowser>
#include <QPdfWriter>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QBuffer>

ConstanciaUsuarios::ConstanciaUsuarios(QWidget *parent, UsuarioSolicitudes *UsuarioSolicitudesParent, QString constancia, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ConstanciaUsuarios),
    parentWindow(parent),
    UsuarioSolicitudesParentWindow(UsuarioSolicitudesParent),
    UsuarioConsultasParentWindow(nullptr),
    client(client)
{
    ui->setupUi(this);
    setupTextFormat(constancia);
}

ConstanciaUsuarios::ConstanciaUsuarios(QWidget *parent, UsuarioConsultas *UsuarioConsultasParent, QString constancia, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ConstanciaUsuarios),
    parentWindow(parent),
    UsuarioSolicitudesParentWindow(nullptr),
    UsuarioConsultasParentWindow(UsuarioConsultasParent),
    EstadoSolicitudesHechasParentWindow(nullptr),
    client(client)
{
    ui->setupUi(this);
    setupTextFormat(constancia);
}

ConstanciaUsuarios::ConstanciaUsuarios(QWidget *parent,  EstadoSolicitudesHechas *EstadoSolicitudesHechasParent, QString constancia, Client& client) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ConstanciaUsuarios),
    parentWindow(parent),
    UsuarioSolicitudesParentWindow(nullptr),
    UsuarioConsultasParentWindow(nullptr),
    EstadoSolicitudesHechasParentWindow(EstadoSolicitudesHechasParent),
    client(client)
{
    ui->setupUi(this);
    setupTextFormat(constancia);
}

ConstanciaUsuarios::~ConstanciaUsuarios()
{
    delete ui;
}

void ConstanciaUsuarios::setupTextFormat(QString information)
{
    QTextDocument* textDocument = new QTextDocument();
    textDocument->setDefaultTextOption(QTextOption(Qt::AlignJustify));
    // Agregar una etiqueta HTML con la imagen del logo
    // QString logoHtml = "<img src=':/logo/Images/Logo3.png' width='100' height='100'>";
    QString logoHtml = "<div style='text-align: center;'>"
                       "<img src=':/logo/Images/Logo5.png' style='width: 200px; height: 200px;'>"
                       "</div>";

    // Concatenar el contenido existente con la etiqueta HTML del logo
    information = logoHtml + information;
    textDocument->setHtml(information);

    ui->textEdit->setDocument(textDocument);

    // Aplicar estilos CSS personalizados al contenido HTML
    ui->textEdit->setStyleSheet("QTextEdit { background-color: #ffffff; color: #333333; font-family: Arial;}"
                                "h1 { color: #555555; text-align: center; }"
                                "hr { border: none; border-top: 1px solid #cccccc; margin: 10px 0; }"
                                "p { margin: 5px 0; text-indent: 1em; text-align: justify; }"
                                "strong { font-weight: bold; }");
    ui->textEdit->setReadOnly(true);
}


void ConstanciaUsuarios::on_acceptButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Guardar como PDF", "", "Archivos PDF (*.pdf)");
    if (!fileName.isEmpty()) {
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPageSize(QPageSize::A4);
        printer.setOutputFileName(fileName);

        QTextDocument* document = ui->textEdit->document();
        QTextCursor cursor(document);

        if (printer.isValid()) {
            document->print(&printer);
            QMessageBox::information(this, "Informe en PDF", "El informe ha sido generado y guardado como PDF.");
        } else {
            QMessageBox::warning(this, "Error", "No se pudo guardar el archivo PDF.");
        }
    }
}


void ConstanciaUsuarios::on_cancelButton_clicked()
{
    parentWindow->show();
    close();
    this->deleteLater();
}


void ConstanciaUsuarios::on_logout_clicked()
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


