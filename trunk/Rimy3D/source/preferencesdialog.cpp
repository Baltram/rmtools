#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "rimy3d.h"

PreferencesDialog & PreferencesDialog::getInstance( void )
{
    static PreferencesDialog * s_pInstance = new PreferencesDialog;
    return *s_pInstance;
}

bool PreferencesDialog::autoUpdate( void ) const
{
    return m_pUi->cbAutoUpdate->isChecked();
}

QString PreferencesDialog::defaultImageFileExt( void ) const
{
    return m_pUi->cobImgExt->currentText().toLower();
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
    m_pUi( new Ui::PreferencesDialog )
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
    int iAutoUpdate = a_Settings.value( "cbAutoUpdate", 3 ).toInt();
    if ( iAutoUpdate == 3 )
        iAutoUpdate = Rimy3D::showQuestion( tr( "Do you want Rimy3D to check for updates automatically?" ) ) ? 3 : 0;
    if ( iAutoUpdate > 1 )
        Rimy3D::checkForUpdates( iAutoUpdate == 3 );
    m_pUi->cbAutoUpdate->setChecked( iAutoUpdate != 0 );
    m_pUi->cbGizmos->setChecked( a_Settings.value( "cbGizmos", true ).toBool() );
    m_pUi->cbXmacCollisionMesh->setChecked( a_Settings.value( "cbXmacCollisionMesh", true ).toBool() );
    a_Settings.endGroup();
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
    pSettings->setValue( "cbAutoUpdate", 1 );
    pSettings->endGroup();
    loadSettings( *pSettings );
}

void PreferencesDialog::on_pbUpdate_clicked( void )
{
    Rimy3D::checkForUpdates( true, true );
}

void PreferencesDialog::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "Preferences" );
    a_Settings.setValue( "cobImgExt", m_pUi->cobImgExt->currentIndex() );
    a_Settings.setValue( "cbAscPrefixes", m_pUi->cbAscPrefixes->isChecked() );
    a_Settings.setValue( "cbAutoUpdate", m_pUi->cbAutoUpdate->isChecked() ? 2 : 0 );
    a_Settings.setValue( "cbGizmos", m_pUi->cbGizmos->isChecked() );
    a_Settings.setValue( "cbXmacCollisionMesh", m_pUi->cbXmacCollisionMesh->isChecked() );
    a_Settings.endGroup();
}
