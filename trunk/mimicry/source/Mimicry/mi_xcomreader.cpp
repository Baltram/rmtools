#include "mi_include_converters.h"
#include "mi_cooking.h"

MILPCChar const mCXcomReader::s_arrShapeMaterialNames[] = {
    "none",
    "wood",
    "metal",
    "water",
    "stone",
    "earth",
    "ice",
    "leather",
    "clay",
    "glass",
    "flesh",
    "snow",
    "debris",
    "foliage",
    "magic",
    "grass",
    "springanddamper1",
    "springanddamper2",
    "springanddamper3",
    "damage",
    "sand",
    "movement",
    "axe"
};

MIUInt const mCXcomReader::s_uMaterialNameCount = static_cast< MIUInt >( sizeof( mCXcomReader::s_arrShapeMaterialNames ) / sizeof( *mCXcomReader::s_arrShapeMaterialNames ) );


namespace
{   
    mCString ReadString( mCIOStreamBinary & a_streamSource )
    {
        return a_streamSource.ReadString( a_streamSource.ReadU16() );
    }

    mCString GetUniqueMaterialName( mCString a_strName, mTStringMap< MIUInt > & a_mapMaterialUsage )
    {

        MIUInt & uUsage = a_mapMaterialUsage[ a_strName ];
        if ( uUsage )
            a_strName += mCString().Format( "_%u", uUsage );
        ++uUsage;
        return a_strName;
    }
}

mEResult mCXcomReader::ReadXcomFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options )
{
    a_sceneDest.Clear();
    if ( mCCooking::InitRisenCooking() == mEResult_False )
        return mEResult_False;
    mCString const strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    if ( a_streamSource.GetSize() < 8 || a_streamSource.ReadString( 8 ) != "GR01CM00" )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xcom file." ), mEResult_False;
    a_streamSource.Seek( a_streamSource.ReadU32() );
    if ( ( a_streamSource.ReadU16() != 1 ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( a_streamSource.ReadU16() != 1 ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( ReadString( a_streamSource ) != "eCCollisionMeshResource2" ) ||
         ( a_streamSource.ReadU8() != 1 ) ||
         ( a_streamSource.ReadU16() != 0 ) )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid ._xcom file." ), mEResult_False;
    a_streamSource.Skip( 10 );
    for ( MIUInt u = a_streamSource.ReadU32(); u--; )
        ReadString( a_streamSource ), ReadString( a_streamSource ), a_streamSource.ReadU16(), a_streamSource.Skip( a_streamSource.ReadU32() );
    a_streamSource.Skip( 2 );
    for ( MIUInt u = 0; u != 2; ++u )
    {
        for ( MIUInt v = a_streamSource.ReadU32(); v--; )
        {
            mCMesh meshDest;
            mCCooking::ReadCookedMesh( a_streamSource, meshDest );
            mCVec3 * pVerts = meshDest.AccessVerts();
            for ( MIUInt u = meshDest.GetNumVerts(); u--; pVerts[ u ] *= 100.0f );
            a_sceneDest.AddNewNode().SwapMesh( meshDest );
        }
    }
    mTStringMap< MIUInt > mapMaterialUsage;
    for ( MIUInt u = 0, ue = a_streamSource.ReadU32(); u != ue; ++u )
    {
        MIUInt uShapeMaterial = a_streamSource.ReadU8() % s_uMaterialNameCount;
        MIBool bIgnoredByTraceRay = a_streamSource.ReadBool();
        MIBool bDisableCollision  = a_streamSource.ReadBool();
        MIBool bDisableResponse   = a_streamSource.ReadBool();
        mCString strMaterialName = GetUniqueMaterialName( s_arrShapeMaterialNames[ uShapeMaterial ], mapMaterialUsage ) +
                                   ( bIgnoredByTraceRay ? "_i" : "" ) +
                                   ( bDisableCollision ? "_c" : "" ) +
                                   ( bDisableResponse ? "_r" : "" );
        a_sceneDest.AddNewMaterial().AccessName() = strMaterialName;
        mCNode * pNode = a_sceneDest.AccessNodeAt( u );
        if ( pNode )
            pNode->AccessMaterialName() = strMaterialName;
    }
    a_sceneDest.AddNewMaterial();  // Prevent the merged material from being named 'earth' or similar
    a_sceneDest.SetName( strSceneName );
    a_sceneDest.MakeOneMeshScene();
    a_sceneDest.AccessNodeAt( 0 )->AccessName() = strSceneName;
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( *a_sceneDest.AccessNodeAt( 0 )->AccessMesh() );
    return mEResult_Ok;
}
