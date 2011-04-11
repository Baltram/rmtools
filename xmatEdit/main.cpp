#include "xmatedit.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    XmatEdit a(argc, argv);

    QCoreApplication::setOrganizationName("Baltram");
    QCoreApplication::setApplicationName("xmatEdit Alpha");

    MainWindow mainWindow;

    rmBool log = false;
    if (argc > 1) {
        if (strcmp(argv[1], "log") == 0) {
            log = true;
        }
    }
    initOutput(argv[0], log);

    mainWindow.show();

    return a.exec();
}
