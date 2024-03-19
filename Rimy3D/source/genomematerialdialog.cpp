#include "genomematerialdialog.h"
#include "ui_genomematerialdialog.h"
#include "rimy3d.h"
#include "sceneinfo.h"
#include "Mimicry.h"
#include <QFileDialog>

namespace
{
    void UpdateFloatValue( QDoubleSpinBox const & a_SpinBox, float & a_fDest )
    {
        if ( abs( a_SpinBox.value() - a_fDest ) > pow( 0.1, a_SpinBox.decimals() ) )
            a_fDest = a_SpinBox.value();
    }
}

GenomeMaterialDialog::GenomeMaterialDialog( SceneInfo & a_SceneInfo, QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pMaterial( 0 ),
    m_SceneInfo( a_SceneInfo ),
    m_pUi( new Ui::GenomeMaterialDialog )
{
    m_pUi->setupUi( this );
    m_pUi->gbBoolEdit->setVisible( false );
    m_pUi->gbCharEdit->setVisible( false );
    m_pUi->gbEnumEdit->setVisible( false );
    m_pUi->gbEulerAnglesEdit->setVisible( false );
    m_pUi->gbFloatColorEdit->setVisible( false );
    m_pUi->gbFloatEdit->setVisible( false );
    m_pUi->gbIntEdit->setVisible( false );
    m_pUi->gbStringEdit->setVisible( false );
    m_pUi->gbVec2Edit->setVisible( false );
    closeMaterial();
}

GenomeMaterialDialog::~GenomeMaterialDialog( void )
{
    delete m_pMaterial;
    delete m_pUi;
}

void GenomeMaterialDialog::closeMaterial( void )
{
    delete m_pMaterial;
    m_pMaterial = 0;
    setWindowTitle( tr( "Genome Material Editor" ) );
    m_pUi->lbShaderElement->setEnabled( false );
    m_pUi->lbProperty->setEnabled( false );
    m_pUi->cbShaderElement->setEnabled( false );
    m_pUi->cbProperty->setEnabled( false );
    m_pUi->pbSaveAs->setEnabled( false );
    m_pUi->cbShaderElement->clear();
    updateActiveGroupBox();
}

bool GenomeMaterialDialog::openMaterial( QString const & a_strMaterialName )
{
    closeMaterial();
    m_pMaterial = new mCGenomeMaterial;
    QFileInfo fileSource( a_strMaterialName );
    if ( ( fileSource.suffix().toLower() == "xshmat" || !mCGenomeMaterial::FindRisenMaterial( a_strMaterialName.toAscii().data(), *m_pMaterial ) ) &&
         ( fileSource.suffix().toLower() == "_xmat" || !mCGenomeMaterial::FindGothic3Material( a_strMaterialName.toAscii().data(), *m_pMaterial ) ) )
    {
        delete m_pMaterial;
        m_pMaterial = 0;
        Rimy3D::showError( tr( "Could not find/open the material." ), windowTitle() );
        return false;
    }
    setWindowTitle( fileSource.baseName() + ( m_pMaterial->GetShaderElementType( mCGenomeMaterial::ESpecialShaderElementIndex_MaterialResourceObject ).Contains( "PS" ) ? ".xshmat" : "._xmat" ) );
    m_pUi->lbShaderElement->setEnabled( true );
    m_pUi->lbProperty->setEnabled( true );
    m_pUi->cbShaderElement->setEnabled( true );
    m_pUi->cbProperty->setEnabled( true );
    m_pUi->pbSaveAs->setEnabled( true );
    QStringList arrItems;
    for ( unsigned int u = 0, ue = m_pMaterial->GetShaderElementCount(); u != ue; ++u )
    {
        QString strShaderElement = QString::number( u ).rightJustified( 2, '0' ) + " " + m_pMaterial->GetShaderElementType( u ).GetText();
        mTArray< MIUInt > arrReferencedShaderElementIndices;
        m_pMaterial->GetReferencedShaderElements( u, arrReferencedShaderElementIndices );
        if ( arrReferencedShaderElementIndices.GetCount() )
        {
            strShaderElement += " -> {";
            for ( unsigned int v = 0, ve = arrReferencedShaderElementIndices.GetCount(); v != ve; ++v )
                if ( arrReferencedShaderElementIndices[ v ] != MI_DW_INVALID )
                    strShaderElement += QString::number( arrReferencedShaderElementIndices[ v ] ).rightJustified( 2, '0' ) + ", ";
            strShaderElement[ strShaderElement.length() - 2 ] = '}';
            strShaderElement.truncate( strShaderElement.length() - 1 );
        }
        MILPCChar arrColorSourceTypeStrings[ mCGenomeMaterial::EColorSourceType_Count ] =
        {
            "absorbation",
            "alpha dissolve",
            "clouds",
            "diffuse",
            "distortion",
            "flowing bump",
            "light emission",
            "normal",
            "opacity",
            "reflection",
            "self-illumination",
            "specular",
            "specular power",
            "static bump",
        };
        mCGenomeMaterial::EColorSourceType enumColoRourceType = m_pMaterial->GetColorSourceType( u );
        if ( enumColoRourceType != mCGenomeMaterial::EColorSourceType_Invalid )
            strShaderElement += QString( " (" ) + arrColorSourceTypeStrings[ enumColoRourceType ] + ")";
        arrItems << strShaderElement;
    }
    arrItems << ( "** " + m_pMaterial->GetShaderElementType( mCGenomeMaterial::ESpecialShaderElementIndex_MaterialResourceObject ) ).GetText();
    arrItems << ( "** " + m_pMaterial->GetShaderElementType( mCGenomeMaterial::ESpecialShaderElementIndex_ShaderObject ) ).GetText();
    m_pUi->cbShaderElement->insertItems( 0, arrItems );
    updateActiveGroupBox();
    return true;
}

void GenomeMaterialDialog::updateMaterialSuggestions( void )
{
    QStringList arrMaterialNames;
    mCScene const & sceneCurrent = m_SceneInfo.getCurrentScene();
    for ( unsigned int u = 0, ue = sceneCurrent.GetNumMaterials(); u != ue; ++u )
    {
        mCMaterial const * pMaterial = dynamic_cast< mCMaterial const * >( sceneCurrent.GetMaterialAt( u ) );
        mCMultiMaterial const * pMultiMaterial = dynamic_cast< mCMultiMaterial const * >( sceneCurrent.GetMaterialAt( u ) );
        if ( pMaterial )
            arrMaterialNames.append( pMaterial->GetName().GetText() );
        if ( pMultiMaterial )
            for ( unsigned int v = 0, ve = pMultiMaterial->GetSubMaterials().GetCount(); v != ve; ++v )
                arrMaterialNames.append( pMultiMaterial->GetSubMaterials()[ v ].GetName().GetText() );
    }
    QString strExt = QFileInfo( m_SceneInfo.getCurrentFile() ).suffix().toLower();
    if ( strExt == "xcmsh" || strExt == "xact" || strExt == "xlmsh" )
        strExt = ".xshmat";
    else if ( strExt == "_xmsh" || strExt == "_xmac" )
        strExt = "._xmat";
    else
        strExt = "";
    for ( int i = arrMaterialNames.count(); i--; arrMaterialNames[ i ] += strExt );
    m_pUi->cbMaterialName->clear();
    m_pUi->cbMaterialName->insertItems( 0, arrMaterialNames );
}

void GenomeMaterialDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

void GenomeMaterialDialog::closeEvent( QCloseEvent * a_pEvent )
{
    QDialog::closeEvent( a_pEvent );
    closeMaterial();
}

bool GenomeMaterialDialog::getActivePropertyIndex( unsigned int & a_uDest )
{
    unsigned int uShaderElementIndex;
    if ( !m_pMaterial || m_pUi->cbProperty->count() == 0 || !getActiveShaderElementIndex( uShaderElementIndex ) )
        return false;
    a_uDest = m_pUi->cbProperty->currentIndex();
    if ( a_uDest < m_pMaterial->GetPropertyCount( uShaderElementIndex ) )
        return true;
    return false;
}

bool GenomeMaterialDialog::getActiveShaderElementIndex( unsigned int & a_uDest )
{
    if ( !m_pMaterial || m_pUi->cbShaderElement->count() == 0 )
        return false;
    int iShaderElementIndex = m_pUi->cbShaderElement->currentIndex();
    if ( iShaderElementIndex == m_pUi->cbShaderElement->count() - 2 )
        a_uDest = mCGenomeMaterial::ESpecialShaderElementIndex_MaterialResourceObject;
    else if ( iShaderElementIndex == m_pUi->cbShaderElement->count() - 1 )
        a_uDest = mCGenomeMaterial::ESpecialShaderElementIndex_ShaderObject;
    else if ( ( a_uDest = iShaderElementIndex ) >= m_pMaterial->GetShaderElementCount() )
        return false;
    return true;
}

void GenomeMaterialDialog::syncProperty( unsigned int a_uShaderElementIndex, unsigned int a_uPropertyIndex, QGroupBox * a_pActiveGroupBox, bool a_bWrite )
{
    mCVariant varValue;
    mCGenomeMaterial::SProperty const & Property = m_pMaterial->GetProperty( a_uShaderElementIndex, a_uPropertyIndex );
    bool bReadOnly = m_pMaterial->IsReadOnly( a_uShaderElementIndex, a_uPropertyIndex );
    a_pActiveGroupBox->setEnabled( !bReadOnly );
    if ( a_pActiveGroupBox == m_pUi->gbBoolEdit )
    {
        if ( a_bWrite )
            varValue.SetData< MIBool >( m_pUi->cbBool->isChecked() );
        else
            m_pUi->cbBool->setChecked( Property.m_varValue.GetData< MIBool >() );
    }
    else if ( a_pActiveGroupBox == m_pUi->gbCharEdit )
    {
        if ( a_bWrite )
            varValue.SetData< MIU8 >( static_cast< MIU8 >( m_pUi->sbChar->value() ) );
        else
            m_pUi->sbChar->setValue( Property.m_varValue.GetData< MIU8 >() );

    }
    else if ( a_pActiveGroupBox == m_pUi->gbEnumEdit )
    {
        mCGenomeMaterial::SEnumContainer EnumContainer = Property.m_varValue.GetData< mCGenomeMaterial::SEnumContainer >();
        if ( a_bWrite )
        {
            if ( m_pUi->cbEnum->isEnabled() && m_pUi->cbEnum->count() != 0 )
                EnumContainer.m_uValue = static_cast< MIUInt >( m_pUi->cbEnum->currentIndex() );
            varValue.SetData< mCGenomeMaterial::SEnumContainer >( EnumContainer );
        }
        else
        {
            m_pUi->cbEnum->clear();
            a_pActiveGroupBox->setEnabled( false );
            if ( EnumContainer.m_uCount != 0 && EnumContainer.m_uValue < EnumContainer.m_uCount )
            {
                a_pActiveGroupBox->setEnabled( !bReadOnly );
                QStringList arrEnumStrings;
                for ( unsigned int u = 0; u != EnumContainer.m_uCount; ++u )
                    arrEnumStrings << EnumContainer.m_pEnumNames[ u ];
                m_pUi->cbEnum->insertItems( 0, arrEnumStrings );
                m_pUi->cbEnum->setCurrentIndex( EnumContainer.m_uValue );
            }
        }
    }
    else if ( a_pActiveGroupBox == m_pUi->gbEulerAnglesEdit )
    {
        mCVec3 vecEulerAngles = Property.m_varValue.GetData< mCVec3 >();
        if ( a_bWrite )
        {
            UpdateFloatValue( *m_pUi->dsbYaw, vecEulerAngles.AccessX() );
            UpdateFloatValue( *m_pUi->dsbPitch, vecEulerAngles.AccessY() );
            UpdateFloatValue( *m_pUi->dsbRoll, vecEulerAngles.AccessZ() );
            varValue.SetData< mCVec3 >( vecEulerAngles );
        }
        else
        {
            m_pUi->dsbYaw->setValue( vecEulerAngles.GetX() );
            m_pUi->dsbPitch->setValue( vecEulerAngles.GetY() );
            m_pUi->dsbRoll->setValue( vecEulerAngles.GetZ() );
        }
    }
    else if ( a_pActiveGroupBox == m_pUi->gbFloatColorEdit )
    {
        mCVec3 vecColor = Property.m_varValue.GetData< mCVec3 >();
        if ( a_bWrite )
        {
            UpdateFloatValue( *m_pUi->dsbRed, vecColor.AccessX() );
            UpdateFloatValue( *m_pUi->dsbGreen, vecColor.AccessY() );
            UpdateFloatValue( *m_pUi->dsbBlue, vecColor.AccessZ() );
            varValue.SetData< mCVec3 >( vecColor );
        }
        else
        {
            m_pUi->dsbRed->setValue( vecColor.GetX() );
            m_pUi->dsbGreen->setValue( vecColor.GetY() );
            m_pUi->dsbBlue->setValue( vecColor.GetZ() );
        }
    }
    else if ( a_pActiveGroupBox == m_pUi->gbFloatEdit )
    {
        float fValue = Property.m_varValue.GetData< MIFloat >();
        if ( a_bWrite )
        {
            UpdateFloatValue( *m_pUi->dsbFloat, fValue );
            varValue.SetData< MIFloat >( fValue );
        }
        else
            m_pUi->dsbFloat->setValue( fValue );
    }
    else if ( a_pActiveGroupBox == m_pUi->gbIntEdit )
    {
        if ( a_bWrite )
            varValue.SetData< MII32 >( m_pUi->sbInt->value() );
        else
            m_pUi->sbInt->setValue( Property.m_varValue.GetData< MII32 >() );
    }
    else if ( a_pActiveGroupBox == m_pUi->gbStringEdit )
    {
        if ( a_bWrite )
            varValue.SetData< mCString >( m_pUi->leString->text().toAscii().data() );
        else
            m_pUi->leString->setText( Property.m_varValue.GetData< mCString >().GetText() );
    }
    else if ( a_pActiveGroupBox == m_pUi->gbVec2Edit )
    {
        mCVec3 vec2D = Property.m_varValue.GetData< mCVec3 >();
        if ( a_bWrite )
        {
            UpdateFloatValue( *m_pUi->dsbX, vec2D.AccessX() );
            UpdateFloatValue( *m_pUi->dsbY, vec2D.AccessY() );
            varValue.SetData< mCVec3 >( vec2D );
        }
        else
        {
            m_pUi->dsbX->setValue( vec2D.GetX() );
            m_pUi->dsbY->setValue( vec2D.GetY() );
        }
    }
    else
        return;
    if ( a_bWrite && !bReadOnly )
        m_pMaterial->SetProperty( a_uShaderElementIndex, a_uPropertyIndex, varValue );
}

void GenomeMaterialDialog::updateActiveGroupBox( void )
{
    static QGroupBox * s_pActiveEditGroupBox = 0;
    static unsigned int s_uShaderElementIndex, s_uPropertyIndex;
    if ( s_pActiveEditGroupBox )
    {
        if ( m_pMaterial )
            syncProperty( s_uShaderElementIndex, s_uPropertyIndex, s_pActiveEditGroupBox, true );
        s_pActiveEditGroupBox->setVisible( false );
    }
    s_pActiveEditGroupBox = 0;
    if ( m_pMaterial && getActiveShaderElementIndex( s_uShaderElementIndex ) && getActivePropertyIndex( s_uPropertyIndex ) )
    {
        mCGenomeMaterial::SProperty const & Property = m_pMaterial->GetProperty( s_uShaderElementIndex, s_uPropertyIndex );
        if ( Property.m_varValue.IsOfType< MIBool >() )
            s_pActiveEditGroupBox = m_pUi->gbBoolEdit;
        else if ( Property.m_varValue.IsOfType< MIU8 >() )
            s_pActiveEditGroupBox = m_pUi->gbCharEdit;
        else if ( Property.m_varValue.IsOfType< mCGenomeMaterial::SEnumContainer >() )
            s_pActiveEditGroupBox = m_pUi->gbEnumEdit;
        else if ( Property.m_varValue.IsOfType< mCVec3 >() && Property.m_strType == "bCEulerAngles" )
            s_pActiveEditGroupBox = m_pUi->gbEulerAnglesEdit;
        else if ( Property.m_varValue.IsOfType< mCVec3 >() && Property.m_strType == "bCFloatColor" )
            s_pActiveEditGroupBox = m_pUi->gbFloatColorEdit;
        else if ( Property.m_varValue.IsOfType< MIFloat >() )
            s_pActiveEditGroupBox = m_pUi->gbFloatEdit;
        else if ( Property.m_varValue.IsOfType< MII32 >() )
            s_pActiveEditGroupBox = m_pUi->gbIntEdit;
        else if ( Property.m_varValue.IsOfType< mCString >() )
            s_pActiveEditGroupBox = m_pUi->gbStringEdit;
        else if ( Property.m_varValue.IsOfType< mCVec3 >() && Property.m_strType == "bCVector2" )
            s_pActiveEditGroupBox =  m_pUi->gbVec2Edit;
        if ( s_pActiveEditGroupBox )
        {
            syncProperty( s_uShaderElementIndex, s_uPropertyIndex, s_pActiveEditGroupBox, false );
            s_pActiveEditGroupBox->setVisible( true );
        }
    }
    setFixedSize( 450, minimumSizeHint().height() );
}

void GenomeMaterialDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
}

void GenomeMaterialDialog::on_cbMaterialName_textChanged( QString const & a_strText )
{
    m_pUi->pbOpen->setEnabled( a_strText.length() != 0 );
}

void GenomeMaterialDialog::on_cbProperty_currentIndexChanged( int )
{
    updateActiveGroupBox();
}

void GenomeMaterialDialog::on_cbShaderElement_currentIndexChanged( int )
{
    m_pUi->cbProperty->clear();
    if ( !m_pMaterial )
        return;
    unsigned int uShaderElementIndex;
    if ( getActiveShaderElementIndex( uShaderElementIndex ) )
    {
        QStringList arrProperties;
        for ( unsigned int u = 0, ue = m_pMaterial->GetPropertyCount( uShaderElementIndex ); u != ue; ++u )
            arrProperties << m_pMaterial->GetProperty( uShaderElementIndex, u ).m_strName.GetText();
        m_pUi->cbProperty->insertItems( 0, arrProperties );
    }
    updateActiveGroupBox();
}

void GenomeMaterialDialog::on_pbCancel_clicked( void )
{
    close();
}

void GenomeMaterialDialog::on_pbOpen_clicked( void )
{
    openMaterial( m_pUi->cbMaterialName->currentText() );
}

void GenomeMaterialDialog::on_pbSaveAs_clicked( void )
{
    if ( !m_pMaterial )
        return;
    updateActiveGroupBox();
    QString strFilter = QFileInfo( windowTitle() ).suffix() == "xshmat" ? tr( "Gothic 3 Material (*.xshmat);;" ) : tr( "Risen Material (*._xmat);;" );
    QString strFilePath = QFileDialog::getSaveFileName( this, tr( "Save As" ), m_SceneInfo.accessMaterialDir() + QDir::separator() + windowTitle(), strFilter ) ;
    if ( strFilePath == "" )
        return;
    mCFileStream streamOut;
    if ( streamOut.Open( strFilePath.toAscii().data(), mEFileOpenMode_Write ) == mEResult_False )
        Rimy3D::showError( tr( "Cannot create/open the file \"%1\"." ).arg( strFilePath ), tr( "Genome Material Editor" ) );
    else
    {
        m_SceneInfo.accessMaterialDir() = QFileInfo( strFilePath ).absolutePath();
        m_pMaterial->Save( streamOut );
    }
}

void GenomeMaterialDialog::on_pbSelectMaterial_clicked( void )
{
    QString strMaterialFilePath = QFileDialog::getOpenFileName( this, tr( "Open" ), m_SceneInfo.accessMaterialDir(), tr( "Genome Materials (*._xmat *.xshmat);;Gothic 3 Material (*.xshmat);;Risen Material (*._xmat);;" ) );
    if ( !strMaterialFilePath.length() )
        return;
    m_pUi->cbMaterialName->lineEdit()->setText( strMaterialFilePath );
    m_SceneInfo.accessMaterialDir() = QFileInfo( strMaterialFilePath ).absolutePath();
}
