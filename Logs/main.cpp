#include "Auditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Auditor w(NULL);
    w.show();
    return a.exec();
}
