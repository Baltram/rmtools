#pragma once

#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>

class XcomPose : public QApplication
{
    Q_OBJECT

public:
    explicit XcomPose(int & argc, char * argv[]);
    ~XcomPose();

public slots:
    static void setLanguage(const QString & locale);

private:
    static QTranslator * german_qt;
    static QTranslator * german_app;

    static QTranslator * current_qt;
    static QTranslator * current_app;
};
