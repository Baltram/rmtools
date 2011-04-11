#pragma once

#include <QMainWindow>
#include <QLayout>
#include <QSettings>
#include <QCloseEvent>
#include "xmatedit.h"
#include "rmPropertyLinker.h"
#include "global.h"
#include "guidlineedit.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString getLocaleStr(void);

private:
    void writeSettings(void);
    void readSettings(void);

    void closeEvent(QCloseEvent * event);
    void updateLanguage(void);

    void updateTitle(void);

	void append(QList<QtBrowserItem *> & dest, QList<QtBrowserItem *> & src);

    Ui::MainWindow *ui;

    rmPropertyLinker * currentLinker;
    rmDummyClass::xmatFile * currentFileContent;
    QtTreePropertyBrowser * propertyBrowser;
    QString xmatFilePath;
    QString xmatDirectory;
    QString localeStr;

	QList<QtBrowserItem *> texItems;
	int iTexItem;

private slots:
	void on_actionSkip_triggered();
	void on_actionSaveAs_triggered();
    void on_actionGerman_triggered();
    void on_actionEnglish_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void on_linker_modified();
};
