#include "mi_include_converters.h"

mC3dsWriter::SOptions::SOptions( void ) :
    m_bExportSmoothingGroups( MIFalse ),
    m_bGothic3ds( MIFalse )
{
}

namespace
{
    enum EChunkID
    {
        EChunkID_M3DMAGIC      = 0x4D4D,
        EChunkID_M3D_VERSION   = 0x0002,
        EChunkID_MDATA         = 0x3D3D,
        EChunkID_MAT_ENTRY     = 0xAFFF,
        EChunkID_MAT_NAME      = 0xA000,
        EChunkID_MAT_TEXMAP    = 0xA200,
        EChunkID_MAT_BUMPMAP   = 0xA230,
        EChunkID_MAT_SPECMAP   = 0xA204,
        EChunkID_MAT_MAPNAME   = 0xA300,
        EChunkID_NAMED_OBJECT  = 0x4000,
        EChunkID_N_TRI_OBJECT  = 0x4100,
        EChunkID_POINT_ARRAY   = 0x4110,
        EChunkID_FACE_ARRAY    = 0x4120,
        EChunkID_MSH_MAT_GROUP = 0x4130,
        EChunkID_SMOOTH_GROUP  = 0x4150,
        EChunkID_TEX_VERTS     = 0x4140,
        EChunkID_MESH_MATRIX   = 0x4160
    };

    static mTArray< MIUInt > s_arrChunkOffsets;

    void StartChunk( mCIOStreamBinary & a_streamDest, EChunkID a_enuChunkID )
    {
        s_arrChunkOffsets.Add( a_streamDest.Tell() );
        a_streamDest << static_cast< MIU16 >( a_enuChunkID ) << ( MIU32 ) 0;
    }

    void EndChunk( mCIOStreamBinary & a_streamDest )
    {
        MIUInt uChunkOffset = s_arrChunkOffsets.Back(), uEndOffset = a_streamDest.Tell();
        a_streamDest.Seek( uChunkOffset + 2 );
        a_streamDest << g_32( uEndOffset - uChunkOffset );
        a_streamDest.Seek( uEndOffset );
        s_arrChunkOffsets.Resize( s_arrChunkOffsets.GetCount() - 1 );
    }

    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << a_strSource << ( MIU8 ) 0;
    }
}

mEResult mC3dsWriter::Write3dsFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    // Convert all standard materials to multi materials
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumMaterials(); u != ue; ++u )
    {
        mCMaterial * pMaterial = dynamic_cast< mCMaterial * >( a_sceneSource.AccessMaterialAt( u ) );
        mCMultiMaterial matMulti;
        if ( !pMaterial )
            continue;
        matMulti.AccessName() = pMaterial->GetName();
        matMulti.AccessSubMaterials().Add( *pMaterial );
        a_sceneSource.RemoveMaterial( pMaterial );
        a_sceneSource.AddMaterial( matMulti );
    }
    // Limit material names to 16 or 260 characters and convert texture file names to 8.3 format
    MIUInt const uMaxLength = a_Options.m_bGothic3ds ? 260 : 16, uPart = ( uMaxLength - 4 ) / 2;
    mTStringMap< MIUInt > mapMaterialNames;
    MIUInt uMaterialCount = 0;
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumMaterials(); u != ue; ++u )
    {
        mTArray< mCMaterial > & arrSubMaterials = dynamic_cast< mCMultiMaterial * >( a_sceneSource.AccessMaterialAt( u ) )->AccessSubMaterials();
        for ( MIUInt v = 0, ve = arrSubMaterials.GetCount(); v != ve; ++v, ++uMaterialCount )
        {
            for ( MIUInt uMapType = mCMaterial::EMapType_Diffuse; uMapType != mCMaterial::EMapType_Count; ++uMapType )
            {
                if ( a_Options.m_bGothic3ds || !arrSubMaterials[ v ].HasTexMap( static_cast< mCMaterial::EMapType >( uMapType ) ) )
                    continue;
                mCString & strFileName = arrSubMaterials[ v ].AccessTexMap( static_cast< mCMaterial::EMapType >( uMapType ) ).AccessTextureFilePath();
                mCString strExt = g_GetFileExt( strFileName ).Replace( ".", "" ).Replace( " ", "" ).Left( 3 );
                strFileName = g_GetFileNameNoExt( strFileName ).Replace( ".", "" ).Replace( " ", "" );
                if ( strFileName.GetLength() > 8 )
                    strFileName = strFileName.Left( 6 ) + "~1";
                strFileName = strFileName + "." + strExt;
                strFileName.ToUpper();
                for ( MILPChar pcIt = strFileName.AccessText(), pcEnd = pcIt + strFileName.GetLength(); pcIt != pcEnd; ++pcIt )
                {
                    if ( static_cast< MIU8 >( *pcIt ) >= 128 ||
                         ( 'A' <= *pcIt && 'Z' >= *pcIt ) ||
                         ( '0' <= *pcIt && '9' >= *pcIt ) ||
                         mCString( ".!#$%&'()-@^_`{}~" ).Contains( *pcIt ) )
                        continue;
                    *pcIt = '_';
                }
            }
            mCString & strName = arrSubMaterials[ v ].AccessName();
            if ( strName.GetLength() <= uMaxLength )
                continue;
            strName.Format( "%s~%s", strName.Left( uPart ).GetText(), strName.Right( uPart ).GetText() );
            strName.Append( mCString().Format( "~%.2x", mapMaterialNames[ strName ]++ & 0xFF ) );
        }
    }
    if ( uMaterialCount > ( a_Options.m_bGothic3ds ? 1024U : 256U ) )
    {
        if ( a_Options.m_bGothic3ds )
            return MI_ERROR( mCConverterError, EBadScene, "Maximum number of materials exceeded. The Gothic 3DS file format only supports up to 1024 materials." ), mEResult_False;
        else
            return MI_ERROR( mCConverterError, EBadScene, "Maximum number of materials exceeded. The 3DS file format only supports up to 256 materials." ), mEResult_False;
    }
    StartChunk( a_streamDest, EChunkID_M3DMAGIC );
    StartChunk( a_streamDest, EChunkID_M3D_VERSION );
    a_streamDest << ( MIU32 ) 3;
    EndChunk( a_streamDest );
    StartChunk( a_streamDest, EChunkID_MDATA );
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumMaterials(); u != ue; ++u )
    {
        mTArray< mCMaterial > & arrSubMaterials = dynamic_cast< mCMultiMaterial * >( a_sceneSource.AccessMaterialAt( u ) )->AccessSubMaterials();
        for ( MIUInt v = 0, ve = arrSubMaterials.GetCount(); v != ve; ++v )
        {
            StartChunk( a_streamDest, EChunkID_MAT_ENTRY );
            StartChunk( a_streamDest, EChunkID_MAT_NAME );
            WriteString( a_streamDest, arrSubMaterials[ v ].GetName() );
            EndChunk( a_streamDest );
            for ( MIUInt uMapType = mCMaterial::EMapType_Diffuse; uMapType != mCMaterial::EMapType_Count; ++uMapType )
            {
                if ( !arrSubMaterials[ v ].HasTexMap( static_cast< mCMaterial::EMapType >( uMapType ) ) )
                    continue;
                StartChunk( a_streamDest, ( uMapType == mCMaterial::EMapType_Specular ? EChunkID_MAT_SPECMAP : ( uMapType == mCMaterial::EMapType_Normal ? EChunkID_MAT_BUMPMAP : EChunkID_MAT_TEXMAP ) ) );
                StartChunk( a_streamDest, EChunkID_MAT_MAPNAME );
                WriteString( a_streamDest, arrSubMaterials[ v ].GetTexMap( static_cast< mCMaterial::EMapType >( uMapType ) )->GetTextureFilePath() );
                EndChunk( a_streamDest );
                EndChunk( a_streamDest );
            }
            EndChunk( a_streamDest );
        }
    }
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumNodes(); u != ue; ++u )
    {
        mCNode * pNode = a_sceneSource.AccessNodeAt( u );
        if ( !pNode->HasMesh() )
            continue;
        StartChunk( a_streamDest, EChunkID_NAMED_OBJECT );
        WriteString( a_streamDest, pNode->GetName() );
        StartChunk( a_streamDest, EChunkID_N_TRI_OBJECT );
        mCMesh & meshSource = *pNode->AccessMesh();
        meshSource.SetNumVNormals( 0 );
        meshSource.SetNumVTangents( 0 );
        meshSource.SetHasVertexColors( MIFalse );
        mTArray< mCMesh::SUniVert > arrUniVerts;
        mTArray< mCFace > arrUVFaces;
        meshSource.CalcUniVertMesh( arrUniVerts, arrUVFaces, MITrue );
        if ( arrUniVerts.GetCount() > 0xFFFF )
            return MI_ERROR( mCConverterError, EBadScene, "Maximum vertex number exceeded. The 3DS file format only supports meshes with up to 65535 vertices. (A vertex with n different UV coordinate sets or materials counts as n vertices.)" ), mEResult_False;
        if ( arrUVFaces.GetCount() > 0xFFFF )
            return MI_ERROR( mCConverterError, EBadScene, "Maximum triangle number exceeded. The 3DS file format only supports meshes with up to 65535 triangles." ), mEResult_False;
        StartChunk( a_streamDest, EChunkID_POINT_ARRAY );
        a_streamDest << ( MIU16 )( arrUniVerts.GetCount() );
        for ( MIUInt v = 0, ve = arrUniVerts.GetCount(); v != ve; ++v )
            a_streamDest << *arrUniVerts[ v ].m_pVert;
        EndChunk( a_streamDest );
        StartChunk( a_streamDest, EChunkID_FACE_ARRAY );
        a_streamDest << ( MIU16 )( arrUVFaces.GetCount() );
        for ( MIUInt v = 0, ve = arrUVFaces.GetCount(); v != ve; ++v )
            a_streamDest << ( MIU16 )( arrUVFaces[ v ].GetA() ) << ( MIU16 )( arrUVFaces[ v ].GetB() ) << ( MIU16 )( arrUVFaces[ v ].GetC() ) << ( MIU16 ) 0;
        if ( pNode->GetMaterialName() != "" && MI_DW_INVALID != a_sceneSource.GetMaterialIndexByName( pNode->GetMaterialName() ) )
        {
            mTArray< mCMaterial > & arrSubMaterials = dynamic_cast< mCMultiMaterial * >( a_sceneSource.AccessMaterialAt( a_sceneSource.GetMaterialIndexByName( pNode->GetMaterialName() ) ) )->AccessSubMaterials();
            for ( MIUInt v = 0, ve = arrSubMaterials.GetCount(); v != ve; ++v )
            {
                mTArray< MIU16 > arrFaceIndices;
                arrFaceIndices.Reserve( arrUVFaces.GetCount() );
                for ( MIUInt w = 0, we = arrUVFaces.GetCount(); w != we; ++w )
                    if ( arrUniVerts[ arrUVFaces[ w ].GetA() ].m_uMatID % ve == v )
                        arrFaceIndices.Add( static_cast< MIU16 >( w ) );
                StartChunk( a_streamDest, EChunkID_MSH_MAT_GROUP );
                WriteString( a_streamDest, arrSubMaterials[ v ].GetName() );
                a_streamDest << ( MIU16 )( arrFaceIndices.GetCount() );
                for ( MIUInt w = 0, we = arrFaceIndices.GetCount(); w != we; ++w )
                    a_streamDest << arrFaceIndices[ w ];
                EndChunk( a_streamDest );
            }
        }
        if ( a_Options.m_bExportSmoothingGroups )
        {
            StartChunk( a_streamDest, EChunkID_SMOOTH_GROUP );
            mCMaxFace const * pFaces = meshSource.GetFaces();
            for ( MIUInt v = 0, ve = meshSource.GetNumFaces(); v != ve; ++v )
                a_streamDest << g_32( pFaces[ v ].GetSGs() );
            EndChunk( a_streamDest );
        }
        EndChunk( a_streamDest );
        if ( meshSource.HasTVFaces() )
        {
            StartChunk( a_streamDest, EChunkID_TEX_VERTS );
            a_streamDest << ( MIU16 )( arrUniVerts.GetCount() );
            for ( MIUInt v = 0, ve = arrUniVerts.GetCount(); v != ve; ++v )
                a_streamDest << arrUniVerts[ v ].m_pTVert->GetX() << arrUniVerts[ v ].m_pTVert->GetY();
            EndChunk( a_streamDest );
        }
        StartChunk( a_streamDest, EChunkID_MESH_MATRIX );
        mCMatrix4 const & matTransform = pNode->GetTransform();
        for ( MIUInt v = 0; v != 4; ++v )
            for ( MIUInt w = 0; w != 3; ++w )
                a_streamDest << matTransform.GetRow( v )[ w ];
        EndChunk( a_streamDest );
        EndChunk( a_streamDest );
        EndChunk( a_streamDest );
    }
    EndChunk( a_streamDest );
    EndChunk( a_streamDest );
    return mEResult_Ok;
}
