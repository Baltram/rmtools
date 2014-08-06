#include "mi_include_converters.h"

namespace
{
    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mCXcomWriter::SOptions::SOptions( void ) :
    m_bConvex( MIFalse )
{
}

mEResult mCXcomWriter::WriteXcomFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    if ( mCCooking::InitRisenCooking() == mEResult_False )
        return mEResult_False;
    a_sceneSource.MakeMatMeshScene();
    MIUInt const uMeshCount = a_sceneSource.GetNumNodes();
    MIU16 const u16PropertySystemVersion = 201, u16PropertyVersion = 30;
    a_streamDest << "GR01CM00" << ( MIU32 ) 40 << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 0;
    a_streamDest << g_time() << ( MIU64 ) 0;
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( a_streamDest, "eCCollisionMeshResource2" );
    a_streamDest << ( MIU8 )( 1 ) << ( MIU16 )( 0 );
    a_streamDest << u16PropertySystemVersion << u16PropertySystemVersion;
    a_streamDest << ( MIU32 ) 0;  // @79
    a_streamDest << u16PropertySystemVersion << ( MIU32 ) 3;
    WriteString( a_streamDest, "IsConvex" );
    WriteString( a_streamDest, "bool" );
    a_streamDest << u16PropertyVersion << ( MIU32 ) 1 << a_Options.m_bConvex;
    mCBox boxInvalid;
    WriteString( a_streamDest, "Boundary" );
    WriteString( a_streamDest, "bCBox" );
    a_streamDest << u16PropertyVersion << ( MIU32 ) 24 << boxInvalid.GetMin() << boxInvalid.GetMax();
    WriteString( a_streamDest, "SubBoundaries" );
    WriteString( a_streamDest, "bTObjArray<class bCBox>" );
    a_streamDest << u16PropertyVersion << ( MIU32 )( 24 * uMeshCount + 5 ) << ( MIU8 ) 1 << ( MIU32 ) uMeshCount;
    for ( MIUInt u = 0; u != uMeshCount; ++u )
        a_streamDest << boxInvalid.GetMin() << boxInvalid.GetMax();
    a_streamDest << u16PropertySystemVersion;
    MIUInt const uDataOffset = a_streamDest.Tell();
    if ( a_Options.m_bConvex )
        a_streamDest << ( MIU32 )( 0 );
    a_streamDest << ( MIU32 ) uMeshCount;
    for ( MIUInt u = 0; u != uMeshCount; ++u )
    {
        mCMesh & meshSource = mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( *a_sceneSource.AccessNodeAt( u )->AccessMesh() );
        mCVec3 * pVerts = meshSource.AccessVerts();
        for ( MIUInt u = meshSource.GetNumVerts(); u--; pVerts[ u ] /= 100.0f );
        mCCooking::WriteCookedMesh( a_streamDest, meshSource, a_Options.m_bConvex );
    }
    if ( !a_Options.m_bConvex )
        a_streamDest << ( MIU32 )( 0 );
    a_streamDest << ( MIU32 ) uMeshCount;
    for ( MIUInt u = 0; u != uMeshCount; ++u )
    {
        mCString strMaterial = mCString( a_sceneSource.GetNodeAt( u )->GetMaterialName() ).ToLower();
        MIU8 u8ShapeMaterial = 0;
        for ( MIUInt v = mCXcomReader::s_uMaterialNameCount; v--; )
            if ( strMaterial.FirstOf( mCXcomReader::s_arrShapeMaterialNames[ v ] ) == 0 )
                u8ShapeMaterial = static_cast< MIU8 >( v );
        MIBool bIgnoredByTraceRay = strMaterial.Contains( "_i" );
        MIBool bDisableCollision  = strMaterial.Contains( "_c" );
        MIBool bDisableResponse   = strMaterial.Contains( "_r" );
        a_streamDest << u8ShapeMaterial << bIgnoredByTraceRay << bDisableCollision << bDisableResponse;
    }
    MIUInt const uOffsetEnd = a_streamDest.Tell();
    a_streamDest.Seek( 12 );
    a_streamDest << g_32( uDataOffset - 40 ) << g_32( uDataOffset ) << g_32( uOffsetEnd - uDataOffset );
    a_streamDest.Seek( 79 );
    a_streamDest << g_32( uDataOffset - 83 );
    a_streamDest.Seek( uOffsetEnd );
    return mEResult_Ok;
}
