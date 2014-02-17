#include "mi_include_converters.h"

namespace
{
    enum ESection
    {
        ESection_Mesh      = 1,
        ESection_Skin      = 2,
        ESection_SceneInfo = 7,
        ESection_Nodes     = 11,
        ESection_Materials = 13
    };

    enum EMeshSection
    {
        EMeshSection_Vertices  = 0,
        EMeshSection_Normals   = 1,
        EMeshSection_Tangents  = 2,
        EMeshSection_TexCoords = 3,
        EMeshSection_BaseVerts = 5
    };

    mCMaterial::EMapType arrNativeMapTypes[] = { mCMaterial::EMapType_Diffuse,  // Placeholder
                                                 mCMaterial::EMapType_Diffuse,  // Placeholder
                                                 mCMaterial::EMapType_Diffuse,
                                                 mCMaterial::EMapType_Specular,
                                                 mCMaterial::EMapType_Diffuse,  // Placeholder
                                                 mCMaterial::EMapType_Normal,
                                                 mCMaterial::EMapType_Diffuse,  // Placeholder
                                                 mCMaterial::EMapType_Specular };
}

mEResult mCXmacReader::ReadXmacFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options )
{
    a_sceneDest.Clear();
    mCString strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    if ( a_streamSource.GetSize() < 150 )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xmac file." ), mEResult_False;
    a_streamSource.Seek( 136 );
    MIUInt const uEndXacOffset = a_streamSource.ReadU32() + 140;
    if ( a_streamSource.ReadString( 3 ) != "XAC" )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xmac file." ), mEResult_False;
    mCMultiMaterial matMultiDest( strSceneName.GetLength() ? strSceneName : "MultiMat_xmac" );
    a_streamSource.Seek( 146 );
    if ( a_streamSource.ReadBool() )
        a_streamSource.SetInvertEndianness( MITrue );
    MIUInt uNextSection = 148;
    while ( uNextSection < uEndXacOffset )
    {
        a_streamSource.Seek( uNextSection );
        MIUInt const uSectionID = a_streamSource.ReadU32();
        MIUInt const uSectionSize = a_streamSource.ReadU32() + 12;
        uNextSection += uSectionSize;
        a_streamSource.Skip( 4 );
        if ( uSectionID == ESection_Nodes )
        {
            MIUInt const uNodeCount = a_streamSource.ReadU32();
            a_streamSource.Skip( 4 );
            for ( MIUInt u = 0 ; u != uNodeCount; ++u )
            {
                mCNode & nodeDest = a_sceneDest.AddNewNode();
                mCQuaternion quatRotation;

                a_streamSource >> quatRotation;
                nodeDest.AccessTransform().ModifyRotation( quatRotation.Inverse() );
                a_streamSource.Skip( 16 );
                 a_streamSource >> nodeDest.AccessPosition();
                mCMaxRisenCoordShifter::GetInstance().ShiftMatrixCoords( nodeDest.AccessTransform() );
                a_streamSource.Skip( 32 );
                MIUInt const uParentIndex = a_streamSource.ReadU32();
                if ( uParentIndex < uNodeCount )
                {
                    nodeDest.AccessTransform() *= a_sceneDest.GetNodeAt( uParentIndex )->GetTransform();
                    a_sceneDest.SetNodeParent( u, uParentIndex );
                }
                a_streamSource.Skip( 76 );
                a_streamSource.Read( nodeDest.AccessName(), a_streamSource.ReadU32() );
                nodeDest.AccessIsBone() = nodeDest.GetName().Left( 4 ).ToLower() == "bone";
            }
        }
        else if ( uSectionID == ESection_Materials )
        {
            MIUInt const uMaterialCount = a_streamSource.ReadU32();
            matMultiDest.AccessSubMaterials().Reserve( uMaterialCount );
            a_streamSource.Skip( 8 );
            for ( MIUInt u = 0; u != uMaterialCount; ++u )
            {
                mCMaterial & matDest = matMultiDest.AccessSubMaterials().AddNew();
                a_streamSource.Skip( 95 );
                MIUInt const uMapCount = a_streamSource.ReadU8();
                a_streamSource.Read( matDest.AccessName(), a_streamSource.ReadU32() );
                for ( MIUInt v = 0; v != uMapCount; ++v )
                {
                    a_streamSource.Skip( 26 );
                    MIUInt const uMapType = a_streamSource.ReadU8() % ( sizeof( arrNativeMapTypes ) / sizeof( *arrNativeMapTypes ) );
                    a_streamSource.Skip( 1 );
                    mCTexMap mapDest( "", a_streamSource.ReadString( a_streamSource.ReadU32() ) + "." + a_Options.m_strTextureFileExtension );
                    matDest.AccessTexMap( arrNativeMapTypes[ uMapType ] ) = mapDest;
                }
            }
            uNextSection = a_streamSource.Tell();
        }
        else if ( uSectionID == ESection_Mesh )
        {
            mCNode & nodeDest = *a_sceneDest.AccessNodeAt( a_streamSource.ReadU32() );
            mCMesh meshDest;
            MIUInt const uVertCount = a_streamSource.ReadU32();
            MIUInt const uUVertCount = a_streamSource.ReadU32();
            MIUInt const uFaceCount = a_streamSource.ReadU32() / 3;
            mTArray< MIUInt > arrBaseVerts( 0, uUVertCount );
            a_streamSource.Skip( 4 );
            MIUInt const uMeshSectionCount = a_streamSource.ReadU32();
            a_streamSource.Skip( 4 );
            for ( MIUInt u = uMeshSectionCount; u--; )
            {
                MIUInt uID = a_streamSource.ReadU32();
                MIUInt uBlockSize = a_streamSource.ReadU32();
                a_streamSource.Skip( 4 );
                if ( ( uID == EMeshSection_Vertices ) && ( uBlockSize == 12 ) )
                {
                    meshDest.SetNumVerts( uVertCount );
                    mCVec3 * pVerts = meshDest.AccessVerts();
                    for ( MIUInt v = 0; v != uUVertCount; a_streamSource >> pVerts[ arrBaseVerts[ v++ ] ] );
                }
                else if ( ( uID == EMeshSection_Normals ) && ( uBlockSize == 12 ) )
                {
                    meshDest.SetNumVNormals( uUVertCount );
                    mCVec3 * pVNormals = meshDest.AccessVNormals();
                    for ( MIUInt v = 0; v != uUVertCount; a_streamSource >> pVNormals[ v++ ] );
                }
                else if ( ( uID == EMeshSection_Tangents ) && ( uBlockSize == 16 ) )
                {
                    a_streamSource.Skip( uBlockSize * uUVertCount );
                }
                else if ( ( uID == EMeshSection_TexCoords ) && ( uBlockSize == 8 ) )
                {
                    meshDest.SetNumTVerts( uUVertCount );
                    mCVec3 * pTVerts = meshDest.AccessTVerts();
                    for ( MIUInt v = 0; v != uUVertCount; ++v )
                        a_streamSource >> pTVerts[ v ][ 0 ] >> pTVerts[ v ][ 1 ];
                }
                else if ( ( uID == EMeshSection_BaseVerts ) && ( uBlockSize == 4 ) )
                {
                    for ( MIUInt v = 0; v != uUVertCount; a_streamSource >> g_32( arrBaseVerts[ v++ ] ) );
                }
                else
                {
                    return MI_ERROR( mCStreamError, EBadFormat, "Unknown ._xmac file version." ), mEResult_False;
                }
            }
            meshDest.SetNumFaces( uFaceCount );
            mCMaxFace * pFaces = meshDest.AccessFaces();
            MIUInt uPassedFaceCount = 0;
            MIUInt uPassedVertCount = 0;
            while ( uPassedFaceCount != uFaceCount )
            {
                MIUInt const uPartFaceCount = a_streamSource.ReadU32() / 3;
                MIUInt const uPartVertCount = a_streamSource.ReadU32();
                MIUInt const uMaterialID = a_streamSource.ReadU32();
                MIUInt const uSkipSize = a_streamSource.ReadU32() * 4;
                for ( MIUInt u = 0; u != uPartFaceCount; a_streamSource >> pFaces[ u++ ] )
                    pFaces[ u ].AccessMatID() = uMaterialID;
                for ( MIUInt u = uPartFaceCount; u--;  )
                    for ( MIUInt v = 3; v--; pFaces[ u ][ v ] += uPassedVertCount );
                a_streamSource.Skip( uSkipSize );
                pFaces += uPartFaceCount;
                uPassedFaceCount += uPartFaceCount;
                uPassedVertCount += uPartVertCount;
            }
            if ( meshDest.HasTVFaces() )
                meshDest.CopyTVFacesFromFaces();
            if ( meshDest.HasVNFaces() )
                meshDest.CopyVNFacesFromFaces();
            pFaces = meshDest.AccessFaces();
            for ( MIUInt u = uFaceCount; u--;  )
                for ( MIUInt v = 3; v--; pFaces[ u ][ v ] = arrBaseVerts[ pFaces[ u ][ v ] ] );
            mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
            nodeDest.AccessIsBone() = MIFalse;
            nodeDest.AccessMaterialName() = matMultiDest.GetName();
            nodeDest.SwapMesh( meshDest );
        }
        else if ( uSectionID == ESection_Skin )
        {
            mCNode & nodeDest = *a_sceneDest.AccessNodeAt( a_streamSource.ReadU32() );
            a_streamSource.Skip( 4 );
            MIUInt const uVertCount = nodeDest.GetMesh()->GetNumVerts();
            MIUInt const uWeightCount = a_streamSource.ReadU32();
            MIUInt const uNodeCount = a_sceneDest.GetNumNodes();
            mTArray< MIUInt > arrVertexIndices( 0, uWeightCount );
            mTArray< MIUInt > arrBoneIndices( 0, uWeightCount );
            mTArray< MIFloat > arrWeights( 0.0f, uWeightCount );
            mTArray< MIBool > arrNodesUsedInSkin( MIFalse, uNodeCount );
            a_streamSource.Skip( 4 );
            for ( MIUInt u = 0; u != uWeightCount; ++u )
            {
                a_streamSource >> arrWeights[ u ];
                arrBoneIndices[ u ] = a_streamSource.ReadU16();
                arrNodesUsedInSkin[ arrBoneIndices[ u ] ] = MITrue;
                a_streamSource.Skip( 2 );
            }
            for ( MIUInt u = 0, v = 0, w = 0; u != uWeightCount; ++v )
                for ( a_streamSource >> g_32( w ) >> g_32( w ); w--; arrVertexIndices[ u++ ] = v );
            MIUInt uBoneCount = 0;
            mTArray< MIUInt > arrNewBoneIndices( uNodeCount, uNodeCount );
            for ( MIUInt u = 0; u != uNodeCount; ++u )
                if ( arrNodesUsedInSkin[ u ] )
                    arrNewBoneIndices[ u ] = uBoneCount++;
            for ( MIUInt u = uWeightCount; u--; arrBoneIndices[ u ] = arrNewBoneIndices[ arrBoneIndices[ u ] ] );
            mTArray< mCUnique::ID > arrBoneIDs( mCUnique::ID(), uBoneCount );
            for ( MIUInt u = uNodeCount; u--; )
                if ( arrNewBoneIndices[ u ] != uNodeCount )
                    arrBoneIDs[ arrNewBoneIndices[ u ] ] = a_sceneDest.GetNodeAt( u )->GetID();
            mCSkin skinDest;
            skinDest.InitSwapping( uVertCount, arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
            nodeDest.SwapSkin( skinDest );
        }
    }
    mCMultiMaterial & matMultiDest2 = a_sceneDest.AddNewMultiMaterial();
    matMultiDest2.Swap( matMultiDest );
    a_sceneDest.IdentifyBones();
    if ( mCGenomeMaterial::AccessMaterialLookupHint() )
        mCGenomeMaterial::LoadRisenMaterials( a_sceneDest );
    if ( a_Options.m_strTextureFileExtension != "" )
        for ( mCMaterial * pMat = matMultiDest2.AccessSubMaterials().AccessBuffer(), * pEnd = pMat + matMultiDest2.GetSubMaterials().GetCount(); pMat != pEnd; ( pMat++ )->RemoveEmptyTexMaps() )
            for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
                g_ReplaceFileExt( pMat->AccessTexMap( i ).AccessTextureFilePath(), a_Options.m_strTextureFileExtension );
    a_sceneDest.SetName( strSceneName );
    return mEResult_Ok;
}
