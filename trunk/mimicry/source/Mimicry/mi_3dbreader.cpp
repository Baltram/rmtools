#include "mi_include_converters.h"

namespace
{
    enum { EVersion = 1 };

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

    MIUInt FromIndex( MIUInt a_uIndex )
    {
        if ( a_uIndex )
            return a_uIndex - 1;
        return MI_DW_INVALID;
    }

    void ReadFace( mCFace & a_faceDest, mCIOStreamBinary & a_streamSource )
    {
        a_faceDest[ 0 ] = FromIndex( a_streamSource.ReadU32() );
        a_faceDest[ 1 ] = FromIndex( a_streamSource.ReadU32() );
        a_faceDest[ 2 ] = FromIndex( a_streamSource.ReadU32() );
    }

    void ReadMaterialChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
    {
        mCString strName = a_streamSource.ReadString();
        EMaterialType enuMaterialType = static_cast< EMaterialType >( a_streamSource.ReadI32() );
        if ( enuMaterialType == EMaterialType_Standard )
            a_sceneDest.AddNewMaterial().SetName( strName );
        else if ( enuMaterialType == EMaterialType_Multi )
            a_sceneDest.AddNewMultiMaterial().SetName( strName );
        else if ( enuMaterialType == EMaterialType_Sub )
            dynamic_cast< mCMultiMaterial * >( a_sceneDest.AccessMaterialAt( FromIndex( a_streamSource.ReadU32() ) ) )->AccessSubMaterials().AddNew().SetName( strName );
    }

    void ReadTexMapChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
    {
        MIUInt uMaterialIndex = FromIndex( a_streamSource.ReadU32() );
        MIUInt uSubMaterialIndex = FromIndex( a_streamSource.ReadU32() );
        EMapType enuMapType = static_cast< EMapType >( a_streamSource.ReadI32() );
        mCString strPath = a_streamSource.ReadString();
        mCMaterialBase * pMaterial = a_sceneDest.AccessMaterialAt( uMaterialIndex );
        mCMaterial * pMaterialDest;
        if ( uSubMaterialIndex == MI_DW_INVALID )
            pMaterialDest = dynamic_cast< mCMaterial * >( pMaterial );
        else
            pMaterialDest = &( dynamic_cast< mCMultiMaterial * >( pMaterial )->AccessSubMaterials()[ uSubMaterialIndex ] );
        mCTexMap mapMap( g_GetFileNameNoExt( strPath ), strPath );
        if ( enuMapType == EMapType_Normal )
            pMaterialDest->SetTextureMapAt( mCMaterial::EMapType_Normal, &mapMap );
        else if ( enuMapType == EMapType_Specular )
            pMaterialDest->SetTextureMapAt( mCMaterial::EMapType_Specular, &mapMap );
        else
            pMaterialDest->SetTextureMapAt( mCMaterial::EMapType_Diffuse, &mapMap );
    }

    void ReadNodeChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
    {
        mCNode & nodeDest = a_sceneDest.AddNewNode();
        if ( a_streamSource.ReadU32() == ENodeType_Bone )
            nodeDest.AccessIsBone() = MITrue;
        nodeDest.AccessName() << a_streamSource;
        MIUInt uParentIndex = FromIndex( a_streamSource.ReadU32() );
        MIUInt uMaterialIndex = FromIndex( a_streamSource.ReadU32() );
        if ( uParentIndex != MI_DW_INVALID )
            a_sceneDest.SetNodeParent( a_sceneDest.GetNumNodes() - 1, uParentIndex );
        if ( uMaterialIndex != MI_DW_INVALID )
            nodeDest.AccessMaterialName() = a_sceneDest.GetMaterialAt( uMaterialIndex )->GetName();
        a_streamSource.Read( &nodeDest.AccessTransform(), sizeof( mCMatrix4 ) );
    }

    void ReadMeshChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
    {
        mCNode & nodeDest = *a_sceneDest.AccessNodeAt( FromIndex( a_streamSource.ReadU32() ) );
        mCMesh meshDest;
        MIUInt const uVertCount = a_streamSource.ReadU32();
        meshDest.SetNumVerts( uVertCount );
        mCVec3 * pVerts = meshDest.AccessVerts();
        for ( MIUInt u = 0; u != uVertCount; ++u )
            a_streamSource >> pVerts[ u ];
        MIUInt const uFaceCount = a_streamSource.ReadU32();
        meshDest.SetNumFaces( uFaceCount );
        mCMaxFace * pFaces = meshDest.AccessFaces();
        for ( MIUInt u = 0; u != uFaceCount; ++u )
        {
            ReadFace( pFaces[ u ], a_streamSource );
            pFaces[ u ].AccessMatID() = FromIndex( a_streamSource.ReadU32() );
            a_streamSource >> pFaces[ u ].AccessSGs();
        }
        MIUInt const uTVertCount = a_streamSource.ReadU32();
        if ( uTVertCount )
        {
            meshDest.SetNumTVerts( uTVertCount );
            mCVec3 * pTVerts = meshDest.AccessTVerts();
            mCFace * pTVFaces = meshDest.AccessTVFaces();
            for ( MIUInt u = 0; u != uTVertCount; ++u )
                a_streamSource >> pTVerts[ u ];
            for ( MIUInt u = 0; u != uFaceCount; ++u )
                ReadFace( pTVFaces[ u ], a_streamSource );
        }
        MIU32 const uVColorCount = a_streamSource.ReadU32();
        if ( uVColorCount )
        {
            meshDest.SetHasVertexColors( MITrue );
            mCColor * pVColors = meshDest.AccessVertexColors();
            for ( MIUInt u = 0; u != uVColorCount; ++u )
                a_streamSource.Read( pVColors + u, sizeof( mCColor ) );
        }
        nodeDest.SwapMesh( meshDest );
    }

    void ReadSkinChunk( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
    {
        mCNode & nodeDest = *a_sceneDest.AccessNodeAt( FromIndex( a_streamSource.ReadU32() ) );
        mCSkin skinDest;
        mTArray< mCUnique::ID > arrBoneIDs;
        mTArray< MIUInt > arrVertexIndices;
        mTArray< MIUInt > arrBoneIndices;
        mTArray< MIFloat > arrWeights;
        for ( MIUInt u = a_streamSource.ReadU32(); u--; )
            arrBoneIDs.Add( a_sceneDest.GetNodeAt( FromIndex( a_streamSource.ReadU32() ) )->GetID() );
        MIUInt uVertCount = a_streamSource.ReadU32();
        for ( MIUInt u = 0; u != uVertCount; ++u )
        {
            MIUInt uVertBoneCount = a_streamSource.ReadU32();
            while ( uVertBoneCount-- )
            {
                arrVertexIndices.Add( u );
                arrBoneIndices.Add( FromIndex( a_streamSource.ReadU32() ) );
                arrWeights.Add( a_streamSource.ReadFloat() );
            }
        }
        skinDest.InitSwapping( nodeDest.GetMesh()->GetNumVerts(), arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
        nodeDest.SwapSkin( skinDest );
    }
}

mEResult mC3dbReader::Read3dbFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource )
{
    a_sceneDest.Clear();
    if ( a_streamSource.ReadString() != "3db" )
    {
        MI_ERROR( mCConverterError, EBadFormat, "Invalid .3db file." );
        return mEResult_False;
    }
    MIFloat const f_Angle = a_streamSource.ReadFloat();
    MIUInt uChunkCount = a_streamSource.ReadU32();
    for ( MIUInt uNextChunk = a_streamSource.Tell(); uChunkCount--; a_streamSource.Seek( uNextChunk ) )
    {
        EChunkID enuChunkID = static_cast< EChunkID >( a_streamSource.ReadI32() );
        MIUInt const uVersionMin = a_streamSource.ReadU32();
        uNextChunk += a_streamSource.ReadU32() + 12;
        if ( EVersion < uVersionMin )
            continue;
        switch ( enuChunkID )
        {
        case EChunkID_Material:
            ReadMaterialChunk( a_sceneDest, a_streamSource );
            break;
        case EChunkID_TexMap:
            ReadTexMapChunk( a_sceneDest, a_streamSource );
            break;
        case EChunkID_Node:
            ReadNodeChunk( a_sceneDest, a_streamSource );
            break;
        case EChunkID_Mesh:
            ReadMeshChunk( a_sceneDest, a_streamSource );
            break;
        case EChunkID_Skin:
            ReadSkinChunk( a_sceneDest, a_streamSource );
        }
    }
    if ( f_Angle != -1.0f )
        for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; )
            if ( a_sceneDest.GetNodeAt( u )->HasMesh() )
                a_sceneDest.AccessNodeAt( u )->AccessMesh()->CalcVNormalsByAngle( f_Angle );
    a_sceneDest.IdentifyBones();
    if ( dynamic_cast< mCFileStream * >( &a_streamSource ) )
    {
        a_sceneDest.SetName( g_GetFileName( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) );
    }
    return mEResult_Ok;
}
