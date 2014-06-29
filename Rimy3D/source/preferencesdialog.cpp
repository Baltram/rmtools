#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "rimy3d.h"
#include "Mimicry.h"
#include <QRadioButton>
#include <QDesktopwidget>

PreferencesDialog & PreferencesDialog::getInstance( void )
{
    static PreferencesDialog * s_pInstance = new PreferencesDialog;
    return *s_pInstance;
}

void PreferencesDialog::showAutoUpdateDialog( void )
{
    PreferencesDialog * pPreferencesDialog = &getInstance();
    QDialog Diag( pPreferencesDialog, Qt::CustomizeWindowHint | Qt::WindowTitleHint );
    Diag.setWindowTitle( Rimy3D::applicationName() );
    QVBoxLayout * pLayout = new QVBoxLayout( &Diag );
    QLabel * pLabel = new QLabel( tr( "Automatic check for updates" ), &Diag );
    QRadioButton * pRadio1Buttons[ 4 ];
    pRadio1Buttons[ 0 ] = new QRadioButton( tr( "Daily" ), &Diag );
    pRadio1Buttons[ 1 ] = new QRadioButton( tr( "Weekly" ), &Diag );
    pRadio1Buttons[ 2 ] = new QRadioButton( tr( "Monthly" ), &Diag );
    pRadio1Buttons[ 3 ] = new QRadioButton( tr( "Never" ), &Diag );
    QPushButton * pButton = new QPushButton( "OK", &Diag );
    pLayout->addWidget( pLabel );
    for ( int i = 0; i != 4; ++i )
    {
        pLayout->addWidget( pRadio1Buttons[ i ] );
        if ( i == pPreferencesDialog->m_iAutoUpdates )
            pRadio1Buttons[ i ]->setChecked( true );
    }
    pLayout->addWidget( pButton );
    Diag.setFixedSize( 0, 0 );
    connect( pButton, SIGNAL( clicked( void ) ), &Diag, SLOT( accept( void ) ) );
    if ( !pPreferencesDialog->isVisible() )
        Diag.move( QApplication::desktop()->availableGeometry().center() - QPoint( 94, 97 ) );
    if ( !Diag.exec() )
        return;
    for ( int i = 0; i != 4; ++i )
        if ( pRadio1Buttons[ i ]->isChecked() )
            pPreferencesDialog->m_iAutoUpdates = i;
    pPreferencesDialog->m_pUi->cbAutoUpdate->setChecked( pPreferencesDialog->m_iAutoUpdates != 3 );
}

bool PreferencesDialog::autoUpdate( void ) const
{
    return m_pUi->cbAutoUpdate->isChecked();
}

void PreferencesDialog::checkForUpdates( bool a_bManual, bool a_bInitial )
{
    if ( !a_bManual )
    {
        int iDaysSince = m_dateLastUpdateCheck.daysTo( QDate::currentDate() );
        if ( m_iAutoUpdates == 3 ) // never
            return;
        if ( m_iAutoUpdates == 2 && iDaysSince < 30 ) // monthly
            return;
        if ( m_iAutoUpdates == 1 && iDaysSince < 7 ) // weekly
            return;
        if ( m_iAutoUpdates == 0 && iDaysSince < 1 ) // daily
            return;
        QDate dateCurrent = QDate::currentDate();
        m_dateLastUpdateCheck.setDate( dateCurrent.year(), dateCurrent.month(), dateCurrent.day() );
    }
    Rimy3D::checkForUpdates( a_bInitial || a_bManual, a_bManual );
}

QString PreferencesDialog::defaultImageFileExt( void ) const
{
    return m_pUi->cobImgExt->currentText().toLower();
}

bool PreferencesDialog::lookUpTextures( void ) const
{
    return m_pUi->cbLookUpTextures->isChecked();
}

bool PreferencesDialog::removeAscPrefixes( void ) const
{
    return m_pUi->cbAscPrefixes->isChecked();
}

bool PreferencesDialog::removeXmacCollisionMesh( void ) const
{
    return m_pUi->cbXmacCollisionMesh->isChecked();
}

bool PreferencesDialog::showTransformGizmos( void ) const
{
    return m_pUi->cbGizmos->isChecked();
}

void PreferencesDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

PreferencesDialog::PreferencesDialog( void ) :
    QDialog( 0, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::PreferencesDialog ),
    m_iAutoUpdates( 0 ),
    m_dateLastUpdateCheck( 1900, 1, 1 )
{
    m_pUi->setupUi( this );
    m_pUi->cobImgExt->addItems( QStringList() << "JPG" << "PNG" << "GIF" << "BMP" );
    setFixedSize( size() );
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

PreferencesDialog::~PreferencesDialog( void )
{
    delete m_pUi;
}

void PreferencesDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
}

void PreferencesDialog::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "Preferences" );
    m_pUi->cobImgExt->setCurrentIndex( a_Settings.value( "cobImgExt", 0 ).toInt() );
    m_pUi->cbAscPrefixes->setChecked( a_Settings.value( "cbAscPrefixes", true ).toBool() );
    m_pUi->cbGizmos->setChecked( a_Settings.value( "cbGizmos", true ).toBool() );
    m_pUi->cbXmacCollisionMesh->setChecked( a_Settings.value( "cbXmacCollisionMesh", true ).toBool() );
    m_pUi->cbLookUpMaterials->setChecked( a_Settings.value( "cbLookUpMaterials", true ).toBool() );
    m_pUi->cbLookUpTextures->setChecked( a_Settings.value( "cbLookUpTextures", true ).toBool() );
    m_iAutoUpdates = a_Settings.value( "iAutoUpdates", -1 ).toInt();
    m_dateLastUpdateCheck = a_Settings.value( "dateLastUpdateCheck", QDate( 1900, 1, 1 ) ).toDate();
    a_Settings.endGroup();
    if ( m_iAutoUpdates == -1 )
    {
        m_iAutoUpdates = 0;
        showAutoUpdateDialog();
        checkForUpdates( false, true );
    }
    else
        checkForUpdates( false, false );
    m_pUi->cbAutoUpdate->setChecked( m_iAutoUpdates != 3 );
}

void PreferencesDialog::on_cbAutoUpdate_clicked( void )
{
    m_pUi->cbAutoUpdate->toggle();
    showAutoUpdateDialog();
}

void PreferencesDialog::on_cbLookUpMaterials_toggled( bool a_bChecked )
{
    mCGenomeMaterial::AccessMaterialLookupHint() = a_bChecked;
}

void PreferencesDialog::on_pbLookUpMaterials_clicked( void )
{
    emit materialLookupRequested();
}

void PreferencesDialog::on_pbOk_clicked( void )
{
    close();
}

void PreferencesDialog::on_pbRestoreDefaults_clicked( void )
{
    QSettings * pSettings = Rimy3D::getSettings();
    pSettings->beginGroup( "Preferences" );
    pSettings->remove( "" );
    pSettings->setValue( "iAutoUpdates", m_iAutoUpdates );
    pSettings->setValue( "dateLastUpdateCheck", m_dateLastUpdateCheck );
    pSettings->endGroup();
    loadSettings( *pSettings );
}

void PreferencesDialog::on_pbUpdate_clicked( void )
{
    checkForUpdates( true, false );
}

void PreferencesDialog::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "Preferences" );
    a_Settings.setValue( "cobImgExt", m_pUi->cobImgExt->currentIndex() );
    a_Settings.setValue( "cbAscPrefixes", m_pUi->cbAscPrefixes->isChecked() );
    a_Settings.setValue( "cbGizmos", m_pUi->cbGizmos->isChecked() );
    a_Settings.setValue( "cbXmacCollisionMesh", m_pUi->cbXmacCollisionMesh->isChecked() );
    a_Settings.setValue( "cbLookUpMaterials", m_pUi->cbLookUpMaterials->isChecked() );
    a_Settings.setValue( "cbLookUpTextures", m_pUi->cbLookUpTextures->isChecked() );
    a_Settings.setValue( "iAutoUpdates", m_iAutoUpdates );
    a_Settings.setValue( "dateLastUpdateCheck", m_dateLastUpdateCheck );
    a_Settings.endGroup();
}
