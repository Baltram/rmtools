#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include "shortinstruction.h"
#include "global.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void writeSettings(void);
    void readSettings(void);

    void closeEvent(QCloseEvent * event);
    void updateLanguage(void);

    Ui::MainWindow *ui;

    QString objPath;
    QString xmacPath;
    QString newXmacPath;
    QString localeStr;

    ShortInstruction * instr;

    static rmOut out;

private slots:
    void on_actionConvert_triggered();
    void on_actionShortInstruction_triggered();
    void on_lineEdit_newXmac_textChanged(QString );
    void on_lineEdit_xmac_textChanged(QString );
    void on_lineEdit_obj_textChanged(QString );
    void on_pushButton_convert_clicked();
    void on_pushButton_newXmac_clicked();
    void on_pushButton_xmac_clicked();
    void on_pushButton_obj_clicked();
    void on_checkBox_stateChanged(int newState);
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionGerman_triggered();
    void on_actionEnglish_triggered();
};
