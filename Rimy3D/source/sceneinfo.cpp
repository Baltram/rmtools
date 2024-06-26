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

QString & SceneInfo::accessMaterialDir( void )
{
    return m_strMaterialDir;
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
            mCTexMap const * pDiffuseMap = mtlSub.GetTexMap( mCMaterial::EMapType_Diffuse );
            QImage Texture;
            if ( pDiffuseMap )
                TextureFinder::getInstance().findTexture( pDiffuseMap->GetTextureFilePath().GetText(), getCurrentDir(), Texture, PreferencesDialog::getInstance().lookUpTextures() );
            GLC_Texture * pTexture = Texture.isNull() ? new GLC_Texture : new GLC_Texture( Texture );
            pTexture->setMaxTextureSize( QSize( 2048, 2048 ) );
            arrMaterialArrays[ i ].append( new GLC_Material( pTexture, mtlSub.GetName().GetText() ) );
        }
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
            int iMatId = arrUniVerts[ arrUVFaces[ i ][ 0 ] ].m_uMatID & 0xFFFFFF;
            if ( ( i + 1 ) == ie )
                ++i, ++ie, iMatId = iCurrentMatId + 1;
            if ( i && ( iMatId != iCurrentMatId ) )
            {
                for ( ; j != i; ++j )
                    for ( int k = 0; k != 3; ++k )
                        listIndices.append( arrUVFaces[ j ][ k ] );
                pMesh->addTriangles( ( pSubMaterials && pSubMaterials->count() ) ? pSubMaterials->at( iCurrentMatId % pSubMaterials->count() ) : 0, listIndices );
                listIndices.clear();
            }
            iCurrentMatId = iMatId;
        }
        pMesh->finish();
        if ( !pMesh->isEmpty() )
            world.rootOccurence()->addChild( new GLC_StructInstance( new GLC_3DRep( pMesh ) ) );
        else
            delete pMesh;
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

bool SceneInfo::openSceneFile( QString a_strFilePath, bool a_bMerge )
{
    static bool s_bRecursive = false;
    PreferencesDialog const & Prefs = PreferencesDialog::getInstance();
    QFileInfo FileInfo( a_strFilePath );
    QString strExt = FileInfo.suffix().toLower();
    if ( !FileInfo.exists() )
        return Rimy3D::showError( tr( "Could not open \"%1\"." ).arg( a_strFilePath ), Rimy3D::applicationName() ), false;
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
    else if ( strExt == "3ds" )
    {
        enuResult = mC3dsReader::Read3dsFileData( sceneNew, streamIn );
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
        Options.m_strTextureFileExtension = Prefs.defaultImageFileExt().toAscii().data();
        enuResult = mCXcmshReader::ReadXcmshFileData( sceneNew, streamIn, Options );
    }
    else if ( strExt == "_xmsh" )
    {
        mCXmshReader::SOptions Options;
        Options.m_strTextureFileExtension = Prefs.defaultImageFileExt().toAscii().data();
        enuResult = mCXmshReader::ReadXmshFileData( sceneNew, streamIn, Options );
    }
    else if ( strExt == "xlmsh" )
    {
        mTArray< mCString > arrMeshNames;
        enuResult = mCXlmshReader::ReadXlmshFileData( arrMeshNames, streamIn );
        MIBool bRecursive = s_bRecursive;
        s_bRecursive = true;
        m_sceneCurrentScene.Swap( sceneNew );
        for ( unsigned int u = arrMeshNames.GetCount(); enuResult == mEResult_Ok && u--; )
            if ( openSceneFile( FileInfo.absolutePath() + QDir::separator() + arrMeshNames[ u ].GetText(), true ) == false )
                enuResult = mEResult_False;
        m_sceneCurrentScene.Swap( sceneNew );
        s_bRecursive = bRecursive;
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
    else if ( strExt == "_xcom" )
    {
        enuResult = mCXcomReader::ReadXcomFileData( sceneNew, streamIn );
    }
    else if ( strExt == "xnvmsh" )
    {
        enuResult = mCXnvmshReader::ReadXnvmshFileData( sceneNew, streamIn );
    }
    else if ( strExt == "r3msh" )
    {
        enuResult = mCR3mshReader::ReadR3mshFileData( sceneNew, streamIn );
    }
    else
    {
        Rimy3D::showError( tr( "Unsupported file type: \".%1\"" ).arg( strExt ), Rimy3D::applicationName() );
    }
    streamIn.Close();
    if ( enuResult == mEResult_Ok )
    {
        m_strCurrentDir = FileInfo.absolutePath();
        m_strCurrentFile = FileInfo.fileName();
        if ( a_bMerge )
            m_sceneCurrentScene.Merge( sceneNew );
        else
            m_sceneCurrentScene.Swap( sceneNew );
        if ( !s_bRecursive )
            emit sceneChanged();
        return true;
    }
    showLastMimicryError( pLastError, strTitle );
    return false;
}

namespace
{
    QString s_strTexExt;
    QQueue< mCMaterial > s_queueMaterialsOld;

    void patchTextureFileNames( mCMaterial & a_matDest )
    {
        s_queueMaterialsOld.enqueue( a_matDest );
        for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse, ie = mCMaterial::EMapType_Count; i != ie; ++i )
        {
            mCTexMap const * pMap = a_matDest.GetTexMap( i );
            if ( !pMap )
                continue;
            mCTexMap mapPatched = *pMap;
            mapPatched.AccessTextureFilePath() = ( QFileInfo( mapPatched.GetTextureFilePath().GetText() ).baseName() + "." + s_strTexExt ).toAscii().data();
            a_matDest.AccessTexMap( i ) = mapPatched;
        }
    }

    void unPatchTextureFileNames( mCMaterial & a_matDest )
    {
        a_matDest = s_queueMaterialsOld.dequeue();
    }

    void iterateOverSceneMaterials( mCScene & a_sceneScene, void ( * a_pfuncCallback )( mCMaterial & ) )
    {
        for ( unsigned int u = 0, ue = a_sceneScene.GetNumMaterials(); u != ue; ++u )
        {
            mCMaterial * pMaterial = dynamic_cast< mCMaterial * >( a_sceneScene.AccessMaterialAt( u ) );
            mCMultiMaterial * pMultiMaterial = dynamic_cast< mCMultiMaterial * >( a_sceneScene.AccessMaterialAt( u ) );
            if ( pMaterial )
                ( *a_pfuncCallback )( *pMaterial );
            else if ( pMultiMaterial )
                for ( unsigned int v = 0, ve = pMultiMaterial->GetSubMaterials().GetCount(); v != ve; ++v )
                    ( *a_pfuncCallback )( pMultiMaterial->AccessSubMaterials()[ v ] );
        }
    }
}

bool SceneInfo::saveSceneFile( QString a_strFilePath, exportSettingsDialog const & a_SettingsDialog, GLC_World const * a_pWorldHint )
{
    m_strCurrentSaveDir = QFileInfo( a_strFilePath ).absolutePath();
    QString strExt = QFileInfo( a_strFilePath ).suffix().toLower();
    s_strTexExt = a_SettingsDialog.textureImageFileExtension();
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
    if ( s_strTexExt != "" )
        iterateOverSceneMaterials( m_sceneCurrentScene, &patchTextureFileNames );
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
    else if ( strExt == "3ds" )
    {
        mC3dsWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_bExportSmoothingGroups = a_SettingsDialog.exportSGs();
        Options.m_bGothic3ds = a_SettingsDialog.gothic3ds();
        enuResult = mC3dsWriter::Write3dsFileData( m_sceneCurrentScene, streamOut, Options );
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
    else if ( strExt == "xcmsh" )
    {
        mCXcmshWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        enuResult = mCXcmshWriter::WriteXcmshFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "_xmsh" )
    {
        mCXmshWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        enuResult = mCXmshWriter::WriteXmshFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "xlmsh" )
    {
        enuResult = mCXlmshWriter::WriteXlmshFileData( m_sceneCurrentScene, streamOut );
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
    else if ( strExt == "xnvmsh" )
    {
        mCXnvmshWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_bConvex = a_SettingsDialog.convex();
        enuResult = mCXnvmshWriter::WriteXnvmshFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else if ( strExt == "_xcom" )
    {
        mCXcomWriter::SOptions Options;
        static_cast< eSConverterOptions & >( Options ) = BaseOptions;
        Options.m_bConvex = a_SettingsDialog.convex();
        enuResult = mCXcomWriter::WriteXcomFileData( m_sceneCurrentScene, streamOut, Options );
    }
    else
    {
        Rimy3D::showError( tr( "Unknown file type: \".%1\"" ).arg( strExt ), Rimy3D::applicationName() );
        enuResult = mEResult_False;
    }
    streamOut.Close();
    if ( s_strTexExt != "" )
        iterateOverSceneMaterials( m_sceneCurrentScene, &unPatchTextureFileNames );
    if ( enuResult == mEResult_Ok )
    {
        if ( !a_pWorldHint || !a_pWorldHint->rootOccurence() || s_strTexExt.length() == 0 )
            return true;
        QSet< GLC_Material * > setMaterials = a_pWorldHint->rootOccurence()->materialSet();
        for ( QSet< GLC_Material * >::const_iterator i = setMaterials.begin(), ie = setMaterials.end(); i != ie; ++i )
        {
            QImage imgTexture = ( *i )->textureHandle() ? ( *i )->textureHandle()->imageOfTexture() : QImage();
            if ( !imgTexture.isNull() && !imgTexture.text( "OriginalFileName" ).isNull() )
                imgTexture.save( QFileInfo( a_strFilePath ).absolutePath() + QDir::separator() + QFileInfo( imgTexture.text( "OriginalFileName" ) ).baseName() + "." + s_strTexExt );
        }
        return true;
    }
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
    m_strMaterialDir = a_Settings.value( "materialdir", QDir::homePath() ).toString();
    a_Settings.endGroup();
}

void SceneInfo::lookUpGenomeMaterials( void )
{
    if ( m_sceneCurrentScene.GetNumMaterials() == 0 )
        return;
    mCGenomeMaterial::LoadGothic3Materials( m_sceneCurrentScene, PreferencesDialog::getInstance().defaultImageFileExt().toAscii().data() );
    mCGenomeMaterial::LoadRisenMaterials( m_sceneCurrentScene, PreferencesDialog::getInstance().defaultImageFileExt().toAscii().data() );
    emit sceneChanged();
}

void SceneInfo::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    a_Settings.setValue( "dir", m_strCurrentDir );
    a_Settings.setValue( "savedir", m_strCurrentSaveDir );
    a_Settings.setValue( "materialdir", m_strMaterialDir );
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
    tr( "Invalid .xlmsh file." );
    tr( "Invalid .xnvmsh file." );
    tr( "Incompatible PhysX version - .xnvmsh file version too old.\n\n(Gothic 3 can't read such files either. If needed, it recreates them using the original .xcmsh data.)" );
    tr( "PhysX error. It might help to download and install 'Nvidia PhysX System Software' from the official Nvidia homepage and restart the program." );
    tr( "PhysX error. It might help to download and install 'Nvidia PhysX Legacy System Software' (NOT 'Nvidia PhysX System Software'!) from the official Nvidia homepage and restart the program." );
    tr( "Please download and install 'Nvidia PhysX System Software' from the official Nvidia homepage and restart the program." );
    tr( "Please download and install 'Nvidia PhysX Legacy System Software' (NOT 'Nvidia PhysX System Software'!) from the official Nvidia homepage and restart the program." );
    tr( "Invalid ._xmsh file." );
    tr( "Invalid ._xmac file." );
    tr( "Unknown ._xmac file version." );
    tr( "Invalid ._xcom file." );
    tr( "Invalid base .xact file." );
    tr( "No common mesh in base .xact file found." );
    tr( "No common mesh in base ._xmac file found." );
    tr( "New mesh and base mesh vertex counts differ." );
    tr( "New mesh has no material." );
    tr( "Skinning includes bone not present in base .xact file." );
    tr( "Skinning includes bone not present in base ._xmac file." );
    tr( "Skinning does not cover all vertices." );
    tr( "Unknown vertex stream array type." );
    tr( "The scene contains no mesh." );
    tr( "Invalid .3ds file." );
    tr( "Maximum number of materials exceeded. The Gothic 3DS file format only supports up to 1024 materials." );
    tr( "Maximum number of materials exceeded. The 3DS file format only supports up to 256 materials." );
    tr( "Maximum vertex number exceeded. The 3DS file format only supports meshes with up to 65535 vertices. (A vertex with n different UV coordinate sets or materials counts as n vertices.)" );
    tr( "Maximum triangle number exceeded. The 3DS file format only supports meshes with up to 65535 triangles." );
}

void SceneInfo::showLastMimicryError( mCError const * a_pLastError, QString a_strTitle )
{
    mCString strError;
    for ( mCError const * pError = 0; ( pError = mCError::GetLastError< mCError >() ) != a_pLastError; mCError::ClearError( pError ) )
        if ( strError == "" )
            strError = pError->GetText();
    if ( strError == "Invalid .gmax file. The file might have been saved without Extended Saving enabled." )
        strError = tr( "Invalid .gmax file. The file might have been saved without Extended Saving enabled. You can install Extended Saving via the tools menu." ).toAscii().data();
    if ( strError != "" )
        Rimy3D::showError( tr( strError.GetText() ), a_strTitle );
}
