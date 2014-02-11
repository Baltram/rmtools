#include "mi_include_converters.h"

namespace
{
    enum EVertexStreamArrayType
    {
        EVertexStreamArrayType_Indices = 0,
        EVertexStreamArrayType_Positions = 1,
        EVertexStreamArrayType_Normals = 3,
        EVertexStreamArrayType_DiffuseColor = 4,
        EVertexStreamArrayType_SpecularColor = 5,
        EVertexStreamArrayType_UVs = 12,
        EVertexStreamArrayType_Tangents = 64
    };

    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mEResult mCXcmshWriter::WriteXcmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, mCXcmshWriter::SOptions a_Options )
{
    a_sceneSource.MakeOneMeshScene();
    mCNode * pSourceNode = a_sceneSource.AccessNodeAt( 0 );
    mTArray< mCString > arrMaterialNames;
    MIUInt uMaterialIndex = a_sceneSource.GetMaterialIndexByName( pSourceNode->GetMaterialName() );
    mCMultiMaterial const * pMultiMaterial = uMaterialIndex == MI_DW_INVALID ? 0 : dynamic_cast< mCMultiMaterial const * >( a_sceneSource.GetMaterialAt( uMaterialIndex ) );
    if ( !pMultiMaterial )
        arrMaterialNames.Add( pSourceNode->GetMaterialName() );
    else for ( MIUInt u = 0, ue = pMultiMaterial->GetSubMaterials().GetCount(); u != ue; ++u )
        arrMaterialNames.Add( pMultiMaterial->GetSubMaterials()[ u ].GetName() );
    mCMesh & meshSource = *pSourceNode->AccessMesh();
    meshSource.SortFacesByMatID();
    mCMaxFace * pFaces = meshSource.AccessFaces();
    MIUInt uMatCountMax = arrMaterialNames.GetCount(), uMatCount = 0;
    for ( MIUInt u = 0, uMatID = MI_DW_INVALID, ue = meshSource.GetNumFaces(); u != ue; ++u )
        if ( ( pFaces[ u ].AccessMatID() %= uMatCountMax ) != uMatID )
            uMatID = pFaces[ u ].GetMatID(), ++uMatCount;
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshSource );
    if ( !meshSource.HasVNFaces() || a_Options.m_bRecalculateVertexNormals )
    {
        if ( a_Options.m_bUseAnglesInsteadOfSGs )
            meshSource.CalcVNormalsByAngle( a_Options.m_fMaxAngleInDegrees );
        else
            meshSource.CalcVNormalsBySGs();
    }
    if ( meshSource.HasTVFaces() )
        meshSource.CalcVTangents();
    mCBox boxBoundary = meshSource.CalcExtents();
    mTArray< mCMesh::SUniVert > arrUVerts;
    mTArray< mCFace > arrUVFaces;
    meshSource.CalcUniVertMesh( arrUVerts, arrUVFaces, MIFalse );
    MIU16 const u16PropertySystemVersion = 83, u16PropertyVersion = 30, u16ResourceMeshVersion = 35;
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( a_streamDest, "eCResourceMeshComplex_PS" );
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 0 );
    a_streamDest << u16PropertySystemVersion  << u16PropertySystemVersion;
    MIUInt const uResourceMeshSizeOffset = a_streamDest.Tell();
    a_streamDest << ( MIU32 )( 0 ) << u16PropertyVersion << ( MIU32 )( 2 );
    WriteString( a_streamDest, "BoundingBox" );
    WriteString( a_streamDest, "bCBox" );
    a_streamDest << u16PropertyVersion << 24 << boxBoundary.GetMin() << boxBoundary.GetMax();
    WriteString( a_streamDest, "ResourcePriority" );
    WriteString( a_streamDest, "float" );
    a_streamDest << u16PropertyVersion << sizeof( MIFloat ) << 0.0f;
    a_streamDest << u16ResourceMeshVersion << ( MIU16 )( 1 ) << ( MIU16 )( 1 ) << 0.0f;
    a_streamDest << g_32( uMatCount );
    MIBool bHasTVerts = meshSource.HasVTFaces(), bHasVTangents = meshSource.HasVTFaces(), bHasColorVerts = meshSource.HasVertexColors() && !a_Options.m_bDropVertexColors;
    MIU32 const u32FVF = 0x2 | 0x10 | ( bHasColorVerts ? 0x40 | 0x80 : 0 ) | ( bHasTVerts ? 0x100 : 0 );
    mCMesh::SUniVert const * pUniVerts = arrUVerts.GetBuffer(), * pUniVertsEnd = pUniVerts + arrUVerts.GetCount();
    mCFace const * pUVFaces = arrUVFaces.GetBuffer(), * pUVFacesEnd = pUVFaces + arrUVFaces.GetCount();
    for ( MIUInt uMatID, uPartVertCount, uPartFaceCount, uPassedVertCount = 0; pUniVerts != pUniVertsEnd; uPassedVertCount += uPartVertCount, pUniVerts += uPartVertCount, pUVFaces += uPartFaceCount )
    {
        uMatID = pUniVerts->m_uMatID;
        uPartVertCount = uPartFaceCount = 0;
        boxBoundary.Invalidate();
        for ( MIUInt uVertCountMax = static_cast< MIUInt >( pUniVertsEnd - pUniVerts ); uPartVertCount != uVertCountMax && pUniVerts[ uPartVertCount ].m_uMatID == uMatID; ++uPartVertCount )
            boxBoundary |= *pUniVerts[ uPartVertCount ].m_pVert;
        for ( MIUInt uFaceCountMax = static_cast< MIUInt >( pUVFacesEnd - pUVFaces ); uPartFaceCount != uFaceCountMax && pUniVerts[ pUVFaces[ uPartFaceCount ][ 0 ] - uPassedVertCount ].m_uMatID == uMatID; ++uPartFaceCount );
        MIUInt const uVertexStreamCount = 3 + ( bHasColorVerts ? 2 : 0 ) + ( bHasTVerts ? 1 : 0 ) + ( bHasVTangents ? 1 : 0 );
        MIUInt const uMeshElementSize = 124 + uVertexStreamCount * 20 + uPartFaceCount * 12 + uPartVertCount * ( 12 + 12 + ( bHasColorVerts ? 4 + 4 : 0 ) + ( bHasTVerts ? 8 : 0 ) + ( bHasVTangents ? 12 : 0 ) );
        a_streamDest << ( MIU16 )( 1 ) << u32FVF << boxBoundary.GetMin() << boxBoundary.GetMax() << g_32( uMeshElementSize );
        WriteString( a_streamDest, g_GetFileNameNoExt( arrMaterialNames[ uMatID ] ) + ".xshmat" );
        a_streamDest << g_32( uVertexStreamCount );
        a_streamDest << g_32( EVertexStreamArrayType_Indices ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartFaceCount * 3 );
        for ( MIUInt u = 0; u != uPartFaceCount; ++u )
            for ( MIUInt v = 3; v--; )
                a_streamDest << g_32( pUVFaces[ u ][ v ] - uPassedVertCount );
        a_streamDest << g_32( EVertexStreamArrayType_Positions ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
        for ( MIUInt u = 0; u != uPartVertCount; ++u )
            a_streamDest << *pUniVerts[ u ].m_pVert;
        a_streamDest << g_32( EVertexStreamArrayType_Normals ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
        for ( MIUInt u = 0; u != uPartVertCount; ++u )
            a_streamDest << *pUniVerts[ u ].m_pVNormal;
        if ( bHasColorVerts )
        {
            a_streamDest << g_32( EVertexStreamArrayType_SpecularColor ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
            for ( MIUInt u = 0; u != uPartVertCount; ++u )
                a_streamDest << ( MIU8 )( 0 ) << ( MIU8 )( 0 ) << ( MIU8 )( 0 ) << g_08( pUniVerts[ u ].m_pVColor->GetAlpha() );
            a_streamDest << g_32( EVertexStreamArrayType_DiffuseColor ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
            for ( MIUInt u = 0; u != uPartVertCount; ++u )
                a_streamDest << ( MIU8 )( 255 ) << ( MIU8 )( 255 ) << ( MIU8 )( 255 ) << ( MIU8 )( ( 0L + pUniVerts[ u ].m_pVColor->GetRed() + pUniVerts[ u ].m_pVColor->GetGreen() + pUniVerts[ u ].m_pVColor->GetBlue() ) / 3 );
        }
        if ( bHasTVerts )
        {
            a_streamDest << g_32( EVertexStreamArrayType_UVs ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
            for ( MIUInt u = 0; u != uPartVertCount; ++u )
                a_streamDest << pUniVerts[ u ].m_pTVert->GetX() << pUniVerts[ u ].m_pTVert->GetY();
        }
        if ( bHasVTangents )
        {
            a_streamDest << g_32( EVertexStreamArrayType_Tangents ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << g_32( uPartVertCount );
            for ( MIUInt u = 0; u != uPartVertCount; ++u )
                a_streamDest << ( *pUniVerts[ u ].m_pVTangent * pUniVerts[ u ].m_fVTHandiness );
        }
    }
    MIUInt const uOffsetEnd = a_streamDest.Tell();
    a_streamDest.Seek( uResourceMeshSizeOffset );
    a_streamDest << g_32( uOffsetEnd - uResourceMeshSizeOffset - 4 );
    a_streamDest.Seek( uOffsetEnd );
    return mEResult_Ok;
}
