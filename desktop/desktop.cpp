#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Wizard wizard;
    wizard.show();
    return app.exec();
}