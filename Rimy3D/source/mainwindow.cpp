#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texturefinder.h"
#include "batchdialog.h"
#include "pluginsdialog.h"
#include <QClipboard>

MainWindow * MainWindow::s_pInstance = 0;

MainWindow & MainWindow::getInstance( void )
{
    if ( !s_pInstance )
        s_pInstance = new MainWindow;
    return *s_pInstance;
}

QString MainWindow::getOpenFilter( void )
{
    QString strResult = tr( "All files" );
    strResult.append( " (*.3ds *.obj *.3db *.gmax *.ase *.asc *.xcmsh *.xlmsh *.xnvmsh *.xact *._xmac *._xmsh *._xcom *.r3msh);;"
                      "Baltram's 3D format (*.3db);;"
                      "3D Studio Mesh (*.3ds);;"
                      "Wavefront OBJ format (*.obj);;"
                      "GMax Scene (*.gmax);;"
                      "3ds Max ASCII Scene (*.ase);;"
                      "Gothic ASCII Scene (*.asc);;"
                      "Gothic 3 Mesh (*.xcmsh);;"
                      "Gothic 3 Motion Actor (*.xact);;"
                      "Gothic 3 LOD Mesh (*.xlmsh);;"
                      "Gothic 3 Collision Mesh (*.xnvmsh);;"
                      "Risen Mesh (*._xmsh);;"
                      "Risen Motion Actor (*._xmac);;"
                      "Risen Collision Mesh (*._xcom);;"
                      "Risen 3 Mesh (*.r3msh);;" );
    return strResult;
}

QString MainWindow::getSaveFilter( void )
{
    QString strResult = tr( "All files" );
    strResult.append( " (*.*);;"
                      "Baltram's 3D format (*.3db);;"
                      "3D Studio Mesh (*.3ds);;"
                      "Wavefront OBJ format (*.obj);;"
                      "3ds Max ASCII Scene (*.ase);;"
                      "Gothic ASCII Scene (*.asc);;"
                      "Gothic 3 Mesh (*.xcmsh);;"
                      "Gothic 3 Motion Actor (*.xact);;"
                      "Gothic 3 LOD Mesh (*.xlmsh);;"
                      "Gothic 3 Collision Mesh (*.xnvmsh);;"
                      "Risen Mesh (*._xmsh);;"
                      "Risen Motion Actor (*._xmac);;"
                      "Risen Collision Mesh (*._xcom);;" );
    return strResult;
}

void MainWindow::applyCliOptions( QVariant ( &options )[ CliOption_Count ] )
{
    exportSettingsDialog * dialogs[] =
    {
        &m_3dbDialog,
        &m_3dsDialog,
        &m_ascDialog,
        &m_aseDialog,
        &m_objDialog,
        &m_xcmshDialog,
        &m_xmshDialog,
        &m_xlmshDialog,
        &m_xactDialog,
        &m_xmacDialog,
        &m_xnvmshDialog,
        &m_xcomDialog
    };
    for ( int i = sizeof( dialogs ) / sizeof( dialogs[ 0 ] ); i--; )
        dialogs[ i ]->applyCliOptions( options );
}

exportSettingsDialog const * MainWindow::getExportSettings( QString const & a_strPath, bool a_bAllowSkinCalculation )
{
    QString strExt = QFileInfo( a_strPath ).suffix().toLower();
    exportSettingsDialog * pDialog = 0;
    if ( strExt == "3db" )
        pDialog = &m_3dbDialog;
    else if ( strExt == "3ds" )
        pDialog = &m_3dsDialog;
    else if ( strExt == "asc" )
        pDialog = &m_ascDialog;
    else if ( strExt == "ase" )
        pDialog = &m_aseDialog;
    else if ( strExt == "obj" )
        pDialog = &m_objDialog;
    else if ( strExt == "xcmsh" )
        pDialog = &m_xcmshDialog;
    else if ( strExt == "_xmsh" )
        pDialog = &m_xmshDialog;
    else if ( strExt == "xlmsh" )
        pDialog = &m_xlmshDialog;
    else if ( strExt == "xact" )
        ( pDialog = &m_xactDialog )->setAutoSkinVisible( m_SceneInfo.sceneContainsUnskinnedMeshes() && a_bAllowSkinCalculation );
    else if ( strExt == "_xmac" )
        ( pDialog = &m_xmacDialog )->setAutoSkinVisible( m_SceneInfo.sceneContainsUnskinnedMeshes() && a_bAllowSkinCalculation );
    else if ( strExt == "xnvmsh" )
        pDialog = &m_xcomDialog;
    else if ( strExt == "_xcom" )
        pDialog = &m_xcomDialog;
    else
        Rimy3D::showError( tr( "Unknown file extension:" ).append( QString( " '.%1'" ).arg( strExt ) ) );
    if ( pDialog && strExt != "asc" && strExt != "xlmsh" && !Rimy3D::quiet() )
    {
        pDialog->setParent( Rimy3D::activeWindow(), Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::Dialog );
        int iResult = pDialog->exec();
        pDialog->setParent( this );
        return iResult ? pDialog : 0;
    }
    return pDialog;
}

bool MainWindow::open( QString a_strFilePath )
{
    bool bOnMerge = m_bOnMerge;
    m_bOnMerge = false;
    if ( a_strFilePath == "" )
        return false;
    a_strFilePath = QDir::toNativeSeparators( a_strFilePath );
    QString strExt = QFileInfo( a_strFilePath ).suffix();
    if ( !Rimy3D::quiet() && ( strExt == "_xmat" || strExt == "xshmat" ) )
    {
        if ( !m_GenomeMaterialDialog.isVisible() )
            m_GenomeMaterialDialog.show();
        return m_GenomeMaterialDialog.openMaterial( a_strFilePath );
    }
    m_pUi->menuSearchPathsReminder->menuAction()->setVisible( false );
    m_RecentFiles.removeOne( a_strFilePath );
    if ( m_SceneInfo.openSceneFile( a_strFilePath, bOnMerge ) )
    {
        m_pUi->actionClose->setEnabled( true );
        m_pUi->actionMerge->setEnabled( true );
        m_pUi->actionSave_As->setEnabled( true );
        m_RecentFiles.enqueue( a_strFilePath );
        QFileInfo File( a_strFilePath );
        if ( !bOnMerge )
            setWindowTitle( Rimy3D::applicationName() + " - " + File.fileName() );
        updateRecentFiles();
        if ( ( strExt == "xcmsh" || strExt == "xact" || strExt == "_xmsh" || strExt == "_xmac" ) && 
             PreferencesDialog::getInstance().lookUpMaterials() &&
             !TextureFinder::getInstance().hasArchives() )
            m_pUi->menuSearchPathsReminder->menuAction()->setVisible( true );
        return true;
    }
    updateRecentFiles();
    return false;
}

bool MainWindow::save( QString a_strFilePath )
{
    if ( a_strFilePath == "" )
        return false;
    a_strFilePath = QDir::toNativeSeparators( a_strFilePath );
    exportSettingsDialog const * pDialog = getExportSettings( a_strFilePath, true );
    if ( pDialog )
        return m_SceneInfo.saveSceneFile( a_strFilePath, *pDialog, &m_pUi->widget->getWorld() );
    return false;
}

void MainWindow::loadSettings( QSettings & a_Settings )
{
    m_pUi->actionFix_GMax_Installation->setVisible( !a_Settings.value( "MAXComponents", true ).toBool() );
    a_Settings.beginGroup( "MainWindow" );
    restoreGeometry( a_Settings.value( "geometry", saveGeometry() ).toByteArray() );
    m_RecentFiles.clear();
    for ( int i = a_Settings.value( "recentfilecount", 0 ).toInt(); i--; )
        m_RecentFiles.enqueue( a_Settings.value( "recentfile" + QString::number( i ) ).toString() );
    a_Settings.endGroup();
    updateRecentFiles();
}

void MainWindow::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "MainWindow" );
    a_Settings.setValue( "geometry", saveGeometry() );
    int iQueueSize = m_RecentFiles.size();
    a_Settings.setValue( "recentfilecount", iQueueSize );
    for ( QList< QString >::iterator it = m_RecentFiles.begin(); iQueueSize--; ++it )
        a_Settings.setValue( "recentfile" + QString::number( iQueueSize ), *it );
    a_Settings.endGroup();
}

void MainWindow::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QMainWindow::changeEvent( a_pEvent );
}

void MainWindow::closeEvent( QCloseEvent * a_pEvent )
{
    Rimy3D::saveSettings();
    if ( QApplication::clipboard()->ownsClipboard() )
        QApplication::clipboard()->clear();
    QMainWindow::closeEvent( a_pEvent );
}

void MainWindow::dragEnterEvent( QDragEnterEvent * a_pEvent )
{
    if ( ( a_pEvent->mimeData()->hasUrls() ) &&
         ( a_pEvent->mimeData()->urls().count() == 1 ) &&
         ( a_pEvent->mimeData()->urls()[ 0 ].toString().left( 5 ).toLower() == "file:" ) )
        a_pEvent->acceptProposedAction();
}

void MainWindow::dropEvent( QDropEvent * a_pEvent )
{
    open( a_pEvent->mimeData()->urls()[ 0 ].toLocalFile() );
}

MainWindow::MainWindow( QWidget * a_pParent ) :
    QMainWindow( a_pParent ),
    m_pUi( new Ui::MainWindow ),
    m_3dbDialog( this, "3db", exportSettingsDialog::NormalsRecalc | exportSettingsDialog::Colors ),
    m_3dsDialog( this, "3ds", exportSettingsDialog::ExportSGs | exportSettingsDialog::Gothic3ds ),
    m_ascDialog( this, "asc", exportSettingsDialog::None ),
    m_aseDialog( this, "ase", exportSettingsDialog::Normals | exportSettingsDialog::Colors ),
    m_objDialog( this, "obj", exportSettingsDialog::Normals | exportSettingsDialog::CreateMtl ),
    m_xcmshDialog( this, "xcmsh", exportSettingsDialog::NormalsCalc | exportSettingsDialog::Colors ),
    m_xmshDialog( this, "xmsh", exportSettingsDialog::NormalsCalc | exportSettingsDialog::Colors ),
    m_xlmshDialog( this, "xlmsh", exportSettingsDialog::None ),
    m_xactDialog( this, "xact", exportSettingsDialog::VertsOnly | exportSettingsDialog::BaseXact | exportSettingsDialog::AutoSkin | exportSettingsDialog::NormalsCalc ),
    m_xmacDialog( this, "_xmac", exportSettingsDialog::VertsOnly | exportSettingsDialog::BaseXmac | exportSettingsDialog::AutoSkin | exportSettingsDialog::NormalsCalc ),
    m_xnvmshDialog( this, "xnvmsh", exportSettingsDialog::Convex | exportSettingsDialog::NoTextures ),
    m_xcomDialog( this, "_xcom", exportSettingsDialog::Convex | exportSettingsDialog::NoTextures ),
    m_GenomeMaterialDialog( m_SceneInfo ),
    m_bOnMerge( false )
{
    m_pUi->setupUi( this );
    setWindowTitle( Rimy3D::applicationName() );
    setAcceptDrops( true );
    updateLanguage();
    connect( &m_SceneInfo, SIGNAL( sceneChanged( void ) ), this, SLOT( onSceneChanged( void ) ) );
    connect( &PreferencesDialog::getInstance(), SIGNAL( materialLookupRequested( void ) ), &m_SceneInfo, SLOT( lookUpGenomeMaterials( void ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
    connect( m_pUi->menuNew_Version_Available, SIGNAL( aboutToShow( void ) ), this, SLOT( on_menuNew_Version_Available_triggered( void ) ) );
    connect( m_pUi->menuSearchPathsReminder, SIGNAL( aboutToShow( void ) ), this, SLOT( on_menuSearchPathsReminder_triggered( void ) ) );
    if ( Rimy3D::isUpToDate() )
        m_pUi->menuBar->removeAction( m_pUi->menuNew_Version_Available->menuAction() );
    m_pUi->menuSearchPathsReminder->menuAction()->setVisible( false );
    TextureFinder::getInstance();
    PreferencesDialog::getInstance();
    BatchDialog::getInstance();
    Rimy3D::loadSettings();
}

MainWindow::~MainWindow( void )
{
    delete m_pUi;
}

void MainWindow::updateLanguage( void )
{
    char const * const arrIconFileNames[ Rimy3D::ELanguage_Count ] =
    {
        "gb.png",
        "de.png"
    };
    m_pUi->menuLanguage->setIcon( QIcon( QString( ":/flags/icons/" ).append( arrIconFileNames[ Rimy3D::getLanguage() ] ) ) );
    m_pUi->retranslateUi( this );
}

void MainWindow::updateRecentFiles( void )
{
    enum { ERecentFileCountMax = 5 };
    QAction * arrActions[ ERecentFileCountMax ];
    arrActions[ 0 ] = m_pUi->actionRecent1;
    arrActions[ 1 ] = m_pUi->actionRecent2;
    arrActions[ 2 ] = m_pUi->actionRecent3;
    arrActions[ 3 ] = m_pUi->actionRecent4;
    arrActions[ 4 ] = m_pUi->actionRecent5;
    while ( m_RecentFiles.count() > ERecentFileCountMax )
        m_RecentFiles.dequeue();
    int iRecentFileCount = m_RecentFiles.count();
    for ( int i = ERecentFileCountMax; i-- > iRecentFileCount; arrActions[ i ]->setVisible( false ) );
    for ( QList< QString >::iterator it = m_RecentFiles.begin(); iRecentFileCount--; ++it )
    {
        arrActions[ iRecentFileCount ]->setVisible( true );
        arrActions[ iRecentFileCount ]->setText( *it );
    }
}

void MainWindow::on_action3db_Import_Export_Scripts_For_Blender_triggered( void )
{
    PluginsDialog( this ).exec( PluginsDialog::Blender );
}

void MainWindow::on_action3db_Tools_for_3ds_Max_triggered( void )
{
    PluginsDialog( this ).exec( PluginsDialog::Max );
}

void MainWindow::on_actionAbout_triggered( void )
{
    Rimy3D::showMessage( "<p></p><b>Rimy3D v" + Rimy3D::getVersionString() + tr( "</b> (April 2nd 2024)"
                         "<div style='text-indent:16px;'>by <a href='mailto:baltram-lielb@web.de'>Baltram</a> @<a href='https://forum.worldofplayers.de/forum/members/33859'>WoP</a></div>"
                         "<p>Website: <a href='https://www.baltr.de/Rimy3D.htm'>baltr.de/Rimy3D.htm</a></p>"
                         "<p>GitHub: <a href='https://github.com/Baltram/rmtools'>github.com/Baltram/rmtools</a></p>"
                         "<p>This program uses the GLC_lib library by Laurent Ribon.</p>"
                         "<p></p>" ),
                         tr( "About Rimy3D" ) );
}

void MainWindow::on_actionBatch_Conversion_triggered()
{
    BatchDialog & BatchDialog = BatchDialog::getInstance();
    BatchDialog.move( this->geometry().center() - BatchDialog.rect().center() );
    BatchDialog.exec();
}

void MainWindow::on_actionCheck_For_Updates_triggered( void )
{
    PreferencesDialog::getInstance().checkForUpdates( true );
}

void MainWindow::on_actionClose_triggered( void )
{
    m_SceneInfo.clearScene();
    m_pUi->actionClose->setEnabled( false );
    m_pUi->actionMerge->setEnabled( false );
    m_pUi->actionSave_As->setEnabled( false );
    setWindowTitle( Rimy3D::applicationName() );
}

void MainWindow::on_actionConfigure_Bitmap_Paths_triggered( void )
{
    TextureFinder::getInstance().showDialog();
}

void MainWindow::on_actionContents_triggered( void )
{
    Rimy3D::displayHelp();
}

void MainWindow::on_actionEnglish_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_English );
}

void MainWindow::on_actionExit_triggered( void )
{
    close();
}

void MainWindow::on_actionExtended_Saving_For_GMax_triggered( void )
{
    PluginsDialog( this ).exec( PluginsDialog::GMax );
}

void MainWindow::on_actionFix_GMax_Installation_triggered( void )
{
    m_pUi->actionFix_GMax_Installation->setVisible( !Rimy3D::checkGmaxInstallation() );
}

void MainWindow::on_actionGenome_Material_Editor_triggered( void )
{
    m_GenomeMaterialDialog.move( this->geometry().center() - m_GenomeMaterialDialog.rect().center() );
    m_GenomeMaterialDialog.exec();
}

void MainWindow::on_actionGerman_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_German );
}

void MainWindow::on_actionMerge_triggered( void )
{
    m_bOnMerge = true;
    on_actionOpen_triggered();
}

void MainWindow::on_actionOpen_triggered( void )
{
    open( QFileDialog::getOpenFileName( this, tr( m_bOnMerge ? "Merge" : "Open" ), m_SceneInfo.getCurrentDir(), getOpenFilter() ) );
}

void MainWindow::on_actionPreferences_triggered( void )
{
    PreferencesDialog & PrefsDialog = PreferencesDialog::getInstance();
    PrefsDialog.move( this->geometry().center() - PrefsDialog.rect().center() );
    PrefsDialog.exec();
}

void MainWindow::on_actionRecent1_triggered( void )
{
    open( m_pUi->actionRecent1->text() );
}

void MainWindow::on_actionRecent2_triggered( void )
{
    open( m_pUi->actionRecent2->text() );
}

void MainWindow::on_actionRecent3_triggered( void )
{
    open( m_pUi->actionRecent3->text() );
}

void MainWindow::on_actionRecent4_triggered( void )
{
    open( m_pUi->actionRecent4->text() );
}

void MainWindow::on_actionRecent5_triggered( void )
{
    open( m_pUi->actionRecent5->text() );
}

void MainWindow::on_actionSave_As_triggered( void )
{
    QString strFilePath = m_SceneInfo.getCurrentSaveDir() + QDir::separator() + QFileInfo( m_SceneInfo.getCurrentFile() ).baseName();
    save( QFileDialog::getSaveFileName( this, tr( "Save As" ), strFilePath, getSaveFilter() ) );
}

void MainWindow::on_menuNew_Version_Available_triggered( void )
{
    m_pUi->menuNew_Version_Available->hide();
    m_pUi->menuNew_Version_Available->show();
    on_actionCheck_For_Updates_triggered();
}

void MainWindow::on_menuSearchPathsReminder_triggered( void )
{
    m_pUi->menuSearchPathsReminder->hide();
    m_pUi->menuSearchPathsReminder->show();
    Rimy3D::showMessage( tr( "When importing Gothic 3/Risen meshes, it is recommended to register material\n"
        "and texture archives in the search paths dialog in the 'Extras' menu.\n"
        "\n"
        "Minimal configuration for Gothic 3:\n"
        "    <Gothic 3 directory>\\Data\\_compiledMaterial.pak\n"
        "    <Gothic 3 directory>\\Data\\_compiledImage.pak\n"
        "\n"
        "Minimal configuration for Risen:\n"
        "    <Risen directory>\\data\\common\\materials.pak\n"
        "    <Risen directory>\\data\\compiled\\images.pak\n"
        "\n"
        "For both games, any patch or mod volumes (.p00, .p01, .m00 etc.) containing\n"
        "images or materials can be added on top of that." ) );
}

void MainWindow::onSceneChanged( void )
{
    m_pUi->widget->resetCamera();
    m_pUi->widget->setWorld( m_SceneInfo.buildGlcWorld() );
    m_GenomeMaterialDialog.updateMaterialSuggestions();
}
