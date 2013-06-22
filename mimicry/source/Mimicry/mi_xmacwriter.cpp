#include "mi_include_converters.h"

mCXmacWriter::SOptions::SOptions( void ) :
    m_bReplaceOnlyVertices( MIFalse ),
    m_bIndirectVertexMatching( MITrue ),
    m_pBaseXmacStream( 0 )
{
    m_fMaxAngleInDegrees = 180.0f;
}

namespace
{
    enum ESection
    {
        ESection_Mesh      = 1,
        ESection_Skin      = 2,
        ESection_Material  = 3,
        ESection_SceneInfo = 7,
        ESection_Nodes     = 11,
        ESection_Morph     = 12,
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

    mCMaterial::EMapType arrNativeMapTypes[] = { mCMaterial::EMapType_Count,
                                                 mCMaterial::EMapType_Count,
                                                 mCMaterial::EMapType_Diffuse,
                                                 mCMaterial::EMapType_Specular,
                                                 mCMaterial::EMapType_Count,
                                                 mCMaterial::EMapType_Normal };

    void Sync( mCIOStreamBinary & a_streamDest, mCIOStreamBinary & a_streamSource, MIUInt & a_uSycOffset, MIUInt a_uSize )
    {
        static mCByteArray arrBuffer;
        arrBuffer.Resize( a_uSize );
        a_streamSource.Seek( a_uSycOffset );
        a_streamSource.Read( arrBuffer.AccessBuffer(), a_uSize );
        a_streamDest.Write( arrBuffer.GetBuffer(), a_uSize );
        a_uSycOffset += a_uSize;
    }
}

mEResult mCXmacWriter::WriteXmacFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    mCIOStreamBinary & streamBaseXmac = *a_Options.m_pBaseXmacStream;
    mCScene sceneBase;
    if ( mCXmacReader::ReadXmacFileData( sceneBase, streamBaseXmac ) == mEResult_False )
        return mEResult_False;
    streamBaseXmac.SetInvertEndianness( MIFalse );

    // Collect all materials that will be used in the final ._xmac file. Indiecs of materials from sceneBase are preserved.
    mTArray< mCMaterial const * > arrMaterials;
    mTArray< mCString > arrCommonMeshes;
    mCMaterial matEmfxDefault( "EMFX_Default" ), matEmfxDefault2( matEmfxDefault );
    for ( MIUInt u = a_sceneSource.GetNumNodes(), v = 0; u--; )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( u );
        if ( !nodeSource.HasMesh() || ( ( v = sceneBase.GetNodeIndexByName( nodeSource.GetName() ) ) == MI_DW_INVALID ) || ( !sceneBase.GetNodeAt( v )->HasMesh() ) )
            continue;
        arrCommonMeshes.Add( nodeSource.GetName() );
        if ( a_Options.m_bReplaceOnlyVertices )
        {
            if ( sceneBase.GetNodeAt( v )->GetMesh()->GetNumVerts() != nodeSource.GetMesh()->GetNumVerts() )
                return MI_ERROR( mCConverterError, EMiscellaneous, "New mesh and base mesh vertex counts differ." ), mEResult_False;
            continue;
        }
        MIUInt const uMatIndex = a_sceneSource.GetMaterialIndexByName( nodeSource.GetMaterialName() );
        if ( uMatIndex == MI_DW_INVALID )
        {
            arrMaterials.Add( &matEmfxDefault );
            continue;
        }
        mCMaterialBase const * pMaterialBase = a_sceneSource.GetMaterialAt( uMatIndex );
        mCMultiMaterial const * pMultiMaterial = dynamic_cast< mCMultiMaterial const * >( pMaterialBase );
        if ( !pMultiMaterial )
            arrMaterials.Add( dynamic_cast< mCMaterial const * >( pMaterialBase ) );
        else
            for ( MIUInt u = 0, ue = pMultiMaterial->GetSubMaterials().GetCount(); u != ue; ++u )
                arrMaterials.Add( &pMultiMaterial->GetSubMaterials()[ u ] );
    }
    if ( arrCommonMeshes.GetCount() == 0 )
        return MI_ERROR( mCConverterError, EMiscellaneous, "No common mesh in base ._xmac file found." ), mEResult_False;
    mTArray< mCMaterial > const & arrBaseMaterials = dynamic_cast< mCMultiMaterial const * >( sceneBase.GetMaterialAt( 0 ) )->GetSubMaterials();
    mTArray< mCMaterial const * > arrBaseMaterialPtrs( 0, arrBaseMaterials.GetCount() );
    if ( !a_Options.m_bReplaceOnlyVertices )
    {
        for ( MIUInt u = arrBaseMaterials.GetCount(); u--; )
            for ( MIUInt v = arrMaterials.GetCount(); v--; )
                if ( ( arrBaseMaterialPtrs[ u ] = &arrBaseMaterials[ u ] )->GetName() == arrMaterials[ v ]->GetName() )
                    arrBaseMaterialPtrs[ u ] = arrMaterials[ v ], v = 0;
        for ( MIUInt u = sceneBase.GetNumNodes(), uMatID = MI_DW_INVALID; u--; )
        {
            mCNode const & nodeBase = *sceneBase.GetNodeAt( u );
            if ( nodeBase.HasMesh() && ( arrCommonMeshes.IndexOf( nodeBase.GetName() ) == MI_DW_INVALID  ) )
                for ( mCMaxFace const * pFace = nodeBase.GetMesh()->GetFaces(), * pEnd = pFace + nodeBase.GetMesh()->GetNumFaces(); pFace != pEnd; ++pFace )
                    if ( uMatID != pFace->GetMatID() )
                        uMatID = pFace->GetMatID(), arrMaterials.Add( arrBaseMaterialPtrs[ uMatID ] );
        }
        arrMaterials.RemoveDuplicates();
        while ( arrMaterials.GetCount() < arrBaseMaterialPtrs.GetCount() )
            arrMaterials.Add( &matEmfxDefault2 );
        for ( MIUInt u = arrBaseMaterialPtrs.GetCount(), v; u--; )
            if ( ( v = arrMaterials.IndexOf( arrBaseMaterialPtrs[ u ] ) ) != MI_DW_INVALID )
                g_swap( arrMaterials[ u ], arrMaterials[ v ] );
        for ( MIUInt u = arrMaterials.GetCount(); u-- && ( arrMaterials[ u ] == &matEmfxDefault2 ); )
            arrMaterials.RemoveAt( u );
    }

    // Ensure that all nodes from sceneBase are available in a_sceneSource.
    mTArray< MIUInt > arrBaseNodeIndices( MI_DW_INVALID, a_sceneSource.GetNumNodes() );
    for ( MIUInt u = sceneBase.GetNumNodes(), v = 0; u--; arrBaseNodeIndices[ v ] = u )
        if ( ( v = a_sceneSource.GetNodeIndexByName( sceneBase.GetNodeAt( u )->GetName() ) ) == MI_DW_INVALID )
            a_sceneSource.AddNewNode().AccessName() = sceneBase.GetNodeAt( u )->GetName(), v = arrBaseNodeIndices.GetCount(), arrBaseNodeIndices.AddNew();

    // Update face material ids and perform auto skinning.
    if ( !a_Options.m_bReplaceOnlyVertices )
    {
        for ( MIUInt u = arrCommonMeshes.GetCount(); u--; )
        {
            mCNode & nodeSource = *a_sceneSource.AccessNodeAt( a_sceneSource.GetNodeIndexByName( arrCommonMeshes[ u ] ) );
            mCNode & nodeBase = *sceneBase.AccessNodeAt( sceneBase.GetNodeIndexByName( arrCommonMeshes[ u ] ) );
            mCMesh & meshSouce = *nodeSource.AccessMesh();
            MIUInt const uMaterialIndex = a_sceneSource.GetMaterialIndexByName( nodeSource.GetMaterialName() );
            mTArray< MIUInt > arrNewMatIDs( arrMaterials.IndexOf( &matEmfxDefault ), 1 );
            if ( uMaterialIndex != MI_DW_INVALID )
            {
                arrNewMatIDs.Clear();
                mCMultiMaterial const * pMultimaterial = dynamic_cast< mCMultiMaterial const * >( a_sceneSource.GetMaterialAt( uMaterialIndex ) );
                if ( !pMultimaterial )
                    arrNewMatIDs.Add( arrMaterials.IndexOf( dynamic_cast< mCMaterial const * >( a_sceneSource.GetMaterialAt( uMaterialIndex ) ) ) );
                else
                    for ( MIUInt u = 0, ue = pMultimaterial->GetSubMaterials().GetCount(); u != ue; ++u )
                        arrNewMatIDs.Add( arrMaterials.IndexOf( &pMultimaterial->GetSubMaterials()[ u ] ) );
            }
            for ( mCMaxFace * pFace = meshSouce.AccessFaces(), * pEnd = pFace + meshSouce.GetNumFaces(); pFace != pEnd; ++pFace )
                pFace->AccessMatID() = arrNewMatIDs[ pFace->GetMatID() % arrNewMatIDs.GetCount() ];
            if ( nodeSource.HasSkin() || !nodeBase.HasSkin() )
                continue;
            MIUInt const uVertCount = meshSouce.GetNumVerts();
            mCSkin skinSource;
            mCSkin const & skinBase = *nodeBase.GetSkin();
            mCVertexMatcher Matcher( meshSouce.GetVerts(), nodeBase.GetMesh()->GetVerts(), uVertCount, nodeBase.GetMesh()->GetNumVerts(), a_Options.m_bIndirectVertexMatching );
            MIUInt uWeightCount = 0;
            for ( MIUInt u = uVertCount; u--; uWeightCount += skinBase.GetNumInfluencingBones( Matcher[ u ] ) );
            mTArray< MIUInt > arrVertexIndices( 0, uWeightCount );
            mTArray< MIUInt > arrBoneIndices( 0, uWeightCount );
            mTArray< MIFloat > arrWeights( 0.0f, uWeightCount );
            mTArray< mCUnique::ID > arrBoneIDs;
            for ( MIUInt u = 0, ue = skinBase.GetNumBones(); u != ue; ++u )
                arrBoneIDs.Add( a_sceneSource.GetNodeAt( a_sceneSource.GetNodeIndexByName( sceneBase.GetNodeAt( sceneBase.GetNodeIndexByID( skinBase.GetBoneIDByIndex( u ) ) )->GetName() ) )->GetID() );
            for ( MIUInt u = 0, v = 0; u != uVertCount; ++u )
                for ( MIUInt w = 0, we = skinBase.GetNumInfluencingBones( Matcher[ u ] ); w != we; ++v, ++w )
                    arrVertexIndices[ v ] = u, arrBoneIndices[ v ] = skinBase.GetBoneIndex( Matcher[ u ], w ), arrWeights[ v ] = skinBase.GetWeight( Matcher[ u ], w );
            skinSource.InitSwapping( uVertCount, arrBoneIDs, arrVertexIndices, arrBoneIndices, arrWeights );
            nodeSource.SwapSkin( skinSource );
        }
    }

    streamBaseXmac.Seek( 136 );
    MIUInt const uEndXacOffset = streamBaseXmac.ReadU32() + 140;
    streamBaseXmac.Seek( 146 );
    if ( streamBaseXmac.ReadBool() )
    {
        a_streamDest.SetInvertEndianness( MITrue );
        streamBaseXmac.SetInvertEndianness( MITrue );
    }
    MIUInt uNewEndXacOffset;
    mCBox boxExtents;
    MIUInt uNextSection = 148;
    for ( MIUInt uSyncOffset = 0; uNextSection < uEndXacOffset; )
    {
        streamBaseXmac.Seek( uNextSection );
        MIUInt const uSectionID = streamBaseXmac.ReadU32();
        MIUInt const uSectionSize = streamBaseXmac.ReadU32() + 12;
        uNextSection += uSectionSize;
        streamBaseXmac.Skip( 4 );
        if ( uSectionID == ESection_Materials )
        {
            streamBaseXmac.Skip( 12 );
            for ( MIUInt u = arrBaseMaterials.GetCount(); u--; )
            {
                streamBaseXmac.Skip( 95 );
                MIUInt const uMapCount = streamBaseXmac.ReadU8();
                streamBaseXmac.Skip( streamBaseXmac.ReadU32() );
                for ( MIUInt v = uMapCount; v--; streamBaseXmac.Skip( streamBaseXmac.ReadU32() ) )
                    streamBaseXmac.Skip( 28 );
            }
            uNextSection = streamBaseXmac.Tell();
            if ( !a_Options.m_bReplaceOnlyVertices )
            {
                a_streamDest << ( MIU32 ) ESection_Materials << ( MIU32 ) 12 << ( MIU32 ) 1 << g_32( arrMaterials.GetCount() ) << g_32( arrMaterials.GetCount() ) << ( MIU32 ) 0;
                for ( MIUInt u = 0, ue = arrMaterials.GetCount(); u != ue; ++u )
                {
                    mCMaterial const & Material = *arrMaterials[ u ];
                    a_streamDest << ( MIU32 ) ESection_Material << g_32( 88 + Material.GetName().GetLength() ) << ( MIU32 ) 2;
                    a_streamDest << 0.0f << 0.0f << 0.0f << 1.0f << 0.75f << 0.75f << 0.75f << 1.0f << 1.0f << 1.0f << 1.0f << 1.0f;
                    a_streamDest << 0.0f << 0.0f << 0.0f << 1.0f << 25.0f << 0.0f << 1.0f << 1.5f << ( MIU16 ) 0 << ( MIU8 ) 70;
                    a_streamDest << ( MIU8 ) ( ( Material.GetTexMap( mCMaterial::EMapType_Diffuse ) ? 1 : 0 ) +
                                               ( Material.GetTexMap( mCMaterial::EMapType_Normal ) ? 1 : 0 ) +
                                               ( Material.GetTexMap( mCMaterial::EMapType_Specular ) ? 1 : 0 ) );
                    a_streamDest << g_32( Material.GetName().GetLength() ) << Material.GetName();
                    for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
                    {
                        mCTexMap const * pMap = Material.GetTexMap( i );
                        if ( !pMap )
                            continue;
                        MIU8 u8MapType = 0;
                        for ( MIU8 w = sizeof( arrNativeMapTypes ) / sizeof( arrNativeMapTypes[ 0 ] ); w--; u8MapType = arrNativeMapTypes[ w ] == i ? w : u8MapType );
                        a_streamDest << 1.0f << 0.0f << 0.0f << 1.0f << 1.0f << 0.0f << static_cast< MIU16 >( u ) << u8MapType << ( MIU8 ) 0;
                        mCString strFileName = g_GetFileNameNoExt( pMap->GetTextureFilePath() );
                        a_streamDest << g_32( strFileName.GetLength() ) << strFileName;
                    }
                }
                uSyncOffset = uNextSection;
            }
        }
        else if ( ( uSectionID == ESection_Mesh ) && !a_Options.m_bReplaceOnlyVertices )
        {
            MIUInt uBaseNodeIndex = streamBaseXmac.ReadU32();
            mCNode & nodeSource = *a_sceneSource.AccessNodeAt( a_sceneSource.GetNodeIndexByName( sceneBase.GetNodeAt( uBaseNodeIndex )->GetName() ) );
            if ( nodeSource.HasMesh() )
            {
                streamBaseXmac.Skip( 20 );
                MIU32 const u32Unknown = streamBaseXmac.ReadU32();
                a_streamDest << ( MIU32 ) ESection_Mesh << ( MIU32 ) 0 << ( MIU32 ) 1;
                MIUInt const uSectionBegin = a_streamDest.Tell();
                mCMesh & meshSource = *nodeSource.AccessMesh();
                mCSkin const & skinSource = *nodeSource.GetSkin();
                MIUInt uMaterialIndexCount = arrMaterials.GetCount();
                mTArray< MIBool > arrUsedMaterialIndices( MIFalse, uMaterialIndexCount );
                mTArray< mTArray< MIUInt > > arrBoneIndicesPerMat( mTArray< MIUInt >(), arrMaterials.GetCount() );
                mTArray< mTArray< MIBool > > arrBoneUsagePerMat( mTArray< MIBool >( MIFalse, skinSource.GetNumBones() ), arrBoneIndicesPerMat.GetCount() );
                for ( mCMaxFace * pFace = meshSource.AccessFaces(), * pEnd = pFace + meshSource.GetNumFaces(); pFace != pEnd; arrUsedMaterialIndices[ pFace->GetMatID() ] = MITrue, ++pFace )
                    for ( MIUInt u = 3; u--; )
                        for ( MIUInt v = skinSource.GetNumInfluencingBones( ( *pFace )[ u ] ); v--; )
                            arrBoneUsagePerMat[ pFace->GetMatID() ][ skinSource.GetBoneIndex( ( *pFace )[ u ], v ) ] = MITrue;
                for ( MIUInt u = uMaterialIndexCount; u--; uMaterialIndexCount -= arrUsedMaterialIndices[ u ] ? 0 : 1 );
                for ( MIUInt u = arrBoneUsagePerMat.GetCount(); u--; )
                    for ( MIUInt v = 0, ve = arrBoneUsagePerMat[ u ].GetCount(); v != ve; ++v )
                        if ( arrBoneUsagePerMat[ u ][ v ] )
                            arrBoneIndicesPerMat[ u ].Add( arrBaseNodeIndices[ a_sceneSource.GetNodeIndexByID( skinSource.GetBoneIDByIndex( v ) ) ] );
                meshSource.SortFacesByMatID();
                if ( !meshSource.HasVNFaces() || a_Options.m_bRecalculateVertexNormals )
                {
                    if ( a_Options.m_bUseAnglesInsteadOfSGs )
                        meshSource.CalcVNormalsByAngle( a_Options.m_fMaxAngleInDegrees );
                    else
                        meshSource.CalcVNormalsBySGs();
                }
                if ( meshSource.HasTVFaces() )
                    meshSource.CalcVTangents();
                mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshSource );
                boxExtents |= meshSource.CalcExtents();
                mTArray< mCMesh::SUniVert > arrUVerts;
                mTArray< mCFace > arrUVFaces;
                meshSource.CalcUniVertMesh( arrUVerts, arrUVFaces, MITrue );
                MIUInt const uVertCount = meshSource.GetNumVerts();
                MIUInt const uUVertCount = arrUVerts.GetCount();
                MIUInt const uFaceCount = meshSource.GetNumFaces();
                a_streamDest << g_32( uBaseNodeIndex ) << g_32( uVertCount ) << g_32( uUVertCount ) << g_32( uFaceCount * 3 );
                a_streamDest << g_32( uMaterialIndexCount ) << ( MIU32 ) ( meshSource.HasTVFaces() ? 5 : 3 ) << u32Unknown;
                MIU16 const u16Magic = 0x1002;  // Just any number.
                a_streamDest << ( MIU32 ) EMeshSection_BaseVerts << ( MIU32 ) 4 << ( MIU16 ) 0 << u16Magic;
                for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
                    a_streamDest << g_32( arrUVerts[ u ].m_uBaseVertIndex );
                a_streamDest << ( MIU32 ) EMeshSection_Vertices << ( MIU32 ) 12 << ( MIU16 ) 1 << u16Magic;
                for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
                    a_streamDest << *arrUVerts[ u ].m_pVert;
                a_streamDest << ( MIU32 ) EMeshSection_Normals << ( MIU32 ) 12 << ( MIU16 ) 1 << u16Magic;
                for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
                    a_streamDest << *arrUVerts[ u ].m_pVNormal;
                if ( meshSource.HasTVFaces() )
                {
                    a_streamDest << ( MIU32 ) EMeshSection_TexCoords << ( MIU32 ) 8 << ( MIU16 ) 0 << u16Magic;
                    for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
                        a_streamDest << arrUVerts[ u ].m_pTVert->GetX() << arrUVerts[ u ].m_pTVert->GetY();
                    a_streamDest << ( MIU32 ) EMeshSection_Tangents << ( MIU32 ) 16 << ( MIU16 ) 1 << u16Magic;
                    for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
                        a_streamDest << *arrUVerts[ u ].m_pVTangent << arrUVerts[ u ].m_fVTHandiness;
                }
                for ( MIUInt u = uMaterialIndexCount, uPartFaceCount = 0, uPassedFaceCount = 0, uPartVertCount = 0, uPassedVertCount = 0; u--; uPassedVertCount += uPartVertCount, uPassedFaceCount += uPartFaceCount )
                {
                    MIUInt uMatID = arrUVerts[ uPassedVertCount ].m_uMatID;
                    a_streamDest << ( MIU32 ) 0 << ( MIU32 ) 0 << g_32( uMatID ) << g_32( arrBoneIndicesPerMat[ uMatID ].GetCount() );
                    uPartFaceCount = uPartVertCount = 0;
                    for ( mCFace const * pFaces = arrUVFaces.GetBuffer() + uPassedFaceCount; uPartFaceCount != uFaceCount - uPassedFaceCount; ++uPartFaceCount )
                    {
                        if ( uMatID != arrUVerts[ pFaces[ uPartFaceCount ].GetA() ].m_uMatID )
                            break;
                        for ( MIUInt v = 0; v != 3; ++v )
                        {
                            MIUInt const uUVertIndex = pFaces[ uPartFaceCount ][ v ];
                            if ( arrUVerts[ uUVertIndex ].m_pVert )
                                arrUVerts[ uUVertIndex ].m_pVert = 0, ++uPartVertCount;
                            a_streamDest << g_32( uUVertIndex - uPassedVertCount );
                        }
                    }
                    a_streamDest.Skip( -16 - uPartFaceCount * 12 );
                    a_streamDest << g_32( uPartFaceCount * 3 ) << g_32( uPartVertCount );
                    a_streamDest.Skip( 8 + uPartFaceCount * 12 );
                    for ( MIUInt v = 0, ve = arrBoneIndicesPerMat[ uMatID ].GetCount(); v != ve; ++v )
                        a_streamDest << g_32( arrBoneIndicesPerMat[ uMatID ][ v ] );
                }
                MIUInt uSectionEnd = a_streamDest.Tell();
                a_streamDest.Seek( uSectionBegin - 8 );
                a_streamDest << g_32( uSectionEnd- uSectionBegin );
                a_streamDest.Seek( uSectionEnd );
                uSyncOffset = uNextSection;
            }
        }
        else if ( ( uSectionID == ESection_Mesh ) && a_Options.m_bReplaceOnlyVertices )
        {
            mCNode & nodeSource = *a_sceneSource.AccessNodeAt( a_sceneSource.GetNodeIndexByName( sceneBase.GetNodeAt( streamBaseXmac.ReadU32() )->GetName() ) );
            if ( nodeSource.HasMesh() )
            {
                mCMesh & meshSource = *nodeSource.AccessMesh();
                mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshSource );
                boxExtents |= meshSource.CalcExtents();
                streamBaseXmac.Skip( 4 );
                MIUInt const uUVertCount = streamBaseXmac.ReadU32();
                streamBaseXmac.Skip( 16 );
                mTArray< MIUInt > arrVertIndices( 0, uUVertCount );
                for ( MIUInt uMeshSectionID, uBlockSize; uMeshSectionID = streamBaseXmac.ReadU32(), uBlockSize = streamBaseXmac.ReadU32(); )
                {
                    streamBaseXmac.Skip( 4 );
                    if ( uMeshSectionID == EMeshSection_BaseVerts )
                        for ( MIUInt u = 0; u != uUVertCount; ++u )
                            streamBaseXmac >> g_32( arrVertIndices[ u ] );
                    else if ( uMeshSectionID == EMeshSection_Vertices )
                        break;
                    else
                        streamBaseXmac.Skip( uUVertCount * uBlockSize );
                }
                Sync( a_streamDest, streamBaseXmac, uSyncOffset, streamBaseXmac.Tell() - uSyncOffset );
                mCVec3 const * pVerts = meshSource.GetVerts();
                for ( MIUInt u = 0; u != uUVertCount; ++u )
                    a_streamDest << pVerts[ arrVertIndices[ u ] ];
                uSyncOffset += 12 * uUVertCount;
            }
        }
        else if ( ( uSectionID == ESection_Skin ) && !a_Options.m_bReplaceOnlyVertices )
        {
            MIUInt uBaseNodeIndex = streamBaseXmac.ReadU32();
            mCNode & nodeSource = *a_sceneSource.AccessNodeAt( a_sceneSource.GetNodeIndexByName( sceneBase.GetNodeAt( uBaseNodeIndex )->GetName() ) );
            if ( nodeSource.HasSkin() )
            {
                a_streamDest << ( MIU32 ) ESection_Skin << ( MIU32 ) 0 << ( MIU32 ) 3;
                MIUInt const uSectionBegin = a_streamDest.Tell();
                streamBaseXmac.Skip( 8 );
                MIUInt const uSkinIndex = streamBaseXmac.ReadU32();
                mCSkin & skinSource = *nodeSource.AccessSkin();
                skinSource.LimitNumInfluencingBonesPerVert( 4 );
                mTArray< MIU16 > arrBaseBoneIndices( 0, skinSource.GetNumBones() );
                for ( MIUInt u = skinSource.GetNumBones(); u--; )
                    if ( ( arrBaseBoneIndices[ u ] = static_cast< MIU16 >( arrBaseNodeIndices[ a_sceneSource.GetNodeIndexByID( skinSource.GetBoneIDByIndex( u ) ) ] ) ) == ( MI_DW_INVALID & 0xFFFF ) )
                        return MI_ERROR( mCConverterError, EMiscellaneous, "Skinning includes bone not present in base ._xmac file." ), mEResult_False;
                streamBaseXmac.Skip( 6 );
                MIU16 u16Unknown = streamBaseXmac.ReadU16();
                a_streamDest << g_32( uBaseNodeIndex ) << g_32( a_sceneSource.GetNumNodes() ) << g_32( skinSource.GetNumWeights() ) << g_32( uSkinIndex );
                for ( MIUInt u = 0, ue = skinSource.GetNumWeights(); u != ue; ++u )
                    a_streamDest << skinSource.GetWeight( u ) << arrBaseBoneIndices[ skinSource.GetBoneIndex( u ) ] << u16Unknown;
                for ( MIUInt u = 0, ue = skinSource.GetNumVerts(), v = 0, w = 0; u != ue; ++u, v += w )
                    a_streamDest << g_32( v ) << g_32( w = skinSource.GetNumInfluencingBones( u ) );
                MIUInt uSectionEnd = a_streamDest.Tell();
                a_streamDest.Seek( uSectionBegin - 8 );
                a_streamDest << g_32( uSectionEnd- uSectionBegin );
                a_streamDest.Seek( uSectionEnd );
                uSyncOffset = uNextSection;
            }
        }
        else if ( ( uSectionID == ESection_Morph ) && !a_Options.m_bReplaceOnlyVertices )
        {
            MIUInt const uMorphCount = streamBaseXmac.ReadU32();
            mCNode & nodeBase = *sceneBase.AccessNodeAt( streamBaseXmac.ReadU32() );
            mCNode & nodeSource = *a_sceneSource.AccessNodeAt( a_sceneSource.GetNodeIndexByName( nodeBase.GetName() ) );
            if ( nodeBase.HasMesh() && nodeSource.HasMesh() )
            {
                mTArray< mCMesh::SUniVert > arrUVerts;
                mTArray< mCFace > arrUVFaces;
                nodeSource.GetMesh()->CalcUniVertMesh( arrUVerts, arrUVFaces, MITrue );
                mCMesh & meshBase = *nodeBase.AccessMesh();
                MIUInt const uBaseUVertCount = meshBase.GetNumVNormals();
                mCFace const * pBaseVNFaces = meshBase.GetVNFaces();
                mCMaxFace const * pBaseFaces = meshBase.GetFaces();
                mTArray< MIUInt > arrBaseVertIndices( 0, uBaseUVertCount );
                for ( MIUInt u = 0, ue = meshBase.GetNumFaces(); u != ue; ++u )
                    for ( MIUInt v = 0; v != 3; ++v )
                        arrBaseVertIndices[ pBaseVNFaces[ u ][ v ] ] = pBaseFaces[ u ][ v ];
                mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshBase );
                mCVertexMatcher Matcher( nodeSource.GetMesh()->GetVerts(), meshBase.GetVerts(), nodeSource.GetMesh()->GetNumVerts(), meshBase.GetNumVerts(), MIFalse );
                mTArray< MIUInt > arrBaseMorphVertIndices( 0, meshBase.GetNumVerts() );
                for ( MIUInt u = 0; u != uMorphCount; ++u )
                {
                    g_memset( arrBaseMorphVertIndices.AccessBuffer(), 0xFF, sizeof( MIUInt ) * arrBaseMorphVertIndices.GetCount() );
                    streamBaseXmac.Skip( 24 );
                    streamBaseXmac.Skip( streamBaseXmac.ReadU32() + 12 );
                    Sync( a_streamDest, streamBaseXmac, uSyncOffset, streamBaseXmac.Tell() - uSyncOffset );
                    MIUInt const uBaseMorphVertCount = streamBaseXmac.ReadU32();
                    mCByteArray arrBaseMorphData( 0, uBaseMorphVertCount * 12 );
                    MIByte const * pBaseMorphData1 = arrBaseMorphData.GetBuffer(), * pBaseMorphData2 = pBaseMorphData1 + 6 * uBaseMorphVertCount, * pBaseMorphData3 = pBaseMorphData2 + 3 * uBaseMorphVertCount;
                    streamBaseXmac.Read( arrBaseMorphData.AccessBuffer(), arrBaseMorphData.GetCount() );
                    for ( MIUInt v = 0; v != uBaseMorphVertCount; ++v )
                        arrBaseMorphVertIndices[ arrBaseVertIndices[ streamBaseXmac.ReadU32() ] ] = v;
                    mTArray< MIUInt > arrUsedUVerts;
                    for ( MIUInt v = 0, ve = arrUVerts.GetCount(); v != ve; ++v )
                        if ( arrBaseMorphVertIndices[ Matcher[ arrUVerts[ v ].m_uBaseVertIndex ] ] < uBaseMorphVertCount )
                            arrUsedUVerts.Add( v );
                    a_streamDest << g_32( arrUsedUVerts.GetCount() );
                    for ( MIUInt v = 0, ve = arrUsedUVerts.GetCount(); v != ve; ++v )
                        a_streamDest.Write( pBaseMorphData1 + arrBaseMorphVertIndices[ Matcher[ arrUVerts[ arrUsedUVerts[ v ] ].m_uBaseVertIndex ] ] * 6, 6 );
                    for ( MIUInt v = 0, ve = arrUsedUVerts.GetCount(); v != ve; ++v )
                        a_streamDest.Write( pBaseMorphData2 + arrBaseMorphVertIndices[ Matcher[ arrUVerts[ arrUsedUVerts[ v ] ].m_uBaseVertIndex ] ] * 3, 3 );
                    for ( MIUInt v = 0, ve = arrUsedUVerts.GetCount(); v != ve; ++v )
                        a_streamDest.Write( pBaseMorphData3 + arrBaseMorphVertIndices[ Matcher[ arrUVerts[ arrUsedUVerts[ v ] ].m_uBaseVertIndex ] ] * 3, 3 );
                    for ( MIUInt v = 0, ve = arrUsedUVerts.GetCount(); v != ve; ++v )
                        a_streamDest << g_32( arrUsedUVerts[ v ] );
                    uSyncOffset += uBaseMorphVertCount * ( 12 + sizeof( MIU32 ) ) + sizeof( MIU32 );
                }
            }
        }
        if ( uNextSection >= uEndXacOffset )
            uNextSection = streamBaseXmac.GetSize(), uNewEndXacOffset = uEndXacOffset + ( a_streamDest.Tell() - uSyncOffset );
        Sync( a_streamDest, streamBaseXmac, uSyncOffset, uNextSection - uSyncOffset );
    }
    a_streamDest.SetInvertEndianness( MIFalse );
    a_streamDest.Seek( 20 );
    a_streamDest << g_32( a_streamDest.GetSize() - 136 );
    a_streamDest << g_64( g_time() );
    a_streamDest.Seek( 110 );
    a_streamDest << boxExtents.GetMin() << boxExtents.GetMax();
    a_streamDest.Skip( 2 );
    a_streamDest << g_32( uNewEndXacOffset - 140 );
    return mEResult_Ok;
}
