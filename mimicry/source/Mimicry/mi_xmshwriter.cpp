#include "mi_include_converters.h"

namespace
{
    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mEResult mCXmshWriter::WriteXmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, mCXmshWriter::SOptions a_Options )
{
    a_sceneSource.MakeOneMeshScene();
    mCNode * pSourceNode = a_sceneSource.AccessNodeAt( 0 );
    mTArray< mCString > arrMaterialNames;
    mTArray< MIUInt > arrUsedMaterialIndices;
    mTArray< MIUInt > arrFirstIndexPerMaterial;
    mTArray< MIUInt > arrFirstVertexPerMaterial;
    MIUInt uMaterialIndex = a_sceneSource.GetMaterialIndexByName( pSourceNode->GetMaterialName() );
    mCMultiMaterial const * pMultiMaterial = uMaterialIndex == MI_DW_INVALID ? 0 : dynamic_cast< mCMultiMaterial const * >( a_sceneSource.GetMaterialAt( uMaterialIndex ) );
    if ( !pMultiMaterial )
        arrMaterialNames.Add( pSourceNode->GetMaterialName() + "._xmat" );
    else for ( MIUInt u = 0, ue = pMultiMaterial->GetSubMaterials().GetCount(); u != ue; ++u )
        arrMaterialNames.Add( pMultiMaterial->GetSubMaterials()[ u ].GetName() + "._xmat" );
    mCMesh & meshSource = *pSourceNode->AccessMesh();
    mCVec3 * pVerts = meshSource.AccessVerts();
    for ( MIUInt u = meshSource.GetNumVerts(); u--; )
        for ( MIUInt j = 3; j--; )
            pVerts[ u ][ j ] = static_cast< MIInt >( pVerts[ u ][ j ] * 22.2f + 0.5f ) / 22.2f;
    meshSource.WeldVertices();
    meshSource.SortFacesByMatID();
    mCMaxFace * pFaces = meshSource.AccessFaces();
    MIUInt uMatCountMax = arrMaterialNames.GetCount(), uMatCount = 0;
    for ( MIUInt u = 0, uMatID = MI_DW_INVALID, ue = meshSource.GetNumFaces(); u != ue; ++u )
        if ( ( pFaces[ u ].AccessMatID() %= uMatCountMax ) != uMatID )
            uMatID = pFaces[ u ].GetMatID(), ++uMatCount, arrFirstIndexPerMaterial.Add( u * 3 );
    arrFirstIndexPerMaterial.Add( meshSource.GetNumFaces() * 3 );
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshSource );
    if ( !meshSource.HasVNFaces() || a_Options.m_bRecalculateVertexNormals )
    {
        if ( a_Options.m_bUseAnglesInsteadOfSGs )
            meshSource.CalcVNormalsByAngle( a_Options.m_fMaxAngleInDegrees );
        else
            meshSource.CalcVNormalsBySGs();
    }
    if ( !meshSource.HasTVFaces() )
        meshSource.CalcFakeTexturing();
    meshSource.CalcVTangents();
    if ( a_Options.m_bDropVertexColors )
        meshSource.SetHasVertexColors( MIFalse );
    if ( !meshSource.HasVertexColors() )
        meshSource.SetHasVertexColors( MITrue );
    mTArray< mCMesh::SUniVert > arrUVerts;
    mTArray< mCFace > arrUVFaces;
    meshSource.CalcUniVertMesh( arrUVerts, arrUVFaces, MIFalse );
    mCBox const boxBoundary = meshSource.CalcExtents();
    mTArray< mCBox > arrSubMeshBoundaries;
    mCMesh::SUniVert const * pUniVert = arrUVerts.GetBuffer(), * pEnd = pUniVert + arrUVerts.GetCount();
    for ( MIUInt uMatID = MI_DW_INVALID; pUniVert != pEnd; ++pUniVert )
    {
        if ( uMatID != pUniVert->m_uMatID )
        {
            uMatID = pUniVert->m_uMatID;
            arrSubMeshBoundaries.AddNew().Invalidate();
            arrUsedMaterialIndices.Add( uMatID );
            arrFirstVertexPerMaterial.Add( static_cast< MIUInt >( pUniVert - arrUVerts.GetBuffer() ) );
        }
        arrSubMeshBoundaries.Back() |= *pUniVert->m_pVert;
    }
    arrFirstVertexPerMaterial.Add( arrUVerts.GetCount() );
    MIU16 const u16PropertySystemVersion = 201, u16PropertyVersion = 30;
    a_streamDest << "GR01MS02" << ( MIU32 ) 40 << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 0;
    a_streamDest << g_time() << ( MIU64 ) 0;
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( a_streamDest, "eCMeshResource2" );
    a_streamDest << ( MIU8 )( 1 ) << ( MIU16 )( 0 );
    a_streamDest << u16PropertySystemVersion << u16PropertySystemVersion;
    a_streamDest << ( MIU32 ) 0;  // @70
    a_streamDest << u16PropertySystemVersion << ( MIU32 ) 3;
    WriteString( a_streamDest, "SubMeshes" );
    WriteString( a_streamDest, "bTObjArray<class eCSubMesh>" );
    a_streamDest << u16PropertyVersion << ( MIU32 ) 0;
    MIUInt const uSubMeshArraySizeOffset = a_streamDest.Tell();//204+nm
    a_streamDest << ( MIU8 ) 1 << ( MIU32 ) uMatCount;
    for ( MIUInt u = 0; u != uMatCount; ++u )
    {
        mCString const & strMaterialName = arrMaterialNames[ arrUsedMaterialIndices[ u ] ];
        a_streamDest << u16PropertySystemVersion << ( MIU32 )( 236 + strMaterialName.GetLength() );
        a_streamDest << u16PropertySystemVersion << ( MIU32 ) 6;
        WriteString( a_streamDest, "MaterialName" );
        WriteString( a_streamDest, "bCString" );
        a_streamDest << u16PropertyVersion << ( MIU32 ) strMaterialName.GetLength() + sizeof( MIU16 );
        WriteString( a_streamDest, strMaterialName );
        WriteString( a_streamDest, "Extends" );
        WriteString( a_streamDest, "bCBox" );
        a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( mCBox ) ) << arrSubMeshBoundaries[ u ].GetMin() << arrSubMeshBoundaries[ u ].GetMax();
        WriteString( a_streamDest, "FirstIndex" );
        WriteString( a_streamDest, "long" );
        a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( MIU32 ) ) << ( MIU32 )( arrFirstIndexPerMaterial[ u ] );
        WriteString( a_streamDest, "IndexCount" );
        WriteString( a_streamDest, "long" );
        a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( MIU32 ) ) << arrFirstIndexPerMaterial[ u + 1 ] - arrFirstIndexPerMaterial[ u ];
        WriteString( a_streamDest, "FirstVertex" );
        WriteString( a_streamDest, "long" );
        a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( MIU32 ) ) << ( MIU32 )( arrFirstVertexPerMaterial[ u ] );
        WriteString( a_streamDest, "VertexCount" );
        WriteString( a_streamDest, "long" );
        a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( MIU32 ) ) << arrFirstVertexPerMaterial[ u + 1 ] - arrFirstVertexPerMaterial[ u ];
        a_streamDest << u16PropertySystemVersion << ( MIU32 ) 1;
        a_streamDest << arrSubMeshBoundaries[ u ].GetMin() << arrSubMeshBoundaries[ u ].GetMax() << ( MIU32 ) 0 << ( MIU32 ) 1;
    }
    MIUInt uOffset = a_streamDest.Tell();
    a_streamDest.Seek( uSubMeshArraySizeOffset - 4 );
    a_streamDest << ( MIU32 )( uOffset - uSubMeshArraySizeOffset );
    a_streamDest.Seek( uOffset );
    WriteString( a_streamDest, "LightmapName" );
    WriteString( a_streamDest, "bCString" );
    a_streamDest << u16PropertyVersion << ( MIU32 ) 2 << ( MIU16 )( 0 );
    WriteString( a_streamDest, "Boundary" );
    WriteString( a_streamDest, "bCBox" );
    a_streamDest << u16PropertyVersion << ( MIU32 )( sizeof( mCBox ) ) << boxBoundary.GetMin() << boxBoundary.GetMax();
    a_streamDest << u16PropertySystemVersion;
    MIUInt const uDataOffset = a_streamDest.Tell();
    // For better info see https://forum.worldofplayers.de/forum/threads/717908?p=12013400&viewfull=1#post12013400
    // D3DVERTEXELEMENT9   Stream    Offset          Type           Method        Usage          UsageIndex
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 00 << ( MIU8 ) 02 << ( MIU8 ) 0 << ( MIU8 ) 00 << ( MIU8 ) 0;
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 12 << ( MIU8 ) 02 << ( MIU8 ) 0 << ( MIU8 ) 03 << ( MIU8 ) 0;
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 24 << ( MIU8 ) 02 << ( MIU8 ) 0 << ( MIU8 ) 06 << ( MIU8 ) 0;
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 36 << ( MIU8 ) 04 << ( MIU8 ) 0 << ( MIU8 ) 10 << ( MIU8 ) 0;
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 40 << ( MIU8 ) 04 << ( MIU8 ) 0 << ( MIU8 ) 10 << ( MIU8 ) 1;
    a_streamDest << ( MIU16 ) 000 << ( MIU16 ) 44 << ( MIU8 ) 01 << ( MIU8 ) 0 << ( MIU8 ) 05 << ( MIU8 ) 0;
    a_streamDest << ( MIU16 ) 003 << ( MIU16 ) 00 << ( MIU8 ) 00 << ( MIU8 ) 0 << ( MIU8 ) 05 << ( MIU8 ) 4;
    a_streamDest << ( MIU16 ) 255 << ( MIU16 ) 00 << ( MIU8 ) 17 << ( MIU8 ) 0 << ( MIU8 ) 00 << ( MIU8 ) 0;  // D3DDECL_END
    a_streamDest << ( MIU16 ) 255 << ( MIU16 ) 00 << ( MIU8 ) 17 << ( MIU8 ) 0 << ( MIU8 ) 00 << ( MIU8 ) 0;  // D3DDECL_END
    a_streamDest << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0;
    a_streamDest << ( MIU32 )( arrUVerts.GetCount() * 52 ) << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 1 << ( MIU32 ) 52;
    a_streamDest << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0;
    a_streamDest << ( MIU32 )( arrUVFaces.GetCount() * 12 ) << ( MIU32 ) 0 << ( MIU32 ) 102 << ( MIU32 ) 1;
    for ( MIUInt u = 0, ue = arrUVerts.GetCount(); u != ue; ++u )
    {
        mCMesh::SUniVert const & UniVert = arrUVerts[ u ];
        a_streamDest << *UniVert.m_pVert << *UniVert.m_pVNormal << *UniVert.m_pVTangent;
        a_streamDest << ( MIU8 ) 255 << ( MIU8 ) 255 << ( MIU8 )( UniVert.m_fVTHandiness < 0.0f ? 0 : 255 ) << ( MIU8 )( ( UniVert.m_pVColor->GetRed() + UniVert.m_pVColor->GetGreen() + UniVert.m_pVColor->GetBlue() ) / 3 );
        a_streamDest << ( MIU8 ) 0 << ( MIU8 ) 0 << ( MIU8 ) 0 << g_08( UniVert.m_pVColor->GetAlpha() );
        a_streamDest << UniVert.m_pTVert->GetX() << UniVert.m_pTVert->GetY();
    }
    for ( mCFace const * pFace = arrUVFaces.GetBuffer(), * pEnd = pFace + arrUVFaces.GetCount(); pFace != pEnd; ++pFace )
        a_streamDest << g_32( pFace->GetC() ) << g_32( pFace->GetB() ) << g_32( pFace->GetA() );
    a_streamDest << ( MIBool )( !pSourceNode->GetName().Contains( "noinstancing" ) );
    MIUInt const uOffsetEnd = a_streamDest.Tell();
    a_streamDest.Seek( 12 );
    a_streamDest << g_32( uDataOffset - 40 ) << g_32( uDataOffset ) << g_32( uOffsetEnd - uDataOffset );
    a_streamDest.Seek( 70 );
    a_streamDest << g_32( uDataOffset - 74 );
    a_streamDest.Seek( uOffsetEnd );
    return mEResult_Ok;
}
