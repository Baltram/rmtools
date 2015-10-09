#include "batchdialog.h"
#include "ui_batchdialog.h"
#include "mainwindow.h"
#include <GLC_World>
#include <QFileDialog>
#include <QtCore>
#include <QCloseEvent>
#include <QGLContext>

QGLContext * BatchDialog::s_pContext = 0;

BatchDialog & BatchDialog::getInstance( void )
{
    static BatchDialog * s_pInstance = new BatchDialog;
    return *s_pInstance;
}

void BatchDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

void BatchDialog::closeEvent( QCloseEvent * a_pEvent )
{
    if ( m_bRunning )
    {
        a_pEvent->ignore();
        if ( !m_bAborted )
            abort();
        return;
    }
    m_pUi->lwSourceFiles->clear();
    m_pUi->pteLog->clear();
    stateChanged();
    QDialog::closeEvent( a_pEvent );
}

BatchDialog::BatchDialog( void ) :
    QDialog( 0, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::BatchDialog ),
    m_bRunning( false ),
    m_bAborted( false )
{
    m_pUi->setupUi( this );
    m_pUi->cobOutputFileFormat->addItems( QStringList() << "Baltram's 3D format (*.3db)" << "3D Studio Mesh (*.3ds)" << "Wavefront OBJ format (*.obj)" << "3ds Max ASCII Scene (*.ase)" << "Gothic ASCII Scene (*.asc)" << "Gothic 3 Mesh (*.xcmsh)" << "Gothic 3 Motion Actor (*.xact)" << "Gothic 3 LOD Mesh (*.xlmsh)" << "Gothic 3 Collision Mesh (*.xnvmsh)" << "Risen Mesh (*._xmsh)" << "Risen Motion Actor (*._xmac)" << "Risen Collision Mesh (*._xcom)" );
    qRegisterMetaType< Rimy3D::EMessage >( "Rimy3D::EMessage" );
    connect( Rimy3D::getInstance(), SIGNAL( message( QString const &, Rimy3D::EMessage ) ), this, SLOT( onMessage( QString const &, Rimy3D::EMessage ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

BatchDialog::~BatchDialog( void )
{
    delete m_pUi;
}

void BatchDialog::abort( void )
{
    if ( !m_bRunning )
        return;
    m_bAborted = true;
    Rimy3D::showWarning( tr( "Aborting batch conversion..." ) );
}

void BatchDialog::addFilesFromDirectory( QDir const & a_Dir )
{
    bool bSortingEnabled = m_pUi->lwSourceFiles->isSortingEnabled();
    m_pUi->lwSourceFiles->setSortingEnabled( false );
    QFileInfoList arrFiles = a_Dir.entryInfoList( QStringList() << "*.3db" << "*.3ds" << "*.obj" << "*.ase" << "*.asc" << "*.xcmsh" << "*.xact" << "*.xlmsh" << "*.xnvmsh" << "*._xmsh" << "*._xmac" << "*._xcom", QDir::Files | QDir::Readable );
    for ( int i = 0, ie = arrFiles.count(); i != ie; ++i )
        m_pUi->lwSourceFiles->addItem( QDir::toNativeSeparators( arrFiles.at( i ).absoluteFilePath() ) );
    if ( m_pUi->cbIncludeSubdirs->isChecked() )
    {
        QFileInfoList arrSubDirs = a_Dir.entryInfoList( QStringList(), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable );
        for ( int i = 0, ie = arrSubDirs.count(); i != ie; ++i )
            addFilesFromDirectory( QDir( arrSubDirs.at( i ).absoluteFilePath( )) );
    }
    m_pUi->lwSourceFiles->setSortingEnabled( bSortingEnabled );
}

void BatchDialog::run( exportSettingsDialog const * a_pDialog ) const
{
    if ( s_pContext )
        s_pContext->makeCurrent();
    QStringList arrPaths;
    for ( int i = 0, ie = m_pUi->lwSourceFiles->count(); i != ie; ++i )
        arrPaths << m_pUi->lwSourceFiles->item( i )->text();
    QDir dirOut( m_pUi->leOutputDirectory->text() );
    bool bOutputToSourceDirs = m_pUi->cbOutputToSourceDirs->isChecked();
    bool bDeleteOriginals = m_pUi->cbDeleteOriginals->isChecked();
    QString strExt = m_pUi->cobOutputFileFormat->currentText().replace( QRegExp( ".*\\(\\*\\.(.*)\\)" ), "\\1" );
    SceneInfo sceneInfo;
    int iSuccessCount = 0, iErrorCount = 0;
    for ( int i = 0, ie = arrPaths.count(); i != ie; ++i, Rimy3D::showMessage( "" ) )
    {
        if ( m_bAborted )
            break;
        ++iErrorCount;
        QString strPath = arrPaths[ i ], strFilePath = QFileInfo( strPath ).absolutePath(), strBaseName = QFileInfo( strPath ).baseName();
        Rimy3D::showMessage( tr( "Processing file %1 of %2 - %3" ).arg( QString::number( i + 1 ), QString::number( ie ), strPath ) );
        if ( !sceneInfo.openSceneFile( strPath ) )
            continue;
        GLC_World World;
        if ( a_pDialog->textureImageFileExtension() != "" )
            World = sceneInfo.buildGlcWorld();
        QString strOutFile = strFilePath + "/" + strBaseName + "." + strExt;
        if ( !bOutputToSourceDirs )
            strOutFile = dirOut.absolutePath() + "/" + strBaseName + "." + strExt;
        strOutFile = QDir::toNativeSeparators( QFileInfo( strOutFile ).absoluteFilePath() );
        if ( !sceneInfo.saveSceneFile( strOutFile, *a_pDialog, &World ) )
            continue;
        --iErrorCount;
        ++iSuccessCount;
        Rimy3D::showMessage( tr( "Saved as %1" ).arg( strOutFile ) );
        if ( bDeleteOriginals )
        {
            if ( !QFile::remove( strPath ) )
                Rimy3D::showWarning( tr( "Could not delete original file." ) );
            else
                Rimy3D::showMessage( tr( "Deleted %1" ).arg( strPath ) );
        }
    }
    Rimy3D::showMessage( tr( "Done. Converted: %2 of %1 - Errors: %3 of %1" ).arg( QString::number( arrPaths.count() ), QString::number( iSuccessCount ), QString::number( iErrorCount ) ) );
    return;
}

void BatchDialog::stateChanged( void )
{
    bool bEnabled = !m_bRunning;
    foreach ( QObject * pChild, children() )
        if ( dynamic_cast< QWidget * >( pChild ) && pChild != m_pUi->pbCancel )
            dynamic_cast< QWidget * >( pChild )->setEnabled( bEnabled );
    if ( m_bRunning )
    {
        m_pUi->pbCancel->setEnabled( true );
        m_pUi->pteLog->setEnabled( true );
        m_pUi->lbLog->setEnabled( true );
        m_pUi->lwSourceFiles->setEnabled( true );
        m_pUi->lbSourceFiles->setEnabled( true );
        return;
    }
    bEnabled = m_pUi->lwSourceFiles->count() != 0;
    m_pUi->pbRemove->setEnabled( bEnabled );
    m_pUi->pbRemoveAll->setEnabled( bEnabled );
    m_pUi->pbStart->setEnabled( bEnabled );
    bEnabled = !m_pUi->cbOutputToSourceDirs->isChecked();
    m_pUi->lbOutputDirectory->setEnabled( bEnabled );
    m_pUi->leOutputDirectory->setEnabled( bEnabled );
    m_pUi->pbChooseOutputFolder->setEnabled( bEnabled );
}

void BatchDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
}

void BatchDialog::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "BatchDialog" );
    m_pUi->cbIncludeSubdirs->setChecked( a_Settings.value( "cbIncludeSubdirs", true ).toBool() );
    m_pUi->cbOutputToSourceDirs->setChecked( a_Settings.value( "cbOutputToSourceDirs", true ).toBool() );
    m_pUi->cobOutputFileFormat->setCurrentIndex( a_Settings.value( "cobOutputFileFormat", 0 ).toInt() );
    m_pUi->leOutputDirectory->setText( a_Settings.value( "leOutputDirectory", "" ).toString() );
    a_Settings.endGroup();
    stateChanged();
}

void BatchDialog::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "BatchDialog" );
    a_Settings.setValue( "cbIncludeSubdirs", m_pUi->cbIncludeSubdirs->isChecked() );
    a_Settings.setValue( "cbOutputToSourceDirs", m_pUi->cbOutputToSourceDirs->isChecked() );
    a_Settings.setValue( "cobOutputFileFormat", m_pUi->cobOutputFileFormat->currentIndex() );
    a_Settings.setValue( "leOutputDirectory", m_pUi->leOutputDirectory->text() );
    a_Settings.endGroup();
}

void BatchDialog::on_cbOutputToSourceDirs_clicked( void )
{
    stateChanged();
}

void BatchDialog::on_pbAddDirectory_clicked( void )
{
    static QString s_strDir;
    QString strDir = QFileDialog::getExistingDirectory( this, tr( "Choose Source Directory" ), s_strDir == "" ? QDir::homePath() : s_strDir );
    if ( strDir == "" )
        return;
    addFilesFromDirectory( strDir );
    s_strDir = strDir;
    stateChanged();
}

void BatchDialog::on_pbAddFiles_clicked( void )
{
    static QString s_strDir;
    QStringList arrFiles = QFileDialog::getOpenFileNames( this, tr( "Choose Source Files" ), s_strDir == "" ? QDir::homePath() : s_strDir, MainWindow::getOpenFilter() );
    if ( arrFiles.count() == 0 )
        return;
    s_strDir = QFileInfo( arrFiles.front() ).absolutePath();
    m_pUi->lwSourceFiles->addItems( arrFiles );
    stateChanged();
}

void BatchDialog::on_pbCancel_clicked( void )
{
    close();
}

void BatchDialog::on_pbChooseOutputFolder_clicked( void )
{
    QString strDir = QFileDialog::getExistingDirectory( this, tr( "Choose Output Directory" ), QDir::homePath() );
    if ( strDir != "" )
        m_pUi->leOutputDirectory->setText( strDir );
}

void BatchDialog::on_pbRemove_clicked( void )
{
    QList< QListWidgetItem * > arrSelectedItems = m_pUi->lwSourceFiles->selectedItems();
    for ( int i = arrSelectedItems.count(); i--; delete arrSelectedItems[ i ] );
    stateChanged();
}

void BatchDialog::on_pbRemoveAll_clicked( void )
{
    m_pUi->lwSourceFiles->clear();
    stateChanged();
}

void BatchDialog::on_pbStart_clicked( void )
{
    if ( m_bRunning )
        return;
    if ( !m_pUi->cbOutputToSourceDirs->isChecked() && !QDir( m_pUi->leOutputDirectory->text() ).exists() )
    {
        Rimy3D::showError( tr( "Output directory does not exist." ) );
        return;
    }
    QString strExt = m_pUi->cobOutputFileFormat->currentText().replace( QRegExp( ".*\\(\\*\\.(.*)\\)" ), "\\1" );
    exportSettingsDialog const * pDialog = MainWindow::getInstance().getExportSettings( "dummy." + strExt, false );
    if ( !pDialog )
        return;
    m_bRunning = true;
    Rimy3D::setQuiet( true );
    m_bAborted = false;
    m_pUi->pteLog->clear();
    stateChanged();
    static QFuture< void > s_Future;
    static QFutureWatcher< void > s_FutureWatcher;
    connect( &s_FutureWatcher, SIGNAL( finished( void ) ), this, SLOT( onFinished( void ) ) );
    if ( !s_pContext )
    {
        s_pContext = new QGLContext( QGLFormat(), this );
        s_pContext->create();
    }
    s_Future = QtConcurrent::run( this, &BatchDialog::run, pDialog );
    s_FutureWatcher.setFuture( s_Future );
}

void BatchDialog::onFinished( void )
{
    m_bRunning = false;
    Rimy3D::setQuiet( false );
    stateChanged();
}

void BatchDialog::onMessage( QString const & a_strText, Rimy3D::EMessage a_enuType )
{
    if ( !m_bRunning )
        return;
    m_pUi->pteLog->setTextColor( QColor( "black" ) );
    if ( a_enuType == Rimy3D::EMessage_Error )
        m_pUi->pteLog->setTextColor( QColor( "red" ) );
    else if ( a_enuType == Rimy3D::EMessage_Warning )
        m_pUi->pteLog->setTextColor( QColor( "blue" ) );
    m_pUi->pteLog->append( a_strText );

}
