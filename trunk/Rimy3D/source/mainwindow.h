#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QQueue>
#include "sceneinfo.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget * a_pParent = 0 );
    ~MainWindow();
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
private:
    void open( QString a_strFileName );
    void updateLanguage( void );
    void updateRecentFiles( void );
private slots:
    void on_actionEnglish_triggered( void );
    void on_actionGerman_triggered( void );
    void on_actionOpen_triggered();
    void on_actionRecent1_triggered();

    void on_actionRecent2_triggered();

    void on_actionRecent3_triggered();

    void on_actionRecent4_triggered();

    void on_actionRecent5_triggered();

private:
    Ui::MainWindow *  m_pUi;
    SceneInfo         m_SceneInfo;
    QQueue< QString > m_RecentFiles;
};

#endif // MAINWINDOW_H
