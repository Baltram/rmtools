#include "xcompose.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    XcomPose a(argc, argv);

    QCoreApplication::setOrganizationName("Baltram");
    QCoreApplication::setApplicationName("xcomPose");

	registerGetDirFunc();

    rmBool log = false;
    if (argc > 1) {
	if (strcmp(argv[1], "log") == 0) {
	    log = true;
	}
    }
    initOutput(argv[0], log);

    MainWindow w;
    outMan_parent = &w;
    w.show();

    return a.exec();
}
