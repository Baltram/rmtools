#pragma once

#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>

class Obj2xmac : public QApplication
{
    Q_OBJECT

public:
    explicit Obj2xmac(int & argc, char * argv[]);
    ~Obj2xmac();

public slots:
    static void setLanguage(const QString & locale);

private:
    static QTranslator * german_qt;
    static QTranslator * german_app;

    static QTranslator * current_qt;
    static QTranslator * current_app;
};
