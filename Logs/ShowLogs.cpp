#include "ShowLogs.h"
#include "ui_ShowLogs.h"
#include <QTextDocument>
#include <QTextOption>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>
#include <QFileDialog>

ShowLogs::ShowLogs(QWidget *parent, Auditor *AuditorMenuParent, QString nombreArchivo) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ShowLogs),
    parentWindow(parent),
    AuditorMenuParentWindow(AuditorMenuParent),
    archivo(nombreArchivo)
{
    ui->setupUi(this);
    setupTextFormat();
}

ShowLogs::~ShowLogs()
{
    delete ui;
}

void ShowLogs::setupTextFormat()
{
    QString information = "";
    QFile *file = new QFile(archivo);
    if (file->open(QIODevice::ReadOnly))
    {
        information = file->readAll();
        file->close();
        static QRegularExpression removePartsRegex("\\(\\.\\./ProyectoGrupazo/[^\\)]+\\)");
        information.replace(removePartsRegex, "");

        static QRegularExpression lineSplitRegex("\\r?\\n");
        QStringList lines = information.split(lineSplitRegex);

        QTextDocument* textDocument = new QTextDocument();
        textDocument->setDefaultTextOption(QTextOption(Qt::AlignJustify));

        // Construir el contenido HTML directamente en el QTextDocument
        QString documentContent = "<h2 style=\"text-align: center;\">Registro de Usuario</h2>";

        for (const QString& line : lines)
        {
            if (!line.trimmed().isEmpty())
            {
                QString formattedLine = line;
                formattedLine.replace("Debug:", "<br><strong>Debug:</strong>");
                formattedLine.replace("Warning:", "<br><strong>Warning:</strong>");
                formattedLine.replace("Critical:", "<br><strong>Critical:</strong>");
                formattedLine.replace("Fatal:", "<br><strong>Fatal:</strong>");
                formattedLine.replace(")", "");

                documentContent += formattedLine + "<br>";
            }
        }
        textDocument->setHtml(documentContent);
        ui->textEdit->setDocument(textDocument);
        ui->textEdit->setReadOnly(true);
    } else {
        // Mostrar error
        QString error = "No se pudo abrir el archivo " + file->fileName();
        QMessageBox::critical(this, tr("Error"), error);
        AuditorMenuParentWindow->show();
        close();
        this->deleteLater();
    }
}

void ShowLogs::on_cancelButton_clicked()
{
    AuditorMenuParentWindow->show();
    close();
    this->deleteLater();
}


void ShowLogs::on_acceptButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Guardar como archivo de texto", "", "Archivos de texto (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << ui->textEdit->toPlainText();
            file.close();
            QMessageBox::information(this, "Informe en archivo de texto", "El informe ha sido generado y guardado como archivo de texto.");
        } else {
            QMessageBox::warning(this, "Error", "No se pudo guardar el archivo de texto.");
        }
    }
}


