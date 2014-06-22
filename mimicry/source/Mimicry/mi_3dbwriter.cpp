#include "mi_include_converters.h"
#include "mi_include_map.h"

#define MI_3DB_CHUNK_BEGIN( STREAM, NAME ) MIUInt uSize = 0;                                                                         \
                                           STREAM << ( MIU32 ) EChunkID_ ## NAME << ( MIU32 ) EMinVersion_ ## NAME << g_32( uSize ); \
                                           MIUInt uOffsetBegin = STREAM.Tell();

#define MI_3DB_CHUNK_END( STREAM ) MIUInt uOffsetEnd = STREAM.Tell(); \
                                   uSize = uOffsetEnd - uOffsetBegin; \
                                   STREAM.Seek( uOffsetBegin - 4 );   \
                                   STREAM << g_32( uSize );           \
                                   STREAM.Seek( uOffsetEnd );         \
                                   ++s_uChunkCount;

namespace
{
    enum { EVersion = 1 };
    enum { EMinVersion = 1 };

    enum { EMinVersion_Material = 1 };
    enum { EMinVersion_TexMap = 1 };
    enum { EMinVersion_Node = 1 };
    enum { EMinVersion_Mesh = 1 };
    enum { EMinVersion_Skin = 1 };

    enum EChunkID
    {
        EChunkID_Material   = 1,
        EChunkID_TexMap     = 2,
        EChunkID_Node       = 3,
        EChunkID_Mesh       = 4,
        EChunkID_Skin       = 5
    };

    enum ENodeType
    {
        ENodeType_Mesh  = 1,
        ENodeType_Bone  = 2,
        ENodeType_Other = 3
    };

    enum EMaterialType
    {
        EMaterialType_Standard = 1,
        EMaterialType_Multi    = 2,
        EMaterialType_Sub      = 3
    };

    enum EMapType
    {
        EMapType_Diffuse  = 2,
        EMapType_Specular = 3,
        EMapType_Normal   = 9
    };

    MIUInt s_uChunkCount;
    MIUInt s_uCurrentMaterialIndex;
    MIUInt s_uCurrentSubMaterialIndex;
    MIUInt s_uCurrentNodeIndex;

    MIU32 AsIndex( MIUInt a_uNumber )
    {
        if ( a_uNumber == MI_DW_INVALID )
            return 0;
        return a_uNumber + 1;
    }

    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << a_strSource << ( MIU8 ) 0;
    }

    void WriteFace( mCIOStreamBinary & a_streamDest, mCFace const & a_faceSource )
    {
        a_streamDest << AsIndex( a_faceSource[ 0 ] );
        a_streamDest << AsIndex( a_faceSource[ 1 ] );
        a_streamDest << AsIndex( a_faceSource[ 2 ] );
    }

    void WriteTexMapChunk( mCIOStreamBinary & a_streamDest, mCTexMap const & a_tmapSource, mCMaterial::EMapType a_enuMapType )
    {
        MI_3DB_CHUNK_BEGIN( a_streamDest, TexMap );
        EMapType enuMapType = EMapType_Diffuse;
        if ( a_enuMapType == mCMaterial::EMapType_Specular )
            enuMapType = EMapType_Specular;
        else if ( a_enuMapType == mCMaterial::EMapType_Normal )
            enuMapType = EMapType_Normal;
        a_streamDest << AsIndex( s_uCurrentMaterialIndex );
        a_streamDest << AsIndex( s_uCurrentSubMaterialIndex );
        a_streamDest << ( MIU32 ) enuMapType;
        WriteString( a_streamDest, a_tmapSource.GetTextureFilePath() );
        MI_3DB_CHUNK_END( a_streamDest );
    }

    void WriteMaterialChunk( mCIOStreamBinary & a_streamDest, mCMaterialBase const * a_pMaterialSource, MIBool a_bIsSubMaterial = MIFalse )
    {
        MI_3DB_CHUNK_BEGIN( a_streamDest, Material );
        WriteString( a_streamDest, a_pMaterialSource->GetName() );
        mCMultiMaterial const * pMultiMatSource = dynamic_cast< mCMultiMaterial const * >( a_pMaterialSource );
        mCMaterial const * pStandardMatSource = dynamic_cast< mCMaterial const * >( a_pMaterialSource );
        a_streamDest << g_32( a_bIsSubMaterial ? EMaterialType_Sub : ( pMultiMatSource ? EMaterialType_Multi : EMaterialType_Standard ) );
        if ( a_bIsSubMaterial )
            a_streamDest << AsIndex( s_uCurrentMaterialIndex );
        MI_3DB_CHUNK_END( a_streamDest );
        if ( !pMultiMatSource )
        {
            if ( !a_bIsSubMaterial )
                s_uCurrentSubMaterialIndex = MI_DW_INVALID;
            for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
                if ( pStandardMatSource->HasTexMap( i ) )
                    WriteTexMapChunk( a_streamDest, *pStandardMatSource->GetTexMap( i ), i );
        }
        else
            for ( s_uCurrentSubMaterialIndex = 0; s_uCurrentSubMaterialIndex != pMultiMatSource->GetSubMaterials().GetCount(); ++s_uCurrentSubMaterialIndex )
                WriteMaterialChunk( a_streamDest, &( pMultiMatSource->GetSubMaterials()[ s_uCurrentSubMaterialIndex ] ), MITrue );
    }

    void WriteNodeChunk( mCIOStreamBinary & a_streamDest, mCScene const & a_sceneSource )
    {
        MI_3DB_CHUNK_BEGIN( a_streamDest, Node );
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( s_uCurrentNodeIndex );
        a_streamDest << ( MIU32 ) ( nodeSource.HasMesh() ? ENodeType_Mesh : ( nodeSource.GetIsBone() ? ENodeType_Bone : ENodeType_Other ) );
        WriteString( a_streamDest, nodeSource.GetName() );
        a_streamDest << AsIndex( a_sceneSource.GetNodeParentIndex( s_uCurrentNodeIndex ) );
        a_streamDest << AsIndex( a_sceneSource.GetMaterialIndexByName( nodeSource.GetMaterialName() ) );
        mCMatrix4 matTransform = nodeSource.GetTransform();
        if ( nodeSource.GetIsBone() )
            matTransform.RemoveScale();
        a_streamDest.Write( &matTransform, sizeof( mCMatrix4 ) );
        MI_3DB_CHUNK_END( a_streamDest );
    }

    void WriteMeshChunk( mCIOStreamBinary & a_streamDest, mCMesh const & a_meshSource, mC3dbWriter::SOptions a_Options )
    {
        MI_3DB_CHUNK_BEGIN( a_streamDest, Mesh );
        a_streamDest << AsIndex( s_uCurrentNodeIndex );
        MIUInt const uVertCount = a_meshSource.GetNumVerts();
        MIUInt const uFaceCount = a_meshSource.GetNumFaces();
        MIUInt const uTVertCount = a_meshSource.GetNumTVerts();
        MIUInt const uVColorCount = ( a_meshSource.HasVertexColors() && !a_Options.m_bDropVertexColors ) ? uVertCount : 0;
        mCVec3 const * pVerts = a_meshSource.GetVerts();
        mCMaxFace const * pFaces = a_meshSource.GetFaces();
        mCVec3 const * pTVerts = a_meshSource.GetTVerts();
        mCFace const * pTVFaces = a_meshSource.GetTVFaces();
        mCColor const * pVColors = a_meshSource.GetVertexColors();
        a_streamDest << g_32( uVertCount );
        for ( MIUInt u = 0; u != uVertCount; ++u )
            a_streamDest << pVerts[ u ];
        a_streamDest << g_32( uFaceCount );
        for ( MIUInt u = 0; u != uFaceCount; ++u )
        {
            WriteFace( a_streamDest, pFaces[ u ] );
            a_streamDest << AsIndex( pFaces[ u ].GetMatID() );
            a_streamDest << pFaces[ u ].GetSGs();
        }
        a_streamDest << g_32( uTVertCount );
        if ( uTVertCount )
        {
            for ( MIUInt u = 0; u != uTVertCount; ++u )
                a_streamDest << pTVerts[ u ];
            for ( MIUInt u = 0; u != uFaceCount; ++u )
                WriteFace( a_streamDest, pTVFaces[ u ] );
        }
        a_streamDest << g_32( uVColorCount );
        for ( MIUInt u = 0; u != uVColorCount; ++u )
            a_streamDest.Write( pVColors + u, sizeof( mCColor ) );
        MI_3DB_CHUNK_END( a_streamDest );
    }

    void WriteSkinChunk( mCIOStreamBinary & a_streamDest, mCScene const & a_sceneSource )
    {
        MI_3DB_CHUNK_BEGIN( a_streamDest, Skin );
        a_streamDest << AsIndex( s_uCurrentNodeIndex );
        mCSkin const & skinSource = *a_sceneSource.GetNodeAt( s_uCurrentNodeIndex )->GetSkin();
        MIUInt const uVertCount = skinSource.GetNumVerts();
        MIUInt const uBoneCount = skinSource.GetNumBones();
        a_streamDest << g_32( uBoneCount ); 
        for ( MIUInt u = 0; u != uBoneCount; ++u )
            a_streamDest << AsIndex( a_sceneSource.GetNodeIndexByID( skinSource.GetBoneIDByIndex( u ) ) );
        a_streamDest << g_32( uVertCount );
        for ( MIUInt u = 0; u != uVertCount; ++u )
        {
            MIUInt uVertBoneCount = skinSource.GetNumInfluencingBones( u );
            a_streamDest << g_32( uVertBoneCount );
            for ( MIUInt v = 0; v != uVertBoneCount; ++v )
            {
                a_streamDest << AsIndex( skinSource.GetBoneIndex( u, v ) );
                a_streamDest << skinSource.GetWeight( u, v );
            }
        }
        MI_3DB_CHUNK_END( a_streamDest );
    }
}

mEResult mC3dbWriter::Write3dbFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    a_sceneSource.SortNodesByLinks();
    WriteString( a_streamDest, "3db" );
    MIFloat fAngle = -1.0f;
    if ( a_Options.m_bRecalculateVertexNormals && a_Options.m_bUseAnglesInsteadOfSGs )
        fAngle = a_Options.m_fMaxAngleInDegrees;
    a_streamDest << fAngle;
    a_streamDest << ( MIU32 ) 0;
    s_uChunkCount = 0;
    for ( s_uCurrentMaterialIndex = 0; s_uCurrentMaterialIndex != a_sceneSource.GetNumMaterials(); ++s_uCurrentMaterialIndex )
        WriteMaterialChunk( a_streamDest, a_sceneSource.GetMaterialAt( s_uCurrentMaterialIndex ) );
    for ( s_uCurrentNodeIndex = 0; s_uCurrentNodeIndex != a_sceneSource.GetNumNodes(); ++s_uCurrentNodeIndex )
        WriteNodeChunk( a_streamDest, a_sceneSource );
    for ( s_uCurrentNodeIndex = 0; s_uCurrentNodeIndex != a_sceneSource.GetNumNodes(); ++s_uCurrentNodeIndex )
        if ( a_sceneSource.GetNodeAt( s_uCurrentNodeIndex )->HasMesh() )
            WriteMeshChunk( a_streamDest, *a_sceneSource.GetNodeAt( s_uCurrentNodeIndex )->GetMesh(), a_Options );
    for ( s_uCurrentNodeIndex = 0; s_uCurrentNodeIndex != a_sceneSource.GetNumNodes(); ++s_uCurrentNodeIndex )
        if ( a_sceneSource.GetNodeAt( s_uCurrentNodeIndex )->HasSkin() )
            WriteSkinChunk( a_streamDest, a_sceneSource );
    a_streamDest.Seek( 8 );
    a_streamDest << g_32( s_uChunkCount );
    return mEResult_Ok;
}
