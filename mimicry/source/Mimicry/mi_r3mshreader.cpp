#include "mi_include_converters.h"

namespace
{   
    mCString ReadString( mCIOStreamBinary & a_streamSource )
    {
        return a_streamSource.ReadString( a_streamSource.ReadU16() );
    }
}

mEResult mCR3mshReader::ReadR3mshFileData(mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options)
{
    a_sceneDest.Clear();
    mCString const strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    if ( a_streamSource.ReadString( 4 ) != "R3RF" )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid .r3msh file." ), mEResult_False;
    MIU32 uResourceHeaderOffset = a_streamSource.ReadU32();
    a_streamSource.Skip(36);
    if (a_streamSource.ReadString(4) != "GMSH" || a_streamSource.ReadU32() != 0x01)
        return MI_ERROR(mCStreamError, EBadFormat, "Invalid .r3msh file."), mEResult_False;
    a_streamSource.ReadU32();

    MIU32 uIndicesSize = a_streamSource.ReadU32();
    if (uIndicesSize % 2 != 0 || (uIndicesSize / 2) % 3 != 0)
        return MI_ERROR(mCStreamError, EBadFormat, "Invalid .r3msh file."), mEResult_False;
    a_streamSource.ReadU32();

    mCMesh meshDest;
    mCNode & nodeDest = a_sceneDest.AddNewNode();
    mCMultiMaterial & matMultiDest = a_sceneDest.AddNewMultiMaterial();
    nodeDest.AccessName() = strSceneName;
    nodeDest.AccessMaterialName() = matMultiDest.AccessName() = "Mtls_" + strSceneName;
   
    MIUInt uFaceCount = uIndicesSize / 6;
    meshDest.SetNumFaces(uFaceCount);
    mCMaxFace * pFace = meshDest.AccessFaces();
    /*for (MIUInt u = 0, ue = arrSubMeshFaceCounts.GetCount(); u != ue; ++u)
        for (MIUInt v = arrSubMeshFaceCounts[u]; v--; (pFace++)->AccessMatID() = u)
            a_streamSource >> pFace->AccessC() >> pFace->AccessB() >> pFace->AccessA();*/
    for (MIUInt u = 0, ue = uFaceCount; u != ue; ++u, (pFace++))
        a_streamSource >> (MIU16 &)pFace->AccessC() >> (MIU16 &)pFace->AccessB() >> (MIU16 &)pFace->AccessA();
    
    MIU32 uVerticesSize = a_streamSource.ReadU32();
    MIU32 uVertexSize = a_streamSource.ReadU32();
    if (uVerticesSize % uVertexSize != 0)
        return MI_ERROR(mCStreamError, EBadFormat, "Invalid .r3msh file."), mEResult_False;
    a_streamSource.ReadU32();

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
    };
    
    MIUInt uVertexElementsCount = a_streamSource.ReadU32();
    for (MIUInt u = 0; u != uVertexElementsCount; ++u)
    {
        SVertexElement Decl;
        a_streamSource.Read( &Decl, sizeof( SVertexElement ) );
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
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid .r3msh file." ), mEResult_False;
    
    MIUInt uVertexCount = uVerticesSize / uVertexSize;
    mCMemoryStream streamVertices;
    streamVertices.Resize( uVerticesSize );
    a_streamSource.Read( streamVertices.AccessBuffer(), uVerticesSize );
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
   
    if ( meshDest.HasTVFaces() )
        meshDest.CopyTVFacesFromFaces();
    if ( meshDest.HasVNFaces() )
        meshDest.CopyVNFacesFromFaces();
    meshDest.WeldVertices();
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
    nodeDest.SwapMesh( meshDest );
    /*if ( mCGenomeMaterial::AccessMaterialLookupHint() )
        mCGenomeMaterial::LoadRisenMaterials( a_sceneDest );*/
    if ( a_Options.m_strTextureFileExtension != "" )
        for ( mCMaterial * pMat = matMultiDest.AccessSubMaterials().AccessBuffer(), * pEnd = pMat + matMultiDest.GetSubMaterials().GetCount(); pMat != pEnd; ( pMat++ )->RemoveEmptyTexMaps() )
            for ( mCMaterial::EMapType i = mCMaterial::EMapType_Diffuse; i != mCMaterial::EMapType_Count; ++i )
                g_ReplaceFileExt( pMat->AccessTexMap( i ).AccessTextureFilePath(), a_Options.m_strTextureFileExtension );
    a_sceneDest.SetName( strSceneName );
    return mEResult_Ok;
}
