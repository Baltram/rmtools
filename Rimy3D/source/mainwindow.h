#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "sceneinfo.h"
#include "exportsettingsdialog.h"
#include "genomematerialdialog.h"
#include "preferencesdialog.h"
#include <QMainWindow>
#include <QQueue>

class QSettings;

namespace Ui
{
    class MainWindow;
}

class MainWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget * a_pParent = 0 );
            ~MainWindow( void );
public:
    void open( QString a_strFilePath );
    void save( QString a_strFilePath );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
    void dragEnterEvent( QDragEnterEvent * a_pEvent );
    void dropEvent( QDropEvent * a_pEvent );
private:
    void updateLanguage( void );
    void updateRecentFiles( void );
private slots:
    void on_actionAbout_triggered( void );
    void on_actionClose_triggered( void );
    void on_actionConfigure_Bitmap_Paths_triggered( void );
    void on_actionEnglish_triggered( void );
    void on_actionExit_triggered( void );
    void on_actionExtended_Saving_For_GMax_triggered( void );
    void on_actionFix_GMax_Installation_triggered( void );
    void on_actionGenome_Material_Editor_triggered( void );
    void on_actionGerman_triggered( void );
    void on_actionOpen_triggered( void );
    void on_actionPreferences_triggered( void );
    void on_actionRecent1_triggered( void );
    void on_actionRecent2_triggered( void );
    void on_actionRecent3_triggered( void );
    void on_actionRecent4_triggered( void );
    void on_actionRecent5_triggered( void );
    void on_actionSave_As_triggered( void );
    void onSceneChanged( void ); 
private:
    Ui::MainWindow *     m_pUi;
    SceneInfo            m_SceneInfo;
    QQueue< QString >    m_RecentFiles;
    GenomeMaterialDialog m_GenomeMaterialDialog;
    exportSettingsDialog m_3dbDialog;
    exportSettingsDialog m_ascDialog;
    exportSettingsDialog m_aseDialog;
    exportSettingsDialog m_objDialog;
    exportSettingsDialog m_xcmshDialog;
    exportSettingsDialog m_xactDialog;
    exportSettingsDialog m_xmacDialog;
};

#endif  // MAINWINDOW_H_INCLUDED
