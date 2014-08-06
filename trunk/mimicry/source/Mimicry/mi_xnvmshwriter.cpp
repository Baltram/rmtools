#include "mi_include_converters.h"

namespace
{
    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mCXnvmshWriter::SOptions::SOptions( void ) :
    m_bConvex( MIFalse )
{
}

mEResult mCXnvmshWriter::WriteXnvmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options )
{
    if ( mCCooking::InitGothic3Cooking() == mEResult_False )
        return mEResult_False;
    a_sceneSource.MakeMatMeshScene();
    MIUInt const uMeshCount = a_sceneSource.GetNumNodes();
    MIU16 const u16PropertySystemVersion = 83, u16PropertyVersion = 30, u16CollisionMeshVersion = 64;
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( a_streamDest, "eCResourceCollisionMesh_PS" );
    a_streamDest << ( MIU8 )( 1 ) << ( MIU16 )( 0 );
    a_streamDest << u16PropertySystemVersion  << u16PropertySystemVersion;
    MIUInt const uResourceSizeOffset = a_streamDest.Tell();
    a_streamDest << ( MIU32 )( 0 ) << u16PropertyVersion << ( MIU32 )( 1 );
    WriteString( a_streamDest, "ResourcePriority" );
    WriteString( a_streamDest, "float" );
    a_streamDest << u16PropertyVersion << sizeof( MIFloat ) << 0.0f;
    a_streamDest << u16CollisionMeshVersion << a_Options.m_bConvex << 0.0f;
    a_streamDest << ( MIU32 ) uMeshCount;
    mTArray< mCBox > arrBoundingBoxes( mCBox(), 1 );
    for ( MIUInt u = 0; u != uMeshCount; ++u )
    {
        MIUInt const uCookedMeshSizeOffset = a_streamDest.Tell();
        a_streamDest << ( MIU64 ) 0;
        mCMesh & meshSource = mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( *a_sceneSource.AccessNodeAt( u )->AccessMesh() );
        arrBoundingBoxes.Add( meshSource.CalcExtents() );
        arrBoundingBoxes.Front() |= arrBoundingBoxes.Back();
        mCVec3 * pVerts = meshSource.AccessVerts();
        for ( MIUInt u = meshSource.GetNumVerts(); u--; pVerts[ u ] /= 100.0f );
        mCCooking::WriteCookedMesh( a_streamDest, meshSource, a_Options.m_bConvex );
        MIUInt const uCurrentOffset = a_streamDest.Tell();
        a_streamDest.Seek( uCookedMeshSizeOffset );
        a_streamDest << static_cast< MIU64 >( uCurrentOffset - uCookedMeshSizeOffset - sizeof( MIU64 ) );
        a_streamDest.Seek( uCurrentOffset );
    }
    a_streamDest << ( MIU16 )( 1 ) << ( MIU16 )( 1 );
    for ( MIUInt u = 0, ue = arrBoundingBoxes.GetCount(); u != ue; ++u )
        a_streamDest << arrBoundingBoxes[ u ].GetMin() << arrBoundingBoxes[ u ].GetMax();
    a_streamDest << a_Options.m_bConvex << ( MIU32 ) uMeshCount;
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
    a_streamDest.Seek( uResourceSizeOffset );
    a_streamDest << g_32( uOffsetEnd - uResourceSizeOffset - 4 );
    a_streamDest.Seek( uOffsetEnd );
    return mEResult_Ok;
}
