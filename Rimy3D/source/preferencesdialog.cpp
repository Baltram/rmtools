#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "rimy3d.h"

PreferencesDialog::PreferencesDialog( QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
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

void PreferencesDialog::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "Preferences" );
    m_pUi->cobImgExt->setCurrentIndex( a_Settings.value( "cobImgExt", 0 ).toInt() );
    m_pUi->cbAscPrefixes->setChecked( a_Settings.value( "cbAscPrefixes", true ).toBool() );
    m_pUi->cbXmacCollisionMesh->setChecked( a_Settings.value( "cbXmacCollisionMesh", true ).toBool() );
    a_Settings.endGroup();
}

void PreferencesDialog::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "Preferences" );
    a_Settings.setValue( "cobImgExt", m_pUi->cobImgExt->currentIndex() );
    a_Settings.setValue( "cbAscPrefixes", m_pUi->cbAscPrefixes->isChecked() );
    a_Settings.setValue( "cbXmacCollisionMesh", m_pUi->cbXmacCollisionMesh->isChecked() );
    a_Settings.endGroup();
}

void PreferencesDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

void PreferencesDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
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
    pSettings->endGroup();
    loadSettings( *pSettings );
}
