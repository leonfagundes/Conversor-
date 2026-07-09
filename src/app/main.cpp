#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("Conversor");
    QApplication::setApplicationName("Conversor");

    MainWindow window;
    window.show();

    return app.exec();
}
