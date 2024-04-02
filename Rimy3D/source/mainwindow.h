#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "main.h"
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
    static MainWindow & getInstance( void );
    static QString      getOpenFilter( void );
    static QString      getSaveFilter( void );
public:
    void                         applyCliOptions( QVariant ( &options )[ CliOption_Count ] );
    exportSettingsDialog const * getExportSettings( QString const & a_strPath, bool a_bAllowSkinCalculation );
    bool                         open( QString a_strFilePath );
    bool                         save( QString a_strFilePath );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
    void dragEnterEvent( QDragEnterEvent * a_pEvent );
    void dropEvent( QDropEvent * a_pEvent );
private:
    explicit MainWindow( QWidget * a_pParent = 0 );
            ~MainWindow( void );
private:
    void updateLanguage( void );
    void updateRecentFiles( void );
private slots:
    void on_action3db_Import_Export_Scripts_For_Blender_triggered( void );
    void on_action3db_Tools_for_3ds_Max_triggered( void );
    void on_actionAbout_triggered( void );
    void on_actionBatch_Conversion_triggered( void );
    void on_actionCheck_For_Updates_triggered( void );
    void on_actionClose_triggered( void );
    void on_actionConfigure_Bitmap_Paths_triggered( void );
    void on_actionContents_triggered( void );
    void on_actionEnglish_triggered( void );
    void on_actionExit_triggered( void );
    void on_actionExtended_Saving_For_GMax_triggered( void );
    void on_actionFix_GMax_Installation_triggered( void );
    void on_actionGenome_Material_Editor_triggered( void );
    void on_actionGerman_triggered( void );
    void on_actionMerge_triggered( void );
    void on_actionOpen_triggered( void );
    void on_actionPreferences_triggered( void );
    void on_actionRecent1_triggered( void );
    void on_actionRecent2_triggered( void );
    void on_actionRecent3_triggered( void );
    void on_actionRecent4_triggered( void );
    void on_actionRecent5_triggered( void );
    void on_actionSave_As_triggered( void );
    void on_menuNew_Version_Available_triggered( void );
    void on_menuSearchPathsReminder_triggered( void );
    void onSceneChanged( void );
private:
    static MainWindow * s_pInstance;
private:
    Ui::MainWindow *     m_pUi;
    SceneInfo            m_SceneInfo;
    QQueue< QString >    m_RecentFiles;
    GenomeMaterialDialog m_GenomeMaterialDialog;
    exportSettingsDialog m_3dbDialog;
    exportSettingsDialog m_3dsDialog;
    exportSettingsDialog m_ascDialog;
    exportSettingsDialog m_aseDialog;
    exportSettingsDialog m_objDialog;
    exportSettingsDialog m_xcmshDialog;
    exportSettingsDialog m_xmshDialog;
    exportSettingsDialog m_xlmshDialog;
    exportSettingsDialog m_xactDialog;
    exportSettingsDialog m_xmacDialog;
    exportSettingsDialog m_xnvmshDialog;
    exportSettingsDialog m_xcomDialog;
    bool                 m_bOnMerge;
};

#endif  // MAINWINDOW_H_INCLUDED
