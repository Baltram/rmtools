#include "mi_include_converters.h"

namespace
{   
    mCString ReadString( mCIOStreamBinary & a_streamSource )
    {
        return a_streamSource.ReadString( a_streamSource.ReadU16() );
    }
}

mEResult mCXmshReader::ReadXmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options )
{
    a_sceneDest.Clear();
    mCString const strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    if ( a_streamSource.GetSize() < 8 || a_streamSource.ReadString( 8 ) != "GR01MS02" )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xmsh file." ), mEResult_False;
    a_streamSource.Seek( a_streamSource.ReadU32() );
    if ( ( a_streamSource.ReadU16() != 1 ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( a_streamSource.ReadU16() != 1 ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( ReadString( a_streamSource ) != "eCMeshResource2" ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( a_streamSource.ReadU16() != 0 ) )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xmsh file." ), mEResult_False;
    mCMesh meshDest;
    mCNode & nodeDest = a_sceneDest.AddNewNode();
    mCMultiMaterial & matMultiDest = a_sceneDest.AddNewMultiMaterial();
    nodeDest.AccessName() = strSceneName;
    nodeDest.AccessMaterialName() = matMultiDest.AccessName() = "Mtls_" + strSceneName;
    MIUInt uFaceCount = 0;
    mTArray< MIUInt > arrSubMeshFaceCounts;
    a_streamSource.Skip( 10 );
    for ( MIUInt u = a_streamSource.ReadU32(); u--; )
    {
        mCString strPropertyName = ReadString( a_streamSource );
        ReadString( a_streamSource );
        a_streamSource.Skip( 2 );
        if ( strPropertyName != "SubMeshes" )
        {
            a_streamSource.Skip( a_streamSource.ReadU32() );
            continue;
        }
        a_streamSource.Skip( 5 );
        for ( MIUInt v = a_streamSource.ReadU32(); v--; )
        {
            a_streamSource.Skip( 2 );
            MIUInt uEndSubMeshOffset = a_streamSource.ReadU32();
            uEndSubMeshOffset += a_streamSource.Tell();
            a_streamSource.Skip( 2 );
            for ( MIUInt w = a_streamSource.ReadU32(); w--; )
            {
                strPropertyName = ReadString( a_streamSource );
                ReadString( a_streamSource );
                a_streamSource.Skip( 2 );
                MIUInt const uPropertySize = a_streamSource.ReadU32();
                if ( strPropertyName == "MaterialName" )
                    matMultiDest.AccessSubMaterials().AddNew().AccessName() = g_GetFileNameNoExt( ReadString( a_streamSource ) );
                else if ( strPropertyName == "IndexCount" )
                    uFaceCount += ( arrSubMeshFaceCounts.AddNew() = a_streamSource.ReadU32() / 3 );
                else
                    a_streamSource.Skip( uPropertySize );
            }
            a_streamSource.Seek( uEndSubMeshOffset );
        }
    }
    a_streamSource.Skip( 2 );
    MIUInt uPositionOffset = MI_DW_INVALID;
    MIUInt uNormalOffset = MI_DW_INVALID;
    MIUInt uTexCoordOffset = MI_DW_INVALID;
    MIUInt uColor1Offset = MI_DW_INVALID;
    MIUInt uColor2Offset = MI_DW_INVALID;
    struct SVertexElement  // D3DVERTEXELEMENT9
    {
        MIU16 m_u16StreamIndex;
        MIU16 m_u16Offset;
        MIU8  m_u8Type;
        MIU8  m_u8Method;
        MIU8  m_u8Usage;
        MIU8  m_u8UsageIndex;
    } arrVertexBufferDeclarations[ 16 ];
    a_streamSource.Read( arrVertexBufferDeclarations, sizeof( arrVertexBufferDeclarations ) );
    for ( MIUInt u = 0; u != 16 && arrVertexBufferDeclarations[ u ].m_u8Type != 17; ++u )
    {
        SVertexElement & Decl = arrVertexBufferDeclarations[ u ];
        if ( Decl.m_u16StreamIndex != 0 ||
             Decl.m_u8Method != 0 )
            continue;
        else if ( Decl.m_u8Usage == 00 && Decl.m_u8Type == 2 )
            uPositionOffset = Decl.m_u16Offset;
        else if ( Decl.m_u8Usage == 03 && Decl.m_u8Type == 2 )
            uNormalOffset = Decl.m_u16Offset;
        else if ( Decl.m_u8Usage == 05 && Decl.m_u8Type == 1 && Decl.m_u8UsageIndex == 0 )
            uTexCoordOffset = Decl.m_u16Offset;
        else if ( Decl.m_u8Usage == 10 && Decl.m_u8Type == 4 && Decl.m_u8UsageIndex == 0 )
            uColor1Offset = Decl.m_u16Offset;
        else if ( Decl.m_u8Usage == 10 && Decl.m_u8Type == 4 && Decl.m_u8UsageIndex == 1 )
            uColor2Offset = Decl.m_u16Offset;
    }
    if ( uPositionOffset == MI_DW_INVALID )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xmsh file." ), mEResult_False;
    MIUInt const uVertexStreamSize = a_streamSource.ReadU32();
    a_streamSource.Skip( 12 );
    MIUInt const uVertexSize = a_streamSource.ReadU32();
    MIUInt const uVertexCount = uVertexStreamSize / uVertexSize;
    a_streamSource.Skip( 56 );
    mCMemoryStream streamVertices;
    streamVertices.Resize( uVertexStreamSize );
    a_streamSource.Read( streamVertices.AccessBuffer(), uVertexStreamSize );
    if ( uPositionOffset != MI_DW_INVALID )
    {
        streamVertices.Seek( uPositionOffset );
        meshDest.SetNumVerts( uVertexCount );
        for ( mCVec3 * pVert = meshDest.AccessVerts(), * pEnd = pVert + uVertexCount; pVert != pEnd; ++pVert, streamVertices.Skip( uVertexSize - sizeof( *pVert ) ) )
            streamVertices >> *pVert;
    }
    if ( uNormalOffset != MI_DW_INVALID )
    {
        streamVertices.Seek( uNormalOffset );
        meshDest.SetNumVNormals( uVertexCount );
        for ( mCVec3 * pVNormal = meshDest.AccessVNormals(), * pEnd = pVNormal + uVertexCount; pVNormal != pEnd; ++pVNormal, streamVertices.Skip( uVertexSize - sizeof( *pVNormal ) ) )
            streamVertices >> *pVNormal;
    }
    if ( uTexCoordOffset != MI_DW_INVALID )
    {
        streamVertices.Seek( uTexCoordOffset );
        meshDest.SetNumTVerts( uVertexCount );
        for ( mCVec3 * pTVert = meshDest.AccessTVerts(), * pEnd = pTVert + uVertexCount; pTVert != pEnd; ++pTVert, streamVertices.Skip( uVertexSize - sizeof( MIFloat ) * 2 ) )
            streamVertices >> pTVert->AccessX() >> pTVert->AccessY();
    }
    if ( uColor1Offset != MI_DW_INVALID )
    {
        streamVertices.Seek( uColor1Offset + 3 );
        meshDest.SetHasVertexColors( MITrue );
        for ( mCColor * pVColor = meshDest.AccessVertexColors(), * pEnd = pVColor + uVertexCount; pVColor != pEnd; ++pVColor, streamVertices.Skip( uVertexSize - sizeof( MIU8 ) ) )
            pVColor->AccessRed() = pVColor->AccessGreen() = pVColor->AccessBlue() = streamVertices.ReadU8();
    }
    if ( uColor2Offset != MI_DW_INVALID )
    {
        streamVertices.Seek( uColor2Offset + 3 );
        meshDest.SetHasVertexColors( MITrue );
        for ( mCColor * pVColor = meshDest.AccessVertexColors(), * pEnd = pVColor + uVertexCount; pVColor != pEnd; ++pVColor, streamVertices.Skip( uVertexSize - sizeof( MIU8 ) ) )
            streamVertices >> g_08( pVColor->AccessAlpha() );
    }
    meshDest.SetNumFaces( uFaceCount );
    mCMaxFace * pFace = meshDest.AccessFaces();
    for ( MIUInt u = 0, ue = arrSubMeshFaceCounts.GetCount(); u != ue; ++u )
        for ( MIUInt v = arrSubMeshFaceCounts[ u ]; v--; ( pFace++ )->AccessMatID() = u )
            a_streamSource >> pFace->AccessC() >> pFace->AccessB() >> pFace->AccessA();
    if ( meshDest.HasTVFaces() )
        meshDest.CopyTVFacesFromFaces();
    if ( meshDest.HasVNFaces() )
        meshDest.CopyVNFacesFromFaces();
    meshDest.WeldVertices();
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
    nodeDest.SwapMesh( meshDest );
    if ( mCGenomeMaterial::AccessMaterialLookupHint() )
        mCGenomeMaterial::LoadRisenMaterials( a_sceneDest );
    if ( a_Options.m_strTextureFileExtension != "" )
        for ( mCMaterial * pMat = matMultiDest.AccessSubMaterials().AccessBuffer(), * pEnd = pMat + matMultiDest.GetSubMaterials().GetCount(); pMat != pEnd; ( pMat++ )->RemoveEmptyTexMaps() )
            for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
                g_ReplaceFileExt( pMat->AccessTexMap( i ).AccessTextureFilePath(), a_Options.m_strTextureFileExtension );
    a_sceneDest.SetName( strSceneName );
    return mEResult_Ok;
}
