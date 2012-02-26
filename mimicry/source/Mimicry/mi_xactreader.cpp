#include "mi_include_converters.h"

mCXactReader::SOptions::SOptions( void )
{
}

namespace
{
    enum ESection
    {
        ESection_Node               = 0,
        ESection_Mesh               = 3,
        ESection_Skin               = 4,
        ESection_Material           = 6,
        ESection_TexMap             = 7,
        ESection_AdditionalNodeData = 8,
        ESection_AdditionalBoneData = 9,
        ESection_SceneInfo          = 16
    };

    enum ETexMapType
    {
        ETexMapType_Diffuse = 1,
        ETexMapType_Normal = 3,
        ETexMapType_Specular = 9
    };

    void InitNode( mCScene & a_sceneDest, MIUInt a_uNodeIndex, mTArray< mCString > const & a_arrParentNames, mTArray< MIBool > & a_arrIsNodeInitialized ) 
    {
        if ( a_arrIsNodeInitialized[ a_uNodeIndex ] )
            return;
        mCNode & nodeDest = *a_sceneDest.AccessNodeAt( a_uNodeIndex );
        MIUInt uParentIndex = a_sceneDest.GetNodeIndexByName( a_arrParentNames[ a_uNodeIndex ] );
        if ( uParentIndex != MI_DW_INVALID )
        {
            InitNode( a_sceneDest, uParentIndex, a_arrParentNames, a_arrIsNodeInitialized );
            nodeDest.AccessTransform() *= a_sceneDest.GetNodeAt( uParentIndex )->GetTransform();
            a_sceneDest.SetNodeParent( a_uNodeIndex, uParentIndex );
        }
        else
        {
            mCMaxRisenCoordShifter::GetInstance().ShiftMatrixCoords( nodeDest.AccessTransform() );
        }
        a_arrIsNodeInitialized[ a_uNodeIndex ] = MITrue;
    }
}

mEResult mCXactReader::ReadXactFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options )
{
    a_sceneDest.Clear();
    if ( dynamic_cast< mCFileStream * >( &a_streamSource ) )
    {
        a_sceneDest.SetName( g_GetFileName( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) );
    }
    mTArray< mCString > arrParentNames;
    mCString const strMultiMatName = "MultiMat_" + a_sceneDest.GetName();
    mCMultiMaterial matMultiDest;
    matMultiDest.SetName( strMultiMatName );
    a_streamSource.Seek( 74 );
    MIUInt const uEndFxaOffset = a_streamSource.ReadU32() + 78;
    if ( a_streamSource.ReadU32() != 0x20415846 )
    {
        MI_ERROR( mCConverterError, EBadFormat, "Invalid source .xact file." );
        return mEResult_False;
    }
    MIUInt uNextSection = a_streamSource.Tell() + 2;
    while ( uNextSection < uEndFxaOffset )
    {
        a_streamSource.Seek( uNextSection );
        MIUInt const uSectionID = a_streamSource.ReadU32();
        MIUInt const uSectionSize = a_streamSource.ReadU32() + 12;
        uNextSection += uSectionSize;
        a_streamSource.Skip( 4 );
        if ( uSectionID == ESection_Node )
        {
            mCVec3 vecPos;
            mCString strName;
            mCQuaternion quatRotation;
            a_streamSource >> vecPos;
            a_streamSource >> quatRotation;
            a_streamSource.Skip( 40 );
            a_streamSource.Read( strName, a_streamSource.ReadU32() );
            a_streamSource.Read( arrParentNames.AddNew(), a_streamSource.ReadU32() );
            mCNode & nodeDest = a_sceneDest.AddNewNode();
            nodeDest.AccessName() = strName;
            nodeDest.AccessTransform().ModifyRotation( quatRotation.Inverse() );
            nodeDest.AccessPosition() = vecPos;
        }
        else if ( uSectionID == ESection_Mesh )
        {
            MIUInt uNodeIndex = a_streamSource.ReadU32();
            MIUInt uVertCount = a_streamSource.ReadU32();
            MIUInt uUVertCount = a_streamSource.ReadU32();
            MIUInt uFaceCount = a_streamSource.ReadU32() / 3;
            MIUInt uMaterialCount = a_streamSource.ReadU32();
            MIUInt uChannelCount = a_streamSource.ReadU32();
            a_streamSource.Skip( 4 );
            mCMesh meshDest;
            meshDest.SetNumVerts( uVertCount );
            meshDest.SetNumFaces( uFaceCount );
            meshDest.SetNumTVerts( uChannelCount ? uUVertCount : 0 );
            meshDest.SetNumVNormals( uUVertCount );
            mTArray< MIUInt > arrVertIndexPerUVert( 0, uUVertCount );
            MIUInt * pVertIndices = arrVertIndexPerUVert.AccessBuffer();
            mCVec3 * pVerts = meshDest.AccessVerts();
            mCMaxFace * pFaces = meshDest.AccessFaces();
            mCVec3 * pTVerts = meshDest.AccessTVerts();
            mCFace * pTVFaces = meshDest.AccessTVFaces();
            mCVec3 * pVNormals = meshDest.AccessVNormals();
            mCFace * pVNFaces = meshDest.AccessVNFaces();
            MIUInt uTotalPassedVertCount = 0;
            for ( MIUInt u = 0; u != uMaterialCount; ++u )
            {
                MIUInt uMatID = a_streamSource.ReadU8();
                a_streamSource.Skip( 3 );
                MIUInt const uPartFaceCount = a_streamSource.ReadU32() / 3;
                MIUInt const uPartUVertCount = a_streamSource.ReadU32();
                for ( MIUInt v = 0; v != uPartUVertCount; ++v )
                {
                    a_streamSource >> g_32( pVertIndices[ v ] );
                    a_streamSource >> pVerts[ pVertIndices[ v ] ];
                    a_streamSource >> pVNormals[ v ];
                    if ( uChannelCount )
                    {
                        a_streamSource >> pTVerts[ v ];
                        pTVerts[ v ].AccessZ() = 0.0f;
                        a_streamSource.Skip( uChannelCount * 8 - 12 );
                    }
                }
                for ( MIUInt v = 0; v != uPartFaceCount; ++v )
                {
                    mCMaxFace & faceDest = pFaces[ v ];
                    a_streamSource >> faceDest;
                    faceDest.AccessMatID() = uMatID;
                    for ( MIUInt w = 3; w--; faceDest[ w ] += uTotalPassedVertCount );
                    pVNFaces[ v ] = faceDest;
                    if ( uChannelCount )
                        pTVFaces[ v ] = faceDest;
                    for ( MIUInt w = 3; w--; faceDest[ w ] = pVertIndices[ faceDest[ w ] ] );
                }
                uTotalPassedVertCount += uPartUVertCount;
                pVNormals += uPartUVertCount;
                pTVerts += uPartUVertCount;
                pFaces += uPartFaceCount;
                pTVFaces += uPartFaceCount;
                pVNFaces += uPartFaceCount;
            }
            mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
            a_sceneDest.AccessNodeAt( uNodeIndex )->SwapMesh( meshDest );
            a_sceneDest.AccessNodeAt( uNodeIndex )->AccessMaterialName() = strMultiMatName;
        }
        else if ( uSectionID == ESection_Skin )
        {
            mCNode & nodeDest = *a_sceneDest.AccessNodeAt( a_streamSource.ReadU32() );
            MIUInt const uVertCount = nodeDest.GetMesh()->GetNumVerts();
            MIUInt const uWeightCount = ( uSectionSize - 12 - 4 - uVertCount ) / 8;
            mTArray< mCUnique::ID > arrBoneIDs( mCUnique::ID(), a_sceneDest.GetNumNodes() );
            mTArray< MIUInt > arrVertexIndices( 0, uWeightCount ); 
            mTArray< MIUInt > arrBoneIndices( 0, uWeightCount ); 
            mTArray< MIFloat > arrWeights( 0.0f, uWeightCount );
            for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; arrBoneIDs[ u ] = a_sceneDest.GetNodeAt( u )->GetID() );
            for ( MIUInt u = 0, w = 0; u != uVertCount; ++u )
            {
                for ( MIUInt v = a_streamSource.ReadU8(); v--; w++ )
                {
                    arrVertexIndices[ w ] = u;
                    arrBoneIndices[ w ] = a_streamSource.ReadU16();
                    a_streamSource.Skip( 2 );
                    arrWeights[ w ] = a_streamSource.ReadFloat();
                }
            }
            mCSkin skinDest;
            skinDest.InitSwapping( uVertCount, arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
            nodeDest.SwapSkin( skinDest );
        }
        else if ( uSectionID == ESection_Material )
        {
            mCString strName;
            a_streamSource.Skip( 68 );
            a_streamSource.Read( strName, a_streamSource.ReadU32() );
            mCMaterial & matDest = matMultiDest.AccessSubMaterials().AddNew();
            matDest.SetName( strName );
        }
        else if ( uSectionID == ESection_TexMap )
        {
            mCString strPath;
            MIU8 u8Type = a_streamSource.ReadU8();
            a_streamSource.Skip( 27 );
            a_streamSource.Read( strPath, a_streamSource.ReadU32() );
            mCMaterial & matDest = matMultiDest.AccessSubMaterials().Back();
            mCTexMap tmapDest( "", strPath + a_Options.m_strTextureFileExtension );
            if ( u8Type == ETexMapType_Diffuse )
                matDest.SetTextureMapAt( mCMaterial::EMapType_Diffuse, &tmapDest );
            else if ( u8Type == ETexMapType_Normal )
                matDest.SetTextureMapAt( mCMaterial::EMapType_Normal, &tmapDest );
            else if ( u8Type == ETexMapType_Specular )
                matDest.SetTextureMapAt( mCMaterial::EMapType_Specular, &tmapDest );
        }
    }
    a_sceneDest.AddNewMultiMaterial().Swap( matMultiDest );
    mTArray< MIBool > arrIsNodeInitialized( MIFalse, a_sceneDest.GetNumNodes() );
        for ( MIUInt u = a_sceneDest.GetNumNodes(); u--; )
            InitNode( a_sceneDest, u, arrParentNames, arrIsNodeInitialized );
    return mEResult_Ok;
}