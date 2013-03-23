#include "sceneinfo.h"
#include "texturefinder.h"
#include "exportsettingsdialog.h"
#include "rimy3d.h"
#include <QFileInfo>
#include <QVector>
#include <GLC_World>
#include <GLC_Mesh>

SceneInfo::SceneInfo( void )
{
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

SceneInfo::~SceneInfo( void )
{
}

GLC_World SceneInfo::buildGlcWorld( void )
{
    GLC_World world;

    unsigned int uMaterialCount = m_sceneCurrentScene.GetNumMaterials();
    QVector< QVector< GLC_Material * > > arrMaterialArrays( uMaterialCount );
    mCMultiMaterial matMultiDummy;
    mCMaterial & matSubDummy = matMultiDummy.AccessSubMaterials().AddNew();
    for ( int i = 0; i != uMaterialCount; ++i )
    {
        mCMaterialBase const * pCurrentMaterial = m_sceneCurrentScene.GetMaterialAt( i );
        mCMultiMaterial const * pMultiMat = dynamic_cast< mCMultiMaterial const * >( pCurrentMaterial );
        if ( !pMultiMat )
        {
            pMultiMat = &matMultiDummy;
            matSubDummy = *dynamic_cast< mCMaterial const * >( pCurrentMaterial );
        }
        for ( int j = 0, je = pMultiMat->GetSubMaterials().GetCount(); j != je; ++j )
        {
            mCMaterial const & mtlSub = pMultiMat->GetSubMaterials()[ j ];
            mCTexMap const * pDiffuseMap = mtlSub.GetTextureMapAt( mCMaterial::EMapType_Diffuse );
            QImage Texture;
            if ( pDiffuseMap )
                TextureFinder::getInstance().findTexture( pDiffuseMap->GetTextureFilePath().GetText(), getCurrentDir(), Texture );
            GLC_Texture * pTexture = Texture.isNull() ? new GLC_Texture : new GLC_Texture( Texture );
            pTexture->setMaxTextureSize( QSize( 2048, 2048 ) );
            arrMaterialArrays[ i ].append( new GLC_Material( pTexture, mtlSub.GetName().GetText() ) );
        }
        if ( !arrMaterialArrays[ i ].count() )
            arrMaterialArrays[ i ].append( 0 );
    }

    for ( int i = 0, ie = m_sceneCurrentScene.GetNumNodes(); i != ie; ++i )
    {
        if ( !m_sceneCurrentScene.GetNodeAt( i )->HasMesh() )
            continue;
        mCMesh meshCurrent = *m_sceneCurrentScene.GetNodeAt( i )->GetMesh();
        unsigned int uMaterialIndex = m_sceneCurrentScene.GetMaterialIndexByName( m_sceneCurrentScene.GetNodeAt( i )->GetMaterialName() );
        QVector< GLC_Material * > * pSubMaterials = uMaterialIndex != MI_DW_INVALID ? &arrMaterialArrays[ uMaterialIndex ] : 0;
        mTArray< mCMesh::SUniVert > arrUniVerts;
        mTArray< mCFace > arrUVFaces;
        if ( !meshCurrent.HasTVFaces() )
            meshCurrent.CalcFakeTexturing();
        if ( !meshCurrent.HasVNFaces() )
            meshCurrent.CalcVNormalsBySGs();
        meshCurrent.SortFacesByMatID();
        meshCurrent.CalcUniVertMesh( arrUniVerts, arrUVFaces );
        int iUniVertCount = arrUniVerts.GetCount();
        QVector< GLfloat > arrVertexFloats( iUniVertCount * 3 );
        QVector< GLfloat > arrNormalFloats( iUniVertCount * 3 );
        QVector< GLfloat > arrTexelFloats( iUniVertCount * 2 + 1 );
        QList< GLuint > listIndices;
        for ( int i = 0; i != iUniVertCount; ++i )
        {
            mCMesh::SUniVert & UniVert = arrUniVerts[ i ];
            for ( int j = 0; j != 3; ++j )
            {
                arrVertexFloats[ 3 * i + j ] = ( *UniVert.m_pVert )[ j ];
                arrNormalFloats[ 3 * i + j ] = ( *UniVert.m_pVNormal )[ j ];
                arrTexelFloats[ 2 * i + j ] = ( *UniVert.m_pTVert )[ j ];
            }
        }
        arrTexelFloats.pop_back();
        GLC_Mesh * pMesh = new GLC_Mesh;
        pMesh->addVertice( arrVertexFloats );
        pMesh->addTexels( arrTexelFloats );
        pMesh->addNormals( arrNormalFloats );
        for ( int i = 0, j = 0, ie = arrUVFaces.GetCount(), iCurrentMatId = 0; i != ie; ++i )
        {
            int iMatId = arrUniVerts[ arrUVFaces[ i ][ 0 ] ].m_uMatID;
            if ( ( i + 1 ) == ie )
                ++i, ++ie, iMatId = iCurrentMatId + 1;
            if ( i && ( iMatId != iCurrentMatId ) )
            {
                for ( ; j != i; ++j )
                    for ( int k = 0; k != 3; ++k )
                        listIndices.append( arrUVFaces[ j ][ k ] );
                pMesh->addTriangles( pSubMaterials ? pSubMaterials->at( iCurrentMatId % pSubMaterials->count() ) : 0, listIndices );
                listIndices.clear();
            }
            iCurrentMatId = iMatId;
        }
        pMesh->finish();
        world.rootOccurence()->addChild( new GLC_StructInstance( new GLC_3DRep( pMesh ) ) );
    }

    for ( int i = arrMaterialArrays.count(); i--; )
        for ( int j = arrMaterialArrays[ i ].count(); j--; )
            if ( arrMaterialArrays[ i ][ j ]->isUnused() )
                delete arrMaterialArrays[ i ][ j ];
    return world;
}

void SceneInfo::clearScene( void )
{
    m_strCurrentFile = "";
    m_sceneCurrentScene.Clear();
    emit sceneChanged();
}

QString const & SceneInfo::getCurrentDir( void )
{
    return m_strCurrentDir;
}

QString const & SceneInfo::getCurrentFile( void )
{
    return m_strCurrentFile;
}

QString const & SceneInfo::getCurrentSaveDir( void )
{
    if ( m_strCurrentSaveDir == "" )
        m_strCurrentSaveDir = getCurrentDir();
    return m_strCurrentSaveDir;
}

mCScene const & SceneInfo::getCurrentScene( void )
{
    return m_sceneCurrentScene;
}

bool SceneInfo::openSceneFile( QString a_strFilePath )
{
    PreferencesDialog const & Prefs = PreferencesDialog::getInstance();
    QFileInfo FileInfo( a_strFilePath );
    QString strExt = FileInfo.suffix().toLower();
    if ( !FileInfo.exists() )
    {
        Rimy3D::showError( tr( "The file \"%1\" doesn't exist." ).arg( a_strFilePath ), Rimy3D::applicationName() );
        return false;
    }
    QString strTitle = tr( "%1 Import" ).arg( QString( strExt ).toUpper() );
    mCFileStream streamIn;
    if ( streamIn.Open( FileInfo.absoluteFilePath().toAscii().data(), mEFileOpenMode_Read ) == mEResult_False )
        return false;
    mCError const * pLastError = mCError::GetLastError< mCError >();
    mEResult enuResult = mEResult_False;
    mCScene sceneNew;
    if ( strExt == "obj" )
    {
        enuResult = mCObjReader::ReadObjFileData( sceneNew, streamIn, FileInfo.absolutePath().toAscii().data() );
    }
    else if ( strExt == "3db" )
    {
        enuResult = mC3dbReader::Read3dbFileData( sceneNew, streamIn );
    }
    else if ( strExt == "gmax" )
    {
        mCMaxFileStream streamInMax( streamIn.GetFileName() );
        enuResult = mCMaxReader::ReadInMaxFileData( sceneNew, streamInMax );
        streamInMax.Close();
    }
    else if ( ( strExt == "ase" ) || ( strExt == "asc" ) )
    {
        enuResult = mCAseReader::ReadAseFileData( sceneNew, streamIn );
        if ( ( strExt == "asc" ) && Prefs.removeAscPrefixes() )
        {
            for ( MIUInt u = sceneNew.GetNumNodes(); u--; )
            {
                mCString & strName = sceneNew.AccessNodeAt( u )->AccessName();
                if ( strName.CompareNoCase( "Bip01" ) == 0 )
                    sceneNew.RemoveNode( sceneNew.AccessNodeAt( u ) );
                else if ( strName.Left( 3 ).ToLower() == "zm_" )
                    strName.TrimLeft( ( MIUInt ) 3 );
                else if ( strName.Left( 5 ).ToLower() == "bip01" )
                    strName.TrimLeft( ( MIUInt ) ( strName[ 5 ] == ' ' ? 6 : 5 ) );
            }
        }
    }
    else if ( strExt == "xact" )
    {
        mCXactReader::SOptions Options;
        Options.m_strTextureFileExtension = Prefs.defaultImageFileExt().toAscii().data();
        enuResult = mCXactReader::ReadXactFileData( sceneNew, streamIn, Options );
    }
    else if ( strExt == "xcmsh" )
    {
        mCXcmshReader::SOptions Options;
        enuResult = mCXcmshReader::ReadXcmshFileData( sceneNew, streamIn, Options );
    }
    else if ( strExt == "_xmac" )
    {
        mCXmacReader::SOptions Options;
        Options.m_strTextureFileExtension = Prefs.defaultImageFileExt().toAscii().data();
        enuResult = mCXmacReader::ReadXmacFileData( sceneNew, streamIn, Options );
        if ( Prefs.removeXmacCollisionMesh() )
            for ( MIUInt u = sceneNew.GetNumNodes(); u--; )
                if ( 0 == sceneNew.GetNodeAt( u )->GetName().CompareNoCase( "CollisionMesh" ) )
                    sceneNew.RemoveNode( sceneNew.AccessNodeAt( u ) );
    }
    else
    {
        Rimy3D::showError( tr( "Unknown file type: \".%1\"" ).arg( strExt ), Rimy3D::applicationName() );
    }
    streamIn.Close();
    if ( enuResult == mEResult_Ok )
    {
        m_strCurrentDir = FileInfo.absolutePath();
        m_strCurrentFile = FileInfo.fileName();
        m_sceneCurrentScene.Swap( sceneNew );
        emit sceneChanged();
        return true;
    }
    showLastMimicryError( pLastError, strTitle );
    return false;
}

bool SceneInfo::saveSceneFile( QString a_strFilePath, exportSettingsDialog const & a_SettingsDialog )
{
    m_strCurrentSaveDir = QFileInfo( a_strFilePath ).absolutePath();
    QString strExt = QFileInfo( a_strFilePath ).suffix().toLower();
    mEResult enuResult = mEResult_False;
    mCMemoryStream streamBase;
    if ( strExt == "xact" )
        enuResult = streamBase.FromFile( a_SettingsDialog.baseXact().toAscii().data() );
    else if ( strExt == "_xmac" )
        enuResult = streamBase.FromFile( a_SettingsDialog.baseXmac().toAscii().data() );
    mCFileStream streamOut;
    if ( streamOut.Open( a_strFilePath.toAscii().data(), mEFileOpenMode_Write ) == mEResult_False )
    {
        Rimy3D::showError( tr( "Cannot create/open the file \"%1\"." ).arg( a_strFilePath ), a_SettingsDialog.windowTitle() );
        return false;
    }
    eSConverterOptions BaseOptions;
    BaseOptions.m_bDropVertexColors = !a_SettingsDialog.colors();
    BaseOptions.m_bDropVertexNormals = !a_SettingsDialog.normals();
    BaseOptions.m_bRecalculateVertexNormals = a_SettingsDialog.recalcNormals();
    BaseOptions.m_bUseAnglesInsteadOfSGs = a_SettingsDialog.anglesNotSgs();
    BaseOptions.m_fMaxAngleInDegrees = static_cast< MIFloat >( a_SettingsDialog.angle() );
    mCError const * pLastError = mCError::GetLastError< mCError >();
    if ( strExt == "3db" )
    {
        mC3dbWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        enuResult = mC3dbWriter::Write3dbFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "asc" )
    {
        mCAseWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_bGothicAscFormat = true;
        enuResult = mCAseWriter::WriteAseFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "ase" )
    {
        mCAseWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        enuResult = mCAseWriter::WriteAseFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "obj" )
    {
        mCObjWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_bWriteMtlFile = a_SettingsDialog.createMtl();
        Options.m_strMtlFilePath = QString( a_strFilePath ).replace( ".obj", ".mtl", Qt::CaseInsensitive ).toAscii().data();
        enuResult = mCObjWriter::WriteObjFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "xact" )
    {
        if ( enuResult == mEResult_False )
            Rimy3D::showError( tr( "Cannot open the base .xact file \"%1\"." ).arg( a_SettingsDialog.baseXact() ), a_SettingsDialog.windowTitle() );
        else
        {
            mCXactWriter::SOptions Options;
            static_cast< eSConverterOptions & >( Options ) = BaseOptions;
            Options.m_pBaseXactStream = &streamBase;
            Options.m_bIndirectVertexMatching = a_SettingsDialog.indirectMatching();
            Options.m_bReplaceOnlyVertices = a_SettingsDialog.vertsOnly();
            enuResult = mCXactWriter::WriteXactFileData( m_sceneCurrentScene, streamOut, Options );
        }
    }
    else if ( strExt == "_xmac" )
    {
        if ( enuResult == mEResult_False )
            Rimy3D::showError( tr( "Cannot open the base ._xmac file \"%1\"." ).arg( a_SettingsDialog.baseXmac() ), a_SettingsDialog.windowTitle() );
        else
        {
            mCXmacWriter::SOptions Options;
            static_cast< eSConverterOptions & >( Options ) = BaseOptions;
            Options.m_pBaseXmacStream = &streamBase;
            Options.m_bIndirectVertexMatching = a_SettingsDialog.indirectMatching();
            Options.m_bReplaceOnlyVertices = a_SettingsDialog.vertsOnly();
            enuResult = mCXmacWriter::WriteXmacFileData( m_sceneCurrentScene, streamOut, Options );
        }
    }
    else
    {
        Rimy3D::showError( tr( "Unknown file type: \".%1\"" ).arg( strExt ), Rimy3D::applicationName() );
        enuResult = mEResult_False;
    }
    streamOut.Close();
    if ( enuResult == mEResult_Ok )
        return true;
    QFile::remove( a_strFilePath );
    showLastMimicryError( pLastError, a_SettingsDialog.windowTitle() );
    return false;
}

bool SceneInfo::sceneContainsUnskinnedMeshes( void )
{
    for ( MIUInt u = m_sceneCurrentScene.GetNumNodes(); u--; )
        if ( m_sceneCurrentScene.GetNodeAt( u )->HasMesh() && !m_sceneCurrentScene.GetNodeAt( u )->HasSkin() )
            return true;
    return false;
}

void SceneInfo::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    m_strCurrentDir = a_Settings.value( "dir", "" ).toString();
    m_strCurrentSaveDir = a_Settings.value( "savedir", "" ).toString();
    a_Settings.endGroup();
}

void SceneInfo::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    a_Settings.setValue( "dir", m_strCurrentDir );
    a_Settings.setValue( "savedir", m_strCurrentSaveDir );
    a_Settings.endGroup();
}

void SceneInfo::errorMessageTranslations( void )
{
    tr( "Unknown Extended Saving version." );
    tr( "Unknown .ase file version." );
    tr( "Invalid .ase file." );
    tr( "Invalid .3db file." );
    tr( "Invalid .xact file." );
    tr( "Invalid .xcmsh file." );
    tr( "Invalid ._xmac file." );
    tr( "Unknown ._xmac file version." );
    tr( "Invalid base .xact file." );
    tr( "No common mesh in base .xact file found." );
    tr( "No common mesh in base ._xmac file found." );
    tr( "New mesh and base mesh vertex counts differ." );
    tr( "New mesh has no material." );
    tr( "Skinning includes bone not present in base .xact file." );
    tr( "Skinning includes bone not present in base ._xmac file." );
    tr( "Skinning does not cover all vertices." );
    tr( "Unknown vertex stream array type." );
}

void SceneInfo::showLastMimicryError( mCError const * a_pLastError, QString a_strTitle )
{
    mCString strError;
    for ( mCError const * pError = 0; ( pError = mCError::GetLastError< mCError >() ) != a_pLastError; mCError::ClearError( pError ) )
        strError = pError->GetText();
    if ( strError == "Invalid .gmax file. The file might have been saved without Extended Saving enabled." )
        strError = tr( "Invalid .gmax file. The file might have been saved without Extended Saving enabled. You can install Extended Saving via the tools menu." ).toAscii().data();
    if ( strError != "" )
        Rimy3D::showError( tr( strError.GetText() ), a_strTitle );
}
