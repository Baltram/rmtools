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
            QImage Texture( pDiffuseMap ? TextureFinder::getInstance().findTextureFile( pDiffuseMap->GetTextureFilePath().GetText(), getCurrentDir() ) : "" );
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
    QFileInfo FileInfo( a_strFilePath );
    QString strExt = FileInfo.suffix().toLower();
    QString strTitle = tr( "%1 Import" ).arg( QString( strExt ).toUpper() );
    if ( !FileInfo.exists() )
    {
        Rimy3D::showError( tr( "The file %1 doesn't exist." ).arg( a_strFilePath ), strTitle );
        return false;
    }
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
    }
    else if ( strExt == "xact" )
    {
        enuResult = mCXactReader::ReadXactFileData( sceneNew, streamIn );
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

bool SceneInfo::saveSceneFile( QString a_strFilePath, exportSettingsDialog & a_SettingsDialog )
{
    m_strCurrentSaveDir = QFileInfo( a_strFilePath ).canonicalPath();
    QString strExt = QFileInfo( a_strFilePath ).suffix().toLower();
    mCFileStream streamOut;
    if ( streamOut.Open( a_strFilePath.toAscii().data(), mEFileOpenMode_Write ) == mEResult_False )
    {
        Rimy3D::showError( tr( "Cannot create/open the file %1." ).arg( a_strFilePath ), a_SettingsDialog.windowTitle() );
        return false;
    }
    eSConverterOptions BaseOptions;
    BaseOptions.m_bDropVertexColors = !a_SettingsDialog.colors();
    BaseOptions.m_bDropVertexNormals = !a_SettingsDialog.normals();
    BaseOptions.m_bRecalculateVertexNormals = a_SettingsDialog.recalcNormals();
    BaseOptions.m_bUseAnglesInsteadOfSGs = a_SettingsDialog.anglesNotSgs();
    BaseOptions.m_fMaxAngleInDegrees = static_cast< MIFloat >( a_SettingsDialog.angle() );
    mCError const * pLastError = mCError::GetLastError< mCError >();
    mEResult enuResult = mEResult_False;
    if ( strExt == "3db" )
    {
        mC3dbWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        /*//$*/mCSkin const * pSkin = 0;
        for ( MIUInt u = m_sceneCurrentScene.GetNumNodes(); u--; )
            if ( m_sceneCurrentScene.GetNodeAt( u )->HasSkin() )
                pSkin = m_sceneCurrentScene.GetNodeAt( u )->GetSkin();
        for ( MIUInt u = pSkin->GetNumBones(); u--; )
            if ( m_sceneCurrentScene.GetNodeIndexByID( pSkin->GetBoneIDByIndex( u ) ) == MI_DW_INVALID )
                u = u;/*//$*/
        pSkin = pSkin;
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
        mCFileStream streamBaseXact;
        if ( streamBaseXact.Open( a_SettingsDialog.baseXact().toAscii().data(), mEFileOpenMode_Read ) == mEResult_False )
        {
            Rimy3D::showError( tr( "Cannot open the file %1." ).arg( a_SettingsDialog.baseXact() ), a_SettingsDialog.windowTitle() );
            return false;
        }
        mCXactWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_pBaseXactStream = &streamBaseXact;
        Options.m_bReplaceOnlyVertices = a_SettingsDialog.vertsOnly();
        enuResult = mCXactWriter::WriteXactFileData( m_sceneCurrentScene, streamOut, Options );
    }
    streamOut.Close();
    if ( enuResult == mEResult_Ok )
        return true;
    showLastMimicryError( pLastError, a_SettingsDialog.windowTitle() );
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
    tr( "Invalid .gmax file. The file might have been saved without Extended Saving enabled." );
    tr( "Unknown Extended Saving version." );
    tr( "Unknown .ase file version." );
    tr( "Invalid .ase file." );
    tr( "Invalid .xact file." );
    tr( "Invalid .3db file." );
    tr( "Invalid source .xact file." );
    tr( "Cannot find same-named mesh in source .xact file." );
    tr( "New mesh has wrong vertex count." );
    tr( "New mesh has no material." );
    tr( "Skinning includes bone not present in .xact file." );
    tr( "Skinning does not cover all vertices." );
}

void SceneInfo::showLastMimicryError( mCError const * a_pLastError, QString a_strTitle )
{
    mCString strError;
    for ( mCError const * pError = 0; ( pError = mCError::GetLastError< mCError >() ) != a_pLastError; mCError::ClearError( pError ) )
        strError = pError->GetText();
    if ( strError != "" )
        Rimy3D::showError( tr( strError.GetText() ), a_strTitle );
}
