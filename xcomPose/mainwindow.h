#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include "global.h"
#include "shortinstruction.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void writeSettings(void);
    void readSettings(void);

    void updateLanguage(void);

    void closeEvent(QCloseEvent * event);

private:
    void updateEnableStates(void);
    void updateTitle(void);

    void importObjFile(QString filePath);
    void exportObjFile(QString filePath, rmU32 index);

private:
    QString localeStr;

    QString xcomFolder;
    QString objOpenFolder;
    QString objStoreFolder;
	QString binFilePath;

    QString currentFilePath;

	ShortInstruction * shortInstr;

    static rmOut out;

    Ui::MainWindow *ui;

private slots:
	void on_actionShortInstruction_triggered();
 void on_pushButtonBin_clicked();
	void on_actionCompleteBinUpdate_triggered();
	void on_actionUpdateBin_triggered();
	void on_actionSetBinPath_triggered();
	void on_actionAbout_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSave_triggered();
    void on_pushButtonExportAll_clicked();
    void on_pushButtonExport_clicked();
    void on_actionExportAll_triggered();
    void on_actionExport_triggered();
    void on_pushButtonImport_clicked();
    void on_actionImport_triggered();
    void on_actionRemove_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_pushButtonRemove_clicked();
    void on_actionEnglish_triggered();
    void on_actionDeutsch_triggered();
    void on_actionExit_triggered();
};
