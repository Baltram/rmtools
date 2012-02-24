#include "mi_include_converters.h"

mCXactWriter::SOptions::SOptions( void ) :
    m_bReplaceOnlyVertices( MIFalse ),
    m_pBaseXactStream( 0 )
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

    void WriteStreamToStream( mCIOStreamBinary & a_streamDest, mCIOStreamBinary & a_streamSource, MIUInt a_uSourceOffset, MIUInt a_uSize )
    {
        static mCByteArray s_arrBuffer;
        s_arrBuffer.Resize( a_uSize );
        MIUInt const uSourceOffsetOld = a_streamSource.Tell();
        a_streamSource.Seek( a_uSourceOffset );
        a_streamSource.Read( s_arrBuffer.AccessBuffer(), a_uSize );
        a_streamDest.Write( s_arrBuffer.GetBuffer(), a_uSize );
    }

    void WriteMeshSection( mCScene const & a_sceneSource, MIUInt a_uMeshIndex, MIUInt a_uFxaMeshIndex, MIUInt a_uMeshMaterialCount, MIU32 a_u32DW1, MIU32 a_u32DW2, 
                           mCIOStreamBinary & a_streamDest, mCIOStreamBinary & a_streamMeshExtraDataDest, mCXactWriter::SOptions const & a_Options )
    {
        mCString const strMaterialName = a_sceneSource.GetNodeAt( a_uMeshIndex )->GetMaterialName();
        mCMesh meshSource( *a_sceneSource.GetNodeAt( a_uMeshIndex )->GetMesh() );
        meshSource.SortFacesByMatID();
        meshSource.CalcVNormalsByAngle( 180.0f );
        if ( !meshSource.HasTVFaces() )
            meshSource.CalcFakeTexturing();
        meshSource.CalcVTangents();
        mCMaxRisenCoordShifter().ShiftMeshCoords( meshSource );

        mTArray< mCMesh::SUniVert > arrUVerts;
        mTArray< mCFace > arrUVFaces;
        meshSource.CalcUniVertMesh( arrUVerts, arrUVFaces );
        for ( MIUInt u = arrUVerts.GetCount(); u--; arrUVerts[ u ].m_uMatID %= a_uMeshMaterialCount );

        MIUInt const uVertCount = meshSource.GetNumVerts();
        MIUInt const uUVertCount = arrUVerts.GetCount();
        MIUInt const uFaceCount = meshSource.GetNumFaces();
        
        a_streamDest << ( MIU32 ) ESection_Mesh << ( MIU32 ) 0 << ( MIU32 ) 3;
        a_streamDest << g_32( a_uFxaMeshIndex );
        a_streamDest << g_32( uVertCount );
        a_streamDest << g_32( uUVertCount );
        a_streamDest << g_32( uFaceCount * 3 );
        a_streamDest << g_32( a_uMeshMaterialCount );
        a_streamDest << ( MIU32 ) 1;  // Channel count.
        a_streamDest << a_u32DW1;

        mCMesh::SUniVert const * pUniVerts = arrUVerts.GetBuffer();
        mCFace const * pUVFaces = arrUVFaces.GetBuffer();
        MIUInt uFacesPassedTotal = 0;
        MIUInt uVertsPassedTotal = 0;
        for ( MIUInt u = 0; u != a_uMeshMaterialCount; ++u )
        {
            *reinterpret_cast< MIU8 * >( &a_u32DW2 ) = ( MIU8 ) u;
            a_streamDest << a_u32DW2;
            MIUInt uCountsOffset = a_streamDest.Tell();
            a_streamDest << ( MIU32 ) 0;
            a_streamDest << ( MIU32 ) 0;

            MIUInt uVertsPassed = 0;
            MIUInt uVertsPassedMax = uUVertCount - uVertsPassedTotal;
            while ( ( uVertsPassed < uVertsPassedMax ) && ( pUniVerts[ uVertsPassed ].m_uMatID == u ) )
            {
                mCMesh::SUniVert const & UVert = pUniVerts[ uVertsPassed++ ];
                a_streamDest << g_32( UVert.m_uBaseVertIndex );
                a_streamDest << *UVert.m_pVert;
                a_streamDest << *UVert.m_pVNormal;
                a_streamDest << UVert.m_pTVert->GetX();
                a_streamDest << UVert.m_pTVert->GetY();
            }

            MIUInt uFacesPassed = 0;
            MIUInt uFacesPassedMax = uFaceCount - uFacesPassedTotal;
            while ( ( uFacesPassed < uFacesPassedMax ) && ( pUniVerts[ pUVFaces[ uFacesPassed ].GetA() - uVertsPassedTotal ].m_uMatID == u ) )
            {
                mCFace const & UVFace = pUVFaces[ uFacesPassed++ ];
                a_streamDest << g_32( UVFace.GetA() - uVertsPassedTotal );
                a_streamDest << g_32( UVFace.GetB() - uVertsPassedTotal );
                a_streamDest << g_32( UVFace.GetC() - uVertsPassedTotal );
            }

            a_streamDest.Seek( uCountsOffset );
            a_streamDest << g_32( uFacesPassed * 3 );
            a_streamDest << g_32( uVertsPassed );
            a_streamDest.Seek( 0, mEStreamSeekMode_End );

            pUniVerts += uVertsPassed;
            uVertsPassedTotal += uVertsPassed;
            pUVFaces += uFacesPassed;
            uFacesPassedTotal += uFacesPassed;
            if ( uFacesPassedTotal == uFaceCount )
                break;
        }
        
        a_streamDest.Seek( 4 );
        a_streamDest << g_32( a_streamDest.GetSize() - 12 );

        a_streamMeshExtraDataDest << ( MIU64 ) 0x0001000000000001;
        a_streamMeshExtraDataDest << ( MIU64 ) 0x0100000001010000;
        a_streamMeshExtraDataDest << g_32( uUVertCount );
        MIU32 u32ColorDummy = 0xFF000000;
        for ( MIUInt u = uUVertCount; u--; a_streamMeshExtraDataDest << u32ColorDummy );
        for ( MIUInt u = 0; u != uUVertCount; a_streamMeshExtraDataDest << *arrUVerts[ u++ ].m_pVTangent );
    }

    MIBool WriteSkinSection( mCScene const & a_sceneSource, MIUInt a_uMeshIndex, MIUInt a_uFxaMeshIndex, mTArray< mCString > const a_arrFxaNodeNames, mTArray< mCVec3 > const a_arrOriginalVerts, mCByteArray a_arrOriginalSkinSection, mCIOStreamBinary & a_streamDest )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( a_uMeshIndex );
        a_streamDest << ( MIU32 ) ESection_Skin << ( MIU32 ) 0 << ( MIU32 ) 1;
        a_streamDest << g_32( a_uFxaMeshIndex );
        if ( nodeSource.HasSkin() )
        {
            mCSkin const & skinSource = *nodeSource.GetSkin();
            MIUInt const uBoneCount = skinSource.GetNumBones();
            mTArray< MIU16 > arrFxaNodeIndexPerBoneIndex( 666, uBoneCount );
            for ( MIUInt u = uBoneCount; u--; )
            {
                mCNode const * pBone = a_sceneSource.GetNodeAt( a_sceneSource.GetNodeIndexByID( skinSource.GetBoneIDByIndex( u ) ) );
                if ( !pBone )
                    return MIFalse;
                mCString const strBoneName = pBone->GetName();
                for ( MIUInt v = a_arrFxaNodeNames.GetCount(); v--; )
                    if ( strBoneName == a_arrFxaNodeNames[ v ] )
                        arrFxaNodeIndexPerBoneIndex[ u ] = v, v = 0;
                if ( arrFxaNodeIndexPerBoneIndex[ u ] == 666 )
                    return MIFalse;
            }
            MIUInt const uVertCount = skinSource.GetNumVerts();
            for ( MIUInt u = 0; u != uVertCount; ++u )
            {
                MIUInt const uWeightCount = skinSource.GetNumInfluencingBones( u );
                a_streamDest << ( MIU8 ) uWeightCount;
                for ( MIUInt v = 0; v != uWeightCount; ++v )
                {
                    a_streamDest << g_16( arrFxaNodeIndexPerBoneIndex[ skinSource.GetBoneIndex( u, v ) ] );
                    a_streamDest << ( MIU16 ) 0x170C;
                    a_streamDest << skinSource.GetWeight( u, v );
                }
            }
        }
        else
        {
            mCMesh const & meshSource = *nodeSource.GetMesh();
            MIUInt const uVertCount = meshSource.GetNumVerts();
            MIUInt const uOriginalVertCount = a_arrOriginalVerts.GetCount();
            mTArray< MIUInt > arrCorrelationIndices;
            mCVertexMatcher::MatchVerts( meshSource.GetVerts(), a_arrOriginalVerts.GetBuffer(), uVertCount, uOriginalVertCount, arrCorrelationIndices );
            mTArray< MIByte const * > arrVertWeightTableOffsets( a_arrOriginalSkinSection.GetBuffer() + 16, uOriginalVertCount + 1 );
            for ( MIUInt u = 0; u != uOriginalVertCount; ++u )
                arrVertWeightTableOffsets[ u + 1 ] = arrVertWeightTableOffsets[ u ] + ( *arrVertWeightTableOffsets[ u ] * 8 + 1 );
            for ( MIUInt u = 0; u != uVertCount; ++u )
            {
                MIUInt uVertIndex = arrCorrelationIndices[ u ];
                a_streamDest.Write( arrVertWeightTableOffsets[ uVertIndex ], static_cast< MIUInt >( arrVertWeightTableOffsets[ uVertIndex + 1 ] - arrVertWeightTableOffsets[ uVertIndex ] ) );
            }
        }
        a_streamDest.Seek( 4 );
        a_streamDest << g_32( a_streamDest.GetSize() - 12 );
        return MITrue;
    }

    void WriteTextureMapSection( mCTexMap const & a_tmapSource, ETexMapType a_enuType, MIUInt a_uMaterialIndex, mCIOStreamBinary & a_streamDest )
    {
        mCString const strTextureFilePath = g_GetFileNameNoExt( a_tmapSource.GetTextureFilePath() );
        a_streamDest << ( MIU32 ) ESection_TexMap << ( MIU32 ) ( 32 + strTextureFilePath.GetLength() ) << ( MIU32 ) 4;
        a_streamDest << ( MIU8 ) a_enuType << ( MIU8 ) 0xF7;
        a_streamDest << ( MIU16 ) a_uMaterialIndex;
        a_streamDest << 0.3f << 0.0f << 0.0f << 1.0f << 1.0f << 0.0f;
        a_streamDest << g_32( strTextureFilePath.GetLength() ) << strTextureFilePath;
        
    }

    void WriteMaterialSection( mCMaterial const & a_matSource, MIUInt a_uMaterialIndex, mCIOStreamBinary & a_streamDest )
    {
        mCString const strMaterialName = a_matSource.GetName();
        a_streamDest << ( MIU32 ) ESection_Material << ( MIU32 ) ( 76 + strMaterialName.GetLength() ) << ( MIU32 ) 5;
        a_streamDest << 0.0f << 0.0f << 0.0f << 0.75f << 0.75f << 0.75f;
        a_streamDest << 1.0f << 1.0f << 1.0f << 0.0f << 0.0f << 0.0f;
        a_streamDest << 25.0f << 0.0f << 1.0f << 1.5f;
        a_streamDest << ( MIU16 ) 0 << ( MIU16 ) 70;
        a_streamDest << g_32( strMaterialName.GetLength() ) << strMaterialName;
        a_streamDest << 0.0f;
        mCTexMap const * arrMaps[ 3 ];
        ETexMapType arrMapTypes[ 3 ] = { ETexMapType_Diffuse, ETexMapType_Normal, ETexMapType_Specular };
        arrMaps[ 0 ] = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Diffuse );
        arrMaps[ 1 ] = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Normal );
        arrMaps[ 2 ] = a_matSource.GetTextureMapAt( mCMaterial::EMapType_Specular );
        for ( MIUInt u = 0; u != 3; ++u )
            if ( arrMaps[ u ] )
                WriteTextureMapSection( *arrMaps[ u ], arrMapTypes[ u ], a_uMaterialIndex, a_streamDest );
    }
}

mEResult mCXactWriter::WriteXactFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    mCIOStreamBinary & streamBaseXact = *a_Options.m_pBaseXactStream;
    mTArray< mCString > arrFxaNodeNames;
    mTArray< mCVec3 > arrOriginalVerts;
    mCByteArray arrOriginalSkinSection;
    mCMaterial const * pFirstMeshMaterial;
    MIUInt uMeshMaterialCount = 0;
    MIUInt uFxaMeshIndex = 0;
    MIUInt uBeginMaterialsOffset = 0;
    MIUInt uEndMaterialsOffset = 0;
    MIUInt uMeshSectionOffset = 0;
    MIUInt uMeshSectionSize = 0;
    MIUInt uSkinSectionOffset = 0;
    MIUInt uSkinSectionSize = 0;
    MIU32 u32DW1 = 0;
    MIU32 u32DW2 = 0;
    mCMemoryStream streamMaterialSections;
    mCMemoryStream streamMeshSection;
    mCMemoryStream streamMeshExtraData;
    mCMemoryStream streamSkinSection;

    streamBaseXact.Seek( 74 );
    MIUInt uEndFxaOffset = streamBaseXact.ReadU32() + 78;
    if ( streamBaseXact.ReadU32() != 0x20415846 )
    {
        MI_ERROR( mCConverterError, EBadFormat, "Source .xact file is not valid." );
        return mEResult_False;
    }
    MIUInt uNextSection = streamBaseXact.Tell() + 2;
    while ( uNextSection < uEndFxaOffset )
    {
        streamBaseXact.Seek( uNextSection );
        MIUInt uSectionID = streamBaseXact.ReadU32();
        MIUInt uSectionSize = streamBaseXact.ReadU32() + 12;
        uNextSection += uSectionSize;
        if ( uSectionID == ESection_Node )
        {
            streamBaseXact.Skip( 4 + 68 );
            streamBaseXact.Read( arrFxaNodeNames.AddNew(), streamBaseXact.ReadU32() );
        }
        else if ( uSectionID == ESection_Mesh )
        {
            uMeshSectionOffset = uNextSection - uSectionSize;
            uMeshSectionSize = uSectionSize;
            streamBaseXact.Skip( 4 );
            uFxaMeshIndex = streamBaseXact.ReadU32();
            arrOriginalVerts.Resize( streamBaseXact.ReadU32() );
            streamBaseXact.Skip( 8 );
            MIUInt const uMaterialCount = streamBaseXact.ReadU32();
            MIUInt const uChannelCount = streamBaseXact.ReadU32();
            u32DW1 = streamBaseXact.ReadU32();
            mCMaxRisenCoordShifter CoordShifter;
            for ( MIUInt u = uMaterialCount; u--; )
            {
                u32DW2 = streamBaseXact.ReadU32();
                MIUInt const uPartFaceCount = streamBaseXact.ReadU32() / 3;
                MIUInt const uPartUVertCount = streamBaseXact.ReadU32();
                for ( MIUInt v = uPartUVertCount; v--; )
                {
                    MIUInt uVertIndex = streamBaseXact.ReadU32();
                    mCVec3 vecVert;
                    vecVert << streamBaseXact;
                    arrOriginalVerts[ uVertIndex ] = CoordShifter.ShiftVectorCoords( vecVert );
                    streamBaseXact.Skip( 12 + 8 * uChannelCount );
                }
                streamBaseXact.Skip( 12 * uPartFaceCount );
            }
        }
        else if ( uSectionID == ESection_Skin )
        {
            uSkinSectionOffset = uNextSection - uSectionSize;
            uSkinSectionSize = uSectionSize;
            arrOriginalSkinSection.Resize( uSkinSectionSize );
            streamBaseXact.Skip( -8 );
            streamBaseXact.Read( arrOriginalSkinSection.AccessBuffer(), uSkinSectionSize );
        }
        else if ( uSectionID == ESection_Material )
        {
            if ( !uBeginMaterialsOffset )
                uBeginMaterialsOffset = uNextSection - uSectionSize;
            uEndMaterialsOffset = uNextSection;
        }
        else if ( uSectionID == ESection_TexMap )
        {
            uEndMaterialsOffset = uNextSection;
        }
    }

    MIUInt uMeshIndex = 666;
    for ( MIUInt u = a_sceneSource.GetNumNodes(); u--; )
    {
        mCNode const & nodeSource = *a_sceneSource.GetNodeAt( u );
        if ( nodeSource.HasMesh() )
            if ( nodeSource.GetName() == arrFxaNodeNames[ uFxaMeshIndex ] )
                uMeshIndex = u;
    }
    if ( uMeshIndex == 666 )
    {
        MI_ERROR( mCConverterError, EMiscellaneous, ( "Cannot find a same-named replacement for the xact mesh \"" + arrFxaNodeNames[ uFxaMeshIndex ] + "\"." ).GetText() );
        return mEResult_False;
    }
    if ( a_Options.m_bReplaceOnlyVertices )
    {
        mCMesh const & meshSource = *a_sceneSource.GetNodeAt( uMeshIndex )->GetMesh();
        streamBaseXact.Seek( uMeshSectionOffset + 16 );
        MIUInt uVertCount = streamBaseXact.ReadU32();
        MIUInt uUVertCount = streamBaseXact.ReadU32();
        streamBaseXact.Skip( 8 );
        MIUInt uChannelCount = streamBaseXact.ReadU32();
        if ( meshSource.GetNumVerts() != uVertCount )
        {
            MI_ERROR( mCConverterError, EMiscellaneous, ( "Wrong vertex count: \"" + arrFxaNodeNames[ uFxaMeshIndex ] + "\"." ).GetText() );
            return mEResult_False;
        }
        a_streamDest << streamBaseXact;
        mCMaxRisenCoordShifter CoordShifter;
        mCVec3 const * pVerts = meshSource.GetVerts();
        MIUInt uPassedUVertCount = 0;
        streamBaseXact.Skip( 4 );
        while ( uPassedUVertCount != uUVertCount )
        {
            streamBaseXact.Skip( 4 );
            MIUInt uFacePartCount = streamBaseXact.ReadU32() / 3;
            MIUInt uUVertPartCount = streamBaseXact.ReadU32();
            for ( MIUInt u = uUVertPartCount; u--; )
            {
                MIUInt uVertIndex = streamBaseXact.ReadU32();
                a_streamDest.Seek( streamBaseXact.Tell() );
                mCVec3 vecVert = pVerts[ uVertIndex ];
                a_streamDest << CoordShifter.ShiftVectorCoords( vecVert );
                streamBaseXact.Skip( 24 + 8 * uChannelCount );
            }
            uPassedUVertCount += uUVertPartCount;
            streamBaseXact.Skip( uFacePartCount * 12 );
        }
        return mEResult_Ok;
    }
    mCMaterialBase const * pMaterial = a_sceneSource.GetMaterialAt( a_sceneSource.GetMaterialIndexByName( a_sceneSource.GetNodeAt( uMeshIndex )->GetMaterialName() ) );
    if ( !pMaterial )
    {
        MI_ERROR( mCConverterError, EMiscellaneous, ( "The mesh \"" + arrFxaNodeNames[ uFxaMeshIndex ] + "\" has no material." ).GetText() );
        return mEResult_False;
    }
    if ( dynamic_cast< mCMaterial const * >( pMaterial ) )
    {
        pFirstMeshMaterial = dynamic_cast< mCMaterial const * >( pMaterial );
        WriteMaterialSection( *pFirstMeshMaterial, 0, streamMaterialSections );
        uMeshMaterialCount = 1;
    }
    else
    {
        mCMultiMaterial const * pMultiMaterial = dynamic_cast< mCMultiMaterial const * >( pMaterial );
        pFirstMeshMaterial = pMultiMaterial->GetSubMaterials().GetBuffer();
        uMeshMaterialCount = pMultiMaterial->GetSubMaterials().GetCount();
        for ( MIUInt u = 0; u != uMeshMaterialCount; ++u )
            WriteMaterialSection( pFirstMeshMaterial[ u ], u, streamMaterialSections );
    }
    WriteMeshSection( a_sceneSource, uMeshIndex, uFxaMeshIndex, uMeshMaterialCount, u32DW1, u32DW2, streamMeshSection, streamMeshExtraData, a_Options );
    if ( !WriteSkinSection( a_sceneSource, uMeshIndex, uFxaMeshIndex, arrFxaNodeNames, arrOriginalVerts, arrOriginalSkinSection, streamSkinSection ) )
    {
        MI_ERROR( mCConverterError, EMiscellaneous, ( "Skinning of \"" + arrFxaNodeNames[ uFxaMeshIndex ] + "\" relies on a bone that doesn't exist in the .xact file." ).GetText() );
        return mEResult_False;
    }

    MIUInt const uEndMeshSectionOffset = uMeshSectionOffset + uMeshSectionSize;
    MIUInt const uEndSkinSectionOffset = uSkinSectionOffset + uSkinSectionSize;

    WriteStreamToStream( a_streamDest, streamBaseXact, 0, uBeginMaterialsOffset );
    WriteStreamToStream( a_streamDest, streamMaterialSections, 0, streamMaterialSections.GetSize() );
    WriteStreamToStream( a_streamDest, streamBaseXact, uEndMaterialsOffset, ( uMeshSectionOffset - uEndMaterialsOffset ) );
    WriteStreamToStream( a_streamDest, streamMeshSection, 0, streamMeshSection.GetSize() );
    WriteStreamToStream( a_streamDest, streamBaseXact, uEndMeshSectionOffset, ( uSkinSectionOffset - uEndMeshSectionOffset ) );
    WriteStreamToStream( a_streamDest, streamSkinSection, 0, streamSkinSection.GetSize() );
    WriteStreamToStream( a_streamDest, streamBaseXact, uEndSkinSectionOffset, ( uEndFxaOffset - uEndSkinSectionOffset ) );
    WriteStreamToStream( a_streamDest, streamMeshExtraData, 0, streamMeshExtraData.GetSize() );
    
    MIUInt const uFxaSize = a_streamDest.GetSize() - streamMeshExtraData.GetSize() - 78;
    a_streamDest << ( MIU32 ) 0xDEADBEEF << ( MIU8 ) 0;
    a_streamDest.Seek( 10 );
    a_streamDest << g_32( a_streamDest.GetSize() - 5 );
    a_streamDest.Seek( 32 );
    a_streamDest << g_32( uFxaSize );
    a_streamDest.Seek( 74 );
    a_streamDest << g_32( uFxaSize );

    return mEResult_Ok;
}