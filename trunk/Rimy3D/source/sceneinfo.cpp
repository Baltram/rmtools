#include <QFileInfo>
#include <QVector>
#include <GLC_Mesh>
#include "sceneinfo.h"
#include "texturefinder.h"
#include "rimy3d.h"

SceneInfo::SceneInfo( void )
{
    connect( Rimy3D::getInstance(), SIGNAL( onSaveSettings( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( onLoadSettings( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
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
            QImage Texture( pDiffuseMap ? TextureFinder::getInstance().findTextureFile( pDiffuseMap->GetTextureFilePath().GetText() ) : "" );
            arrMaterialArrays[ i ].append( new GLC_Material( Texture.isNull() ? new GLC_Texture : new GLC_Texture( Texture ), mtlSub.GetName().GetText() ) );
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

mCScene const & SceneInfo::getCurrentScene( void )
{
    return m_sceneCurrentScene;
}

bool SceneInfo::openSceneFile( QString a_strFileName )
{
    QFileInfo FileInfo( a_strFileName );
    if ( !FileInfo.exists() )
        return false;
    mCFileStream streamIn;
    if ( streamIn.Open( FileInfo.absoluteFilePath().toAscii().data(), mEFileOpenMode_Read ) == mEResult_False )
        return false;
    mCError const * pLastError = mCError::GetLastError< mCError >();
    mEResult enuResult = mEResult_False;
    mCScene sceneNew;
    QString strExt = FileInfo.suffix().toLower();
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
    mCString strError;
    for ( mCError const * pError = 0; ( pError = mCError::GetLastError< mCError >() ) != pLastError; mCError::ClearError( pError ) )
        strError = pError->GetText();
    if ( strError != "" )
        Rimy3D::showError( tr( strError.GetText() ) );
    return false;
}

void SceneInfo::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    m_strCurrentDir = a_Settings.value( "dir", "" ).toString();
    a_Settings.endGroup();
}

void SceneInfo::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    a_Settings.setValue( "dir", m_strCurrentDir );
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
