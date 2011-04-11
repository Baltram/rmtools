#include "xcompose.h"

QTranslator * XcomPose::german_qt = 0;
QTranslator * XcomPose::german_app = 0;
QTranslator * XcomPose::current_qt = 0;
QTranslator * XcomPose::current_app = 0;

XcomPose::XcomPose(int & argc, char * argv[]) : QApplication(argc, argv)
{
    delete german_qt;
    german_qt = new QTranslator;
    german_qt->load("qt_de");

    delete german_app;
    german_app = new QTranslator;
	german_app->load(":/translations/xcomPose_de");
}

XcomPose::~XcomPose() {}

void XcomPose::setLanguage(const QString & locale)
{
    QTranslator * newQtTranslator = 0;
    QTranslator * newAppTranslator = 0;

    if (locale == "de") {
	newQtTranslator = german_qt;
	newAppTranslator = german_app;
    }
    // else...

    if (newQtTranslator && newAppTranslator) {
	installTranslator(newQtTranslator);
	installTranslator(newAppTranslator);
    }
    if ((current_qt && current_app) && ((current_qt != newQtTranslator) && (current_app != newAppTranslator))) {
	removeTranslator(current_qt);
	removeTranslator(current_app);
    }
    current_qt = newQtTranslator;
    current_app = newAppTranslator;
}

