#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rimy3d.h"
#include "texturefinder.h"

MainWindow::MainWindow( QWidget * a_pParent ) :
    QMainWindow( a_pParent ),
    m_pUi( new Ui::MainWindow ),
    m_3dbDialog( this, "3db", exportSettingsDialog::NormalsRecalc | exportSettingsDialog::Colors ),
    m_ascDialog( this, "asc", exportSettingsDialog::None ),
    m_aseDialog( this, "ase", exportSettingsDialog::Normals ),
    m_objDialog( this, "obj", exportSettingsDialog::Normals | exportSettingsDialog::CreateMtl ),
    m_xactDialog( this, "xact", exportSettingsDialog::NormalsCalc | exportSettingsDialog::VertsOnly | exportSettingsDialog::BaseXact )
{
    m_pUi->setupUi( this );
    setAcceptDrops( true );
    updateLanguage();
    connect( &m_SceneInfo, SIGNAL( sceneChanged( void ) ), this, SLOT( onSceneChanged( void ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
    TextureFinder::getInstance();
    Rimy3D::setMainWindow( this );
    Rimy3D::loadSettings();
}

MainWindow::~MainWindow( void )
{
    delete m_pUi;
}

void MainWindow::open( QString a_strFilePath )
{
    if ( a_strFilePath == "" )
        return;
    a_strFilePath = QDir::toNativeSeparators( a_strFilePath );
    m_RecentFiles.removeOne( a_strFilePath );
    if ( m_SceneInfo.openSceneFile( a_strFilePath ) )
    {
        if ( m_pUi->actionClose->isEnabled() )
            setWindowTitle( Rimy3D::applicationName() );
        m_pUi->actionClose->setEnabled( true );
        m_pUi->actionSave_As->setEnabled( true );
        m_RecentFiles.enqueue( a_strFilePath );
        QFileInfo File( a_strFilePath );
        //Rimy3D::showMessage( File.fileName() );
        setWindowTitle( Rimy3D::applicationName() + " - " + File.fileName() );
    }
    updateRecentFiles();
}

void MainWindow::save( QString a_strFilePath )
{
    if ( a_strFilePath == "" )
        return;
    a_strFilePath = QDir::toNativeSeparators( a_strFilePath );
    QString strExt = QFileInfo( a_strFilePath ).suffix().toLower();
    exportSettingsDialog * pDialog = 0;
    if ( strExt == "3db" )
        pDialog = &m_3dbDialog;
    else if ( strExt == "asc" )
        pDialog = &m_ascDialog;
    else if ( strExt == "ase" )
        pDialog = &m_aseDialog;
    else if ( strExt == "obj" )
        pDialog = &m_objDialog;
    else if ( strExt == "xact" )
        pDialog = &m_xactDialog;
    else
        Rimy3D::showError( tr( "Unknown file extension:" ).append( QString( " '.%1'" ).arg( strExt ) ) );
    if ( !pDialog || ( ( strExt != "asc" ) && !pDialog->exec() ) )
        return;
    m_SceneInfo.saveSceneFile( a_strFilePath, *pDialog );
}

void MainWindow::loadSettings( QSettings & a_Settings )
{
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

void MainWindow::on_actionAbout_triggered( void )
{
    Rimy3D::showMessage( "<p></p><b>Rimy3D v0.1</b> (April 13th 2012)"
                         "<div style='text-indent:16px;'>by <a href='mailto:baltram-lielb@web.de'>Baltram</a> @<a href='http://forum.worldofplayers.de/forum/members/33859'>WoP</a></div>"
                         "<p>Support: <a href='http://www.baltr.de/Rimy3D.htm'>www.baltr.de/Rimy3D.htm</a></p>"
                         "<p>This program uses the <a href='http://www.glc-lib.net'>GLC_lib</a> library by Laurent Ribon.</p>"
                         "<p></p>",
                         tr( "About Rimy3D" ) );
}

void MainWindow::on_actionClose_triggered( void )
{
    m_SceneInfo.clearScene();
    m_pUi->actionClose->setEnabled( false );
    m_pUi->actionSave_As->setEnabled( false );
    setWindowTitle( Rimy3D::applicationName() );
}

void MainWindow::on_actionEnglish_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_English );
}

void MainWindow::on_actionExit_triggered( void )
{
    close();
}

void MainWindow::on_actionGerman_triggered( void )
{
    Rimy3D::setLanguage( Rimy3D::ELanguage_German );
}

void MainWindow::on_actionOpen_triggered( void )
{
    QString strFilter = tr( "All files" ).append( " (*.obj *.3db *.gmax *.ase *.asc *.xact);;"
                                                  "Wavefront OBJ format (*.obj);;"
                                                  "Baltram's 3D format (*.3db);;"
                                                  "GMax Scene (*.gmax);;"
                                                  "3ds Max ASCII Scene (*.ase);;"
                                                  "Gothic ASCII Scene (*.asc);;"
                                                  "Gothic 3 Motion Actor (*.xact);;" );
    open( QFileDialog::getOpenFileName( this, tr( "Open" ), m_SceneInfo.getCurrentDir(), strFilter ) );
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

void MainWindow::onSceneChanged( void )
{
    m_pUi->widget->resetCamera();
    m_pUi->widget->setWorld( m_SceneInfo.buildGlcWorld() );
}

void MainWindow::on_actionConfigure_Bitmap_Paths_triggered( void )
{
    TextureFinder::getInstance().showDialog();
}

void MainWindow::on_actionSave_As_triggered( void )
{
    QString strFilter = tr( "All files" ).append( " (*.*);;"
                        "Wavefront OBJ format (*.obj);;"
                        "Baltram's 3D format (*.3db);;"
                        "3ds Max ASCII Scene (*.ase);;"
                        "Gothic ASCII Scene (*.asc);;"
                        "Gothic 3 Motion Actor (*.xact);;" );
    QString strFilePath = m_SceneInfo.getCurrentSaveDir() + QDir::separator() + m_SceneInfo.getCurrentFile();
    save( QFileDialog::getSaveFileName( this, tr( "Save As" ), strFilePath, strFilter ) );
}