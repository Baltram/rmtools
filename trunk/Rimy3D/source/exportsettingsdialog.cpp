#include "exportsettingsdialog.h"
#include "ui_exportsettingsdialog.h"
#include "rimy3d.h"
#include <QFileDialog>

exportSettingsDialog::exportSettingsDialog( QWidget * a_pParent, QString a_strExt, Flags a_Flags ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_strExt( a_strExt.toLower() ),
    m_pUi( new Ui::exportSettingsDialog )
{
    m_pUi->setupUi( this );
    m_pUi->w1->setVisible( a_Flags & Colors );
    m_pUi->w2->setVisible( a_Flags & ( NormalsRecalc | NormalsCalc | Normals ) );
    m_pUi->rbNKeep->setEnabled( !( a_Flags & NormalsRecalc ) );
    m_pUi->w3->setVisible( a_Flags & VertsOnly );
    m_pUi->w4->setVisible( a_Flags & BaseXact );
    m_pUi->w5->setVisible( a_Flags & CreateMtl );
    m_pUi->w6->setVisible( a_Flags & BaseXmac );
    m_pUi->w7->setVisible( !( a_Flags & NoTextures ) );
    m_pUi->w8->setVisible( a_Flags & Convex );
    m_pUi->cbTextureFormat->insertItems( 0, QStringList() << "TGA" << "JPG" << "PNG" << "BMP" );
    m_pUi->cbNormals->setEnabled( a_Flags & Normals );
    m_pUi->gbAutoSkin->setVisible( a_Flags & AutoSkin );
    updateLanguage();
    setFixedSize( 0, 0 );
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

exportSettingsDialog::~exportSettingsDialog( void )
{
    delete m_pUi;
}

int exportSettingsDialog::angle( void ) const
{
    return m_pUi->sbAngle->value();
}

bool exportSettingsDialog::anglesNotSgs( void ) const
{
    return m_pUi->rbNRecalcA->isChecked();
}

QString exportSettingsDialog::baseXact( void ) const
{
    return m_pUi->leBaseXact->text();
}

QString exportSettingsDialog::baseXmac( void ) const
{
    return m_pUi->leBaseXmac->text();
}

bool exportSettingsDialog::colors( void ) const
{
    return m_pUi->cbVColors->isChecked();
}

bool exportSettingsDialog::convex( void ) const
{
    return m_pUi->cbConvex->isChecked();
}

bool exportSettingsDialog::createMtl( void ) const
{
    return m_pUi->cbMtl->isChecked();
}

bool exportSettingsDialog::indirectMatching( void ) const
{
    return m_pUi->rbIndirectMatching->isChecked();
}

bool exportSettingsDialog::normals( void ) const
{
    return m_pUi->cbNormals->isChecked();
}

bool exportSettingsDialog::recalcNormals( void ) const
{
    return !m_pUi->rbNKeep->isChecked();
}

void exportSettingsDialog::setAutoSkinVisible( bool a_bVisible )
{
    m_pUi->gbAutoSkin->setVisible( a_bVisible );
}

QString exportSettingsDialog::textureImageFileExtension( void ) const
{
    if ( !m_pUi->cbSaveTextures->isChecked() )
        return "";
    return m_pUi->cbTextureFormat->currentText().toLower();
}

bool exportSettingsDialog::vertsOnly( void ) const
{
    return m_pUi->cbVertsOnly->isChecked();
}

void exportSettingsDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

void exportSettingsDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
    setWindowTitle( tr( "%1 Export" ).arg( m_strExt.toUpper() ) );
}

void exportSettingsDialog::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "exportSettingsDialog_" + m_strExt );
    m_pUi->cbVColors->setChecked( a_Settings.value( "cbVColors", false ).toBool() );
    m_pUi->cbNormals->setChecked( a_Settings.value( "cbNormals", true ).toBool() );
    m_pUi->rbNKeep->setChecked( a_Settings.value( "rbNKeep", false ).toBool() );
    m_pUi->rbNRecalcS->setChecked( a_Settings.value( "rbNRecalcS", false ).toBool() );
    m_pUi->rbNRecalcA->setChecked( a_Settings.value( "rbNRecalcA", true ).toBool() );
    m_pUi->sbAngle->setValue( a_Settings.value( "sbAngle", 70 ).toInt() );
    m_pUi->cbVertsOnly->setChecked( a_Settings.value( "cbVertsOnly", false ).toBool() );
    m_pUi->rbIndirectMatching->setChecked( a_Settings.value( "rbIndirectMatching", true ).toBool() );
    m_pUi->rbDirectMatching->setChecked( a_Settings.value( "rbDirectMatching", false ).toBool() );
    m_pUi->leBaseXact->setText( a_Settings.value( "leBaseXact" ).toString() );
    m_pUi->leBaseXmac->setText( a_Settings.value( "leBaseXmac" ).toString() );
    m_pUi->cbMtl->setChecked( a_Settings.value( "cbMtl", true ).toBool() );
    m_pUi->cbSaveTextures->setChecked( a_Settings.value( "cbSaveTextures", false ).toBool() );
    m_pUi->cbTextureFormat->setCurrentIndex( a_Settings.value( "cbTextureFormat", 0 ).toInt() );
    m_pUi->cbConvex->setChecked( a_Settings.value( "cbConvex", false ).toBool() );
    a_Settings.endGroup();
    if ( m_strExt == "_xmac" || m_strExt == "xact" )
    {
        m_pUi->sbAngle->setValue( 180 );
        m_pUi->rbNRecalcA->setChecked( true );
    }
}

void exportSettingsDialog::on_cbNormals_toggled( bool a_bIsChecked )
{
    m_pUi->gbVNRecalc->setEnabled( a_bIsChecked );
}

void exportSettingsDialog::on_cbSaveTextures_toggled( bool a_bChecked )
{
    m_pUi->cbTextureFormat->setEnabled( a_bChecked );
}

void exportSettingsDialog::on_cbVertsOnly_toggled( bool a_bChecked )
{
    m_pUi->gbAutoSkin->setEnabled( !a_bChecked );
}

void exportSettingsDialog::on_pbBaseXact_clicked( void )
{
    QString strFile = QFileDialog::getOpenFileName( this, tr( "Open" ), m_pUi->leBaseXact->text(), "Gothic 3 Motion Actor (*.xact);;" );
    if ( strFile != "" )
        m_pUi->leBaseXact->setText( QDir::toNativeSeparators( strFile ) );
}

void exportSettingsDialog::on_pbBaseXmac_clicked( void )
{
    QString strFile = QFileDialog::getOpenFileName( this, tr( "Open" ), m_pUi->leBaseXmac->text(), "Risen Motion Actor (*._xmac);;" );
    if ( strFile != "" )
        m_pUi->leBaseXmac->setText( QDir::toNativeSeparators( strFile ) );
}

void exportSettingsDialog::on_pbDefault_clicked( void )
{
    QSettings * pSettings = Rimy3D::getSettings();
    pSettings->beginGroup( "exportSettingsDialog_" + m_strExt );
    pSettings->remove( "" );
    pSettings->endGroup();
    loadSettings( *pSettings );
}

void exportSettingsDialog::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "exportSettingsDialog_" + m_strExt );
    a_Settings.setValue( "cbVColors", m_pUi->cbVColors->isChecked() );
    a_Settings.setValue( "cbNormals", m_pUi->cbNormals->isChecked() );
    a_Settings.setValue( "rbNKeep", m_pUi->rbNKeep->isChecked() );
    a_Settings.setValue( "rbNRecalcS", m_pUi->rbNRecalcS->isChecked() );
    a_Settings.setValue( "rbNRecalcA", m_pUi->rbNRecalcA->isChecked() );
    a_Settings.setValue( "sbAngle", m_pUi->sbAngle->value() );
    a_Settings.setValue( "cbVertsOnly", m_pUi->cbVertsOnly->isChecked() );
    a_Settings.setValue( "rbIndirectMatching", m_pUi->rbIndirectMatching->isChecked() );
    a_Settings.setValue( "rbDirectMatching", m_pUi->rbDirectMatching->isChecked() );
    a_Settings.setValue( "leBaseXact", m_pUi->leBaseXact->text() );
    a_Settings.setValue( "leBaseXmac", m_pUi->leBaseXmac->text() );
    a_Settings.setValue( "cbMtl", m_pUi->cbMtl->isChecked() );
    a_Settings.setValue( "cbSaveTextures", m_pUi->cbSaveTextures->isChecked() );
    a_Settings.setValue( "cbTextureFormat", m_pUi->cbTextureFormat->currentIndex() );
    a_Settings.setValue( "cbConvex", m_pUi->cbConvex->isChecked() );
    a_Settings.endGroup();
}
