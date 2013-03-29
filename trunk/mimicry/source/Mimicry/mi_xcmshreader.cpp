#include "mi_include_converters.h"

namespace
{
    enum EVertexStreamArrayType
    {
        EVertexStreamArrayType_Indices = 0,
        EVertexStreamArrayType_Positions = 1,
        EVertexStreamArrayType_PositionsTransformed = 2,
        EVertexStreamArrayType_Normals = 3,
        EVertexStreamArrayType_DiffuseColor = 4,
        EVertexStreamArrayType_SpecularColor = 5,
        EVertexStreamArrayType_PointSize = 6,
        EVertexStreamArrayType_UVs = 12,
        EVertexStreamArrayType_Unknown2DCoords1 = 15,
        EVertexStreamArrayType_Unknown2DCoords2 = 18,
        EVertexStreamArrayType_SecondaryUVs = 21,
        EVertexStreamArrayType_Tangents = 64,
        EVertexStreamArrayType_Unknown3DCoords = 72,
        EVertexStreamArrayType_Unknown2DCoords3 = 73
    };

    void SkipArray( mCGenomeFile & a_File, MIUInt uElementSize, MIBool a_bIsGenomeArray = MITrue )
    {
        a_File().Skip( a_bIsGenomeArray ? 1 : 0 );
        a_File().Skip( a_File().ReadU32() * uElementSize );
    }
}

mEResult mCXcmshReader::ReadXcmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions )
{
    a_sceneDest.Clear();
    mCString const strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    mCGenomeFile File( &a_streamSource );
    File.StartRead();
    if ( ( File().GetSize() < 11 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File.ReadString() != "eCResourceMeshComplex_PS" ) ||
         ( File().ReadU8() != 1 ) ||
         ( File().ReadU16() != 0 ) )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid .xcmsh file." ), mEResult_False;
    MIUInt uVersion = File().ReadU16();
    File().Skip( 6 );
    if ( uVersion < 81 )
        File.ReadString();
    if ( uVersion < 82 )
        File().Skip( 20 );
    File().ReadU16();
    for ( MIUInt u = File().ReadU32(); u--; )
        File.ReadString(), File.ReadString(), File().ReadU16(), File().Skip( File().ReadU32() );
    uVersion = File().ReadU16();
    MIUInt const uPSVersion = File().ReadU16();
    if ( uPSVersion > 22 )
        File().ReadU32();
    if ( uPSVersion < 30 )
        if ( File().ReadU16() > 1 )
            File().ReadBool();
    File().ReadFloat();
    MIUInt const uMeshElementCount = File().ReadU32();
    if ( uVersion < 34 )
    {
        for ( MIUInt u = 0; u != uMeshElementCount; ++u )
            a_sceneDest.AddNewNode().AccessName() = File.ReadString();
        return mEResult_Ok;
    }
    mCMesh meshDest;
    mCNode & nodeDest = a_sceneDest.AddNewNode();
    mCMultiMaterial & matMultiDest = a_sceneDest.AddNewMultiMaterial();
    nodeDest.AccessName() = strSceneName;
    nodeDest.AccessMaterialName() = matMultiDest.AccessName() = "Mtls_" + strSceneName;
    for ( MIUInt u = 0, uPassedVertCount = 0; u != uMeshElementCount; ++u, uPassedVertCount = meshDest.GetNumVerts() )
    {
        uVersion = File().ReadU16();
        File().ReadU32();
        File().Skip( 24 + 4 );
        mCMaterial & matDest = matMultiDest.AccessSubMaterials().AddNew();
        matDest.AccessName() = g_GetFileNameNoExt( File.ReadString() );
        MIBool bVertsAllocated = MIFalse;
        for ( MIUInt v = File().ReadU32(); v--; )
        {
            MIUInt const uVertexStreamArrayType = File().ReadU32();
            File().Skip( 2 + 1 );
            MIUInt uElementCount = File().ReadU32();
            MIUInt uElementSize = 4;
            switch ( uVertexStreamArrayType )
            {
            case EVertexStreamArrayType_Indices:
                meshDest.SetNumFaces( meshDest.GetNumFaces() + uElementCount / 3 );
                for ( mCMaxFace * pEnd = meshDest.AccessFaces() + meshDest.GetNumFaces(), * pFace = pEnd - uElementCount / 3; pFace != pEnd; pFace->AccessMatID() = u, ++pFace )
                    for ( MIUInt w = 3; w--; )
                        ( *pFace )[ w ] = File().ReadU32() + uPassedVertCount;
                break;
            case EVertexStreamArrayType_Positions:
                if ( !bVertsAllocated )
                    bVertsAllocated = MITrue, meshDest.SetNumVerts( meshDest.GetNumVerts() + uElementCount );
                for ( mCVec3 * pEnd = meshDest.AccessVerts() + meshDest.GetNumVerts(), * pVert = pEnd - uElementCount; pVert != pEnd; ++pVert )
                    File() >> *pVert;
                break;
            case EVertexStreamArrayType_Normals:
                meshDest.SetNumVNormals( meshDest.GetNumVNormals() + uElementCount );
                for ( mCVec3 * pEnd = meshDest.AccessVNormals() + meshDest.GetNumVNormals(), * pVNormal = pEnd - uElementCount; pVNormal != pEnd; ++pVNormal )
                    File() >> *pVNormal;
                break;
            case EVertexStreamArrayType_DiffuseColor:
                if ( !bVertsAllocated )
                    bVertsAllocated = MITrue, meshDest.SetNumVerts( meshDest.GetNumVerts() + uElementCount );
                meshDest.SetHasVertexColors( MITrue );
                for ( mCColor * pEnd = meshDest.AccessVertexColors() + meshDest.GetNumVerts(), * pVColor = pEnd - uElementCount; pVColor != pEnd; ++pVColor )
                    File() >> pVColor->AccessBlue() >> pVColor->AccessGreen() >> pVColor->AccessRed() >> pVColor->AccessAlpha();  // ToDo: Verify byte order.
                break;
            case EVertexStreamArrayType_UVs:
                meshDest.SetNumTVerts( meshDest.GetNumTVerts() + uElementCount );
                for ( mCVec3 * pEnd = meshDest.AccessTVerts() + meshDest.GetNumTVerts(), * pTVert = pEnd - uElementCount; pTVert != pEnd; ++pTVert )
                    File() >> pTVert->AccessX() >> pTVert->AccessY();
                break;
            case EVertexStreamArrayType_PositionsTransformed:
                uElementSize += 4;
            case EVertexStreamArrayType_Tangents:
            case EVertexStreamArrayType_Unknown3DCoords:
                uElementSize += 4;
            case EVertexStreamArrayType_Unknown2DCoords1:
            case EVertexStreamArrayType_Unknown2DCoords2:
            case EVertexStreamArrayType_Unknown2DCoords3:
            case EVertexStreamArrayType_SecondaryUVs:
                uElementSize += 4;
            case EVertexStreamArrayType_SpecularColor:
            case EVertexStreamArrayType_PointSize:
                File().Skip( uElementCount * uElementSize );
                break;
            default:
                return MI_ERROR( mCStreamError, EBadFormat, "Unknown vertex stream array type." ), mEResult_False;
            }
        }
        if ( uVersion > 2 )
            SkipArray( File, 4 ), SkipArray( File, 4 );
        if ( uVersion > 1 )
            for ( MIUInt v = ( File().Skip( 1 ), File().ReadU32() ); v--; File().Skip( 84 ) )
                SkipArray( File, 4 ), SkipArray( File, 4 );
        if ( uVersion > 3 )
            SkipArray( File, 24, MIFalse ), SkipArray( File, 4 );
    }
    if ( meshDest.HasTVFaces() )
        meshDest.CopyTVFacesFromFaces();
    if ( meshDest.HasVNFaces() )
        meshDest.CopyVNFacesFromFaces();
    meshDest.WeldVertices();
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( meshDest );
    nodeDest.SwapMesh( meshDest );
    if ( mCGenomeMaterial::AccessMaterialLookupHint() )
        mCGenomeMaterial::LoadGothic3Materials( a_sceneDest );
    a_sceneDest.SetName( strSceneName );
    return mEResult_Ok;
}
