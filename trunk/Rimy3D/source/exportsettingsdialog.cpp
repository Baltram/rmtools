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

void exportSettingsDialog::setAutoSkinEnabled( bool a_bEnabled )
{
    m_pUi->gbAutoSkin->setEnabled( a_bEnabled );
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
    a_Settings.endGroup();
}

void exportSettingsDialog::on_cbNormals_toggled( bool a_bIsChecked )
{
    m_pUi->gbVNRecalc->setEnabled( a_bIsChecked );
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
    a_Settings.endGroup();
}
