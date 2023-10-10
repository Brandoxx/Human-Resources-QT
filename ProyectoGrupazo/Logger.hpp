#ifndef LOGGER_H
#define LOGGER_H

#include <QtDebug>
#include <QFile>

constexpr static const char* LOG_FILE = "../UILog.txt";

class Logger
{
public:
    // Make singleton
    static Logger& getInstance() {
        static Logger instance(LOG_FILE);
        return instance;
    }

    Logger(Logger const&) = delete;

    void operator=(Logger const&) = delete;

    ~Logger() {
        closeFile();
    }

    void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg) {
        QString txt;
        switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtFatalMsg:
            txt = QString("Fatal: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
            break;
        }
        writeToLog(txt);
    }


    bool openFile() {
        // Clear the file

        if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qDebug() << "Error al abrir el archivo";
            return false;
        }
        return true;
    }

    void closeFile() {
        file->close();
    }

    void writeToLog(QString& txt) {
        QTextStream out(file);
        out << txt << "\n";
    }

private:
    QFile *file;

    Logger(QString filename) : file(new QFile(filename)) {
        if (!openFile()) {
            qCritical() << "Error al abrir el archivo de log";
        } else {
            qDebug() << "Archivo de log abierto correctamente";
        }
    }
};

// Free function that just calls the instance method

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg) {
    Logger::getInstance().logMessageHandler(type, context, msg);
}

#endif // LOGGER_H

