#include "mi_include_converters.h"
#include "mi_include_variant.h"

namespace
{
    mCMaterialBase * ReadMaterial( mTArray< mCVariant > & a_arrChunk ) 
    {
        static mCMaterial mtlDest;
        static mCMultiMaterial mtlDestMulti;
        mCString strName;
        MIBool bIsMultiMaterial = MIFalse;
        MIInt iCount = 0;
        a_arrChunk[ 3 ].SwapData( strName );
        a_arrChunk[ 4 ].SwapData( bIsMultiMaterial );
        a_arrChunk[ 5 ].SwapData( iCount );
        if ( bIsMultiMaterial )
        {
            mtlDestMulti = mCMultiMaterial( strName );
            mTArray< mCMaterial > & arrSubs = mtlDestMulti.AccessSubMaterials();
            arrSubs.Resize( iCount );
            for ( MIUInt u = iCount; u--; )
            {
                mTArray< mCVariant > arrSubMaterialChunk;
                a_arrChunk[ 6 + u ].SwapData( arrSubMaterialChunk );
                mCMaterialBase * pSub = ReadMaterial( arrSubMaterialChunk );
                mCMaterial * pSubMat = dynamic_cast< mCMaterial * >( pSub );
                if ( pSubMat )
                    arrSubs[ u ] = *pSubMat;
                else
                    arrSubs[ u ].SetName( pSub->GetName() );
            }
            return &mtlDestMulti;
        }
        else
        {
            mtlDest = mCMaterial( strName );
            for ( MIUInt u = iCount; u--; )
            {
                mTArray< mCVariant > arrMap;
                a_arrChunk[ 6 + u ].SwapData( arrMap );
                mCString strType;
                mCString strTexturePath;
                arrMap[ 0 ].SwapData( strType );
                arrMap[ 1 ].SwapData( strTexturePath );
                mCTexMap mapDest( strType, strTexturePath );
                if ( strType == "diffuse" )
                    mtlDest.SetTextureMapAt( mCMaterial::EMapType_Diffuse, &mapDest );
                else if ( strType == "specular" )
                    mtlDest.SetTextureMapAt( mCMaterial::EMapType_Specular, &mapDest );
                else if ( strType == "bump" )
                    mtlDest.SetTextureMapAt( mCMaterial::EMapType_Normal, &mapDest );
            }
            return &mtlDest;
        }
    }

    void ReadMaterialChunk( mTArray< mCVariant > & a_arrChunk, mCScene & a_sceneDest ) 
    {
        a_sceneDest.AddMaterial( *ReadMaterial( a_arrChunk ) );
    }

    void ReadNodeChunk( mTArray< mCVariant > & a_arrChunk, mCScene & a_sceneDest ) 
    {
        mCString strName;
        MIInt iParent;
        MIInt iMaterial;
        mCVec3 vecPosition;
        mCMatrix4 matRotation;
        a_arrChunk[ 3 ].SwapData( strName );
        a_arrChunk[ 4 ].SwapData( iParent );
        a_arrChunk[ 5 ].SwapData( iMaterial );
        a_arrChunk[ 6 ].SwapData( vecPosition );
        a_arrChunk[ 7 ].SwapData( matRotation );
        mCNode & nodeDest = a_sceneDest.AddNewNode();
        nodeDest.AccessName() = strName;
        nodeDest.AccessTransform() = matRotation;
        if ( iMaterial-- )
            nodeDest.AccessMaterialName() = a_sceneDest.GetMaterialAt( iMaterial )->GetName();
        if ( iParent-- )
            a_sceneDest.SetNodeParent( a_sceneDest.GetNumNodes() - 1, iParent );
    }

    void ReadMeshChunk( mTArray< mCVariant > & a_arrChunk, mCScene & a_sceneDest ) 
    {
        MIInt iNode;
        mTArray< mCVariant > arrVariantVerts;
        mTArray< mCVariant > arrVariantFaces;
        mTArray< mCVariant > arrVariantMatIDs;
        mTArray< mCVariant > arrVariantSGs;
        mTArray< mCVariant > arrVariantTVerts;
        mTArray< mCVariant > arrVariantTVFaces;
        mTArray< mCVariant > arrVariantVColors;
        mTArray< mCVariant > arrVariantVCFaces;

        a_arrChunk[ 3 ].SwapData( iNode );
        a_arrChunk[ 4 ].SwapData( arrVariantVerts );
        a_arrChunk[ 5 ].SwapData( arrVariantFaces );
        a_arrChunk[ 6 ].SwapData( arrVariantMatIDs );
        a_arrChunk[ 7 ].SwapData( arrVariantSGs );
        a_arrChunk[ 8 ].SwapData( arrVariantTVerts );
        a_arrChunk[ 9 ].SwapData( arrVariantTVFaces );
        a_arrChunk[ 10 ].SwapData( arrVariantVColors );
        a_arrChunk[ 11 ].SwapData( arrVariantVCFaces );

        MIUInt const uNumVerts = arrVariantVerts.GetCount();
        MIUInt const uNumFaces = arrVariantFaces.GetCount();
        MIUInt const uNumTVerts = arrVariantTVerts.GetCount();
        MIUInt const uNumVColors = arrVariantVColors.GetCount();

        mTArray< mCVec3 > arrVerts( mCVec3(), uNumVerts );
        mTArray< mCVec3 > arrVecFaces( mCVec3(), uNumFaces );
        mTArray< mCMaxFace > arrFaces( mCMaxFace(), uNumFaces );
        mTArray< MIInt > arrMatIDs( 00, uNumFaces );
        mTArray< MIInt > arrSGs( 00, uNumFaces );
        mTArray< mCVec3 > arrTVerts( mCVec3(), uNumTVerts );
        mTArray< mCVec3 > arrVecTVFaces( mCVec3(), uNumTVerts ? uNumFaces : 0 );
        mTArray< mCFace > arrTVFaces( mCFace(), uNumFaces );
        mTArray< mCColor > arrVColors( mCColor(), uNumVColors );
        mTArray< mCVec3 > arrVecVCFaces( mCVec3(), uNumVColors ? uNumFaces : 0 );
        mTArray< mCFace > arrVCFaces( mCFace(), uNumFaces );

        for ( MIUInt u = uNumVerts; u--; arrVariantVerts[ u ].GetData( arrVerts[ u ] ) );
        for ( MIUInt u = uNumFaces; u--; arrVariantFaces[ u ].GetData( arrVecFaces[ u ] ) );
        for ( MIUInt u = uNumFaces; u--; arrVariantMatIDs[ u ].GetData( arrMatIDs[ u ] ) );
        for ( MIUInt u = uNumFaces; u--; arrVariantSGs[ u ].GetData( arrSGs[ u ] ) );
        for ( MIUInt u = uNumFaces; u--; )
        {
            mCVec3 & vecFace = arrVecFaces[ u ];
            arrFaces[ u ] = mCMaxFace( static_cast< MIUInt >( vecFace.AccessX() ) - 1, 
                                       static_cast< MIUInt >( vecFace.AccessY() ) - 1, 
                                       static_cast< MIUInt >( vecFace.AccessZ() ) - 1,
                                       static_cast< MIUInt >( arrMatIDs[ u ] ) - 1,
                                       static_cast< MIU32 >( arrSGs[ u ] ) );
        }
        if ( uNumTVerts )
        {
            for ( MIUInt u = uNumTVerts; u--; arrVariantTVerts[ u ].GetData( arrTVerts[ u ] ) );
            for ( MIUInt u = uNumFaces; u--; arrVariantTVFaces[ u ].GetData( arrVecTVFaces[ u ] ) );
            for ( MIUInt u = uNumFaces; u--; )
            {
                mCVec3 & vecTVFace = arrVecTVFaces[ u ];
                arrTVFaces[ u ] = mCFace( static_cast< MIUInt >( vecTVFace.AccessX() ) - 1, 
                                          static_cast< MIUInt >( vecTVFace.AccessY() ) - 1, 
                                          static_cast< MIUInt >( vecTVFace.AccessZ() ) - 1 );
            }
        }
        if ( uNumVColors )
        {
            for ( MIUInt u = uNumVColors; u--; arrVariantVColors[ u ].GetData( arrVColors[ u ] ) );
            for ( MIUInt u = uNumFaces; u--; arrVariantVCFaces[ u ].GetData( arrVecVCFaces[ u ] ) );
            for ( MIUInt u = uNumFaces; u--; )
            {
                mCVec3 & vecVCFace = arrVecVCFaces[ u ];
                arrVCFaces[ u ] = mCFace( static_cast< MIUInt >( vecVCFace.AccessX() ) - 1, 
                                          static_cast< MIUInt >( vecVCFace.AccessY() ) - 1, 
                                          static_cast< MIUInt >( vecVCFace.AccessZ() ) - 1 );
            }
        }

        mCMesh meshDest;
        meshDest.SetNumVerts( uNumVerts );
        meshDest.SetNumFaces( uNumFaces );
        meshDest.SetNumTVerts( uNumTVerts );
        meshDest.SetHasVertexColors( uNumVColors ? MITrue : MIFalse );
        g_memcpy( meshDest.AccessVerts(), arrVerts.AccessBuffer(), uNumVerts * sizeof( mCVec3 ) );
        g_memcpy( meshDest.AccessFaces(), arrFaces.AccessBuffer(), uNumFaces * sizeof( mCMaxFace ) );
        if ( uNumTVerts )
        {
            g_memcpy( meshDest.AccessTVerts(), arrTVerts.AccessBuffer(), uNumTVerts * sizeof( mCVec3 ) );
            g_memcpy( meshDest.AccessTVFaces(), arrTVFaces.AccessBuffer(), uNumFaces * sizeof( mCFace ) );
        }
        if ( uNumVColors )
        {
            mCMaxFace * pFaces = meshDest.AccessFaces();
            mCColor * pVColors = meshDest.AccessVertexColors();
            for ( MIUInt u = uNumFaces; u--; )
                for ( MIUInt v = 3; v--; )
                    pVColors[ pFaces[ u ][ v ] ] = arrVColors[ arrVCFaces[ u ][ v ] ];
        }
        a_sceneDest.AccessNodeAt( iNode - 1 )->SwapMesh( meshDest );
    }

    void ReadSkinChunk( mTArray< mCVariant > & a_arrChunk, mCScene & a_sceneDest ) 
    {
        MIInt iNode;
        mTArray< mCVariant > arrVariantNodeIndexPerBone;
        mTArray< mCVariant > arrVariantVertexIndices;
        mTArray< mCVariant > arrVariantBoneIndices;
        mTArray< mCVariant > arrVariantWeights;

        a_arrChunk[ 3 ].SwapData( iNode );
        a_arrChunk[ 4 ].SwapData( arrVariantNodeIndexPerBone );
        a_arrChunk[ 5 ].SwapData( arrVariantVertexIndices );
        a_arrChunk[ 6 ].SwapData( arrVariantBoneIndices );
        a_arrChunk[ 7 ].SwapData( arrVariantWeights );

        MIUInt const uWeightCount = arrVariantWeights.GetCount();
        MIUInt const uBoneCount = arrVariantNodeIndexPerBone.GetCount();
        MIUInt uVertCount = 0;
        mTArray< mCUnique::ID > arrBoneIDs( mCUnique::ID(), uBoneCount );
        mTArray< MIUInt > arrVertexIndices( 0, uWeightCount ); 
        mTArray< MIUInt > arrBoneIndices( 0, uWeightCount );
        mTArray< MIFloat > arrWeights( 0.0f, uWeightCount );
        
        for ( MIUInt u = uBoneCount; u--; arrBoneIDs[ u ] = a_sceneDest.GetNodeAt( arrVariantNodeIndexPerBone[ u ].GetData< MIInt >() - 1 )->GetID() );
        for ( MIUInt u = uWeightCount; u--; arrVertexIndices[ u ] = arrVariantVertexIndices[ u ].GetData< MIInt >() - 1 );
        for ( MIUInt u = uWeightCount; u--; arrBoneIndices[ u ] = arrVariantBoneIndices[ u ].GetData< MIInt >() - 1 );
        for ( MIUInt u = uWeightCount; u--; arrWeights[ u ] = arrVariantWeights[ u ].GetData< MIFloat >() );
        uVertCount = arrVertexIndices[ uWeightCount - 1 ] + 1;

        mCSkin skinDest;
        skinDest.InitSwapping( uVertCount, arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
        a_sceneDest.AccessNodeAt( iNode - 1 )->SwapSkin( skinDest );
    }
}

mEResult mCMaxReader::ReadInMaxFileData( mCScene & a_sceneDest, mCMaxFileStream & a_streamSource )
{
    MIInt const iMinVersion = 1;
    MIInt const iVersion = 1;
    mCVariant rootVariant;
    a_sceneDest.Clear();
    if ( a_streamSource.ReadPersistentGlobal( "_extendedSaveData", rootVariant ) != mEResult_Ok )
    {
        MI_ERROR( mCConverterError, EBadFormat, "No valid max file with extended saving." );
        return mEResult_False;
    }
    for ( ; ; )
    {
        mTArray< mCVariant > arrChunkVariants;
        if ( rootVariant.SwapData( arrChunkVariants ) != mEResult_Ok )
            break;
        for ( MIUInt u = 0, ue = arrChunkVariants.GetCount(); u != ue; ++u )
        {
            mTArray< mCVariant > arrChunk;
            if ( arrChunkVariants[ u ].SwapData( arrChunk ) != mEResult_Ok )
                break;
            if ( arrChunk.GetCount() < 3 )
                break;
            MIInt iChunkMinVersion = 0;
            MIInt iChunkVersion = 0;
            mCString strChunkName;
            arrChunk[ 0 ].SwapData( iChunkMinVersion );
            arrChunk[ 1 ].SwapData( iChunkVersion );
            arrChunk[ 2 ].SwapData( strChunkName );
            strChunkName.ToLower();
            if ( ( iChunkVersion < iMinVersion ) || ( iVersion < iChunkMinVersion ) )
                continue;
            if ( strChunkName == "material" )
            {
                ReadMaterialChunk( arrChunk, a_sceneDest );
            }
            else if ( strChunkName == "node" )
            {
                ReadNodeChunk( arrChunk, a_sceneDest );
            }
            else if ( strChunkName == "mesh" )
            {
                ReadMeshChunk( arrChunk, a_sceneDest );
            }
            else if ( strChunkName == "skin" )
            {
                ReadSkinChunk( arrChunk, a_sceneDest );
            }
            else
            {
                break;
            }
        }
        a_sceneDest.SetName( a_streamSource.GetFileName() );
        return mEResult_Ok;
    }
    MI_ERROR( mCConverterError, EBadFormat, "Unknown extended saving version." );
    return mEResult_False;
}