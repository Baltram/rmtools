#include "xmatedit.h"

QTranslator * XmatEdit::german_qt = 0;
QTranslator * XmatEdit::german_app = 0;
QTranslator * XmatEdit::current_qt = 0;
QTranslator * XmatEdit::current_app = 0;

XmatEdit::XmatEdit(int & argc, char * argv[]) : QApplication(argc, argv)
{
    delete german_qt;
    german_qt = new QTranslator;
    german_qt->load("qt_de");

    delete german_app;
    german_app = new QTranslator;
    german_app->load(":/translations/xmatEdit_de");
}

XmatEdit::~XmatEdit() {}

void XmatEdit::setLanguage(const QString & locale)
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
