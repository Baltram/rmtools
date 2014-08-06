#include "mi_include_converters.h"
#include "mi_cooking.h"

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

mEResult mCXnvmshReader::ReadXnvmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options )
{
    a_sceneDest.Clear();
    if ( mCCooking::InitGothic3Cooking() == mEResult_False )
        return mEResult_False;
    mCString const strSceneName = dynamic_cast< mCFileStream * >( &a_streamSource ) ? g_GetFileNameNoExt( dynamic_cast< mCFileStream * >( &a_streamSource )->GetFileName() ) : "";
    mCGenomeFile File( &a_streamSource );
    File.StartRead();
    if ( ( File().GetSize() < 11 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File().ReadU16() != 1 ) ||
         ( File().ReadU8() != 1 ) ||
         ( File.ReadString() != "eCResourceCollisionMesh_PS" ) ||
         ( File().ReadU8() != 1 ) ||
         ( File().ReadU16() != 0 ) )
        return MI_ERROR( mCStreamError, EBadFormat, "Invalid .xnvmsh file." ), mEResult_False;
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
    // if ( uVersion < 32 )  // Rejected by the Genome engine
    if ( uVersion < 64 )     // PhysX 2.5.0 error when loading xnvmsh files older than version 64. Gothic 3 can't either, it recreates them using the original .xcmsh data.
        return MI_ERROR( mCStreamError, EBadFormat, "Incompatible PhysX version - .xnvmsh file version too old.\n\n(Gothic 3 can't read such files either. If needed, it recreates them using the original .xcmsh data.)" ), mEResult_False;
    if ( uVersion >= 35 )
        File().ReadBool();  // HasConvexResource
    File().ReadFloat();     // ResourcePriority
    MIUInt uMeshCount = File().ReadU32();
    for ( MIUInt u = 0; u != uMeshCount; ++u )
    {
        File().ReadU64();  // Size
        mCMesh meshDest;
        if ( !mCCooking::ReadCookedMesh( File(), meshDest ) )
            return MI_ERROR( mCStreamError, EBadFormat, "PhysX error." ), mEResult_False;
        mCVec3 * pVerts = meshDest.AccessVerts();
        for ( MIUInt u = meshDest.GetNumVerts(); u--; pVerts[ u ] *= 100.0f );
        a_sceneDest.AddNewNode().SwapMesh( meshDest );
    }
    MIUInt const uPSVersion = File().ReadU16();
    if ( uPSVersion > 22 )
        File().ReadU32();
    if ( uPSVersion < 30 )
        if ( File().ReadU16() > 1 )
            File().ReadBool();
    File().Skip( 24 * ( 1 + uMeshCount ) );
    mTStringMap< MIUInt > mapMaterialUsage;
    if ( uVersion >= 42 )
    {
        File().ReadBool();
        for ( MIUInt u = 0, ue = File().ReadU32(); u != ue; ++u )
        {
            MIUInt uShapeMaterial = File().ReadU8() % mCXcomReader::s_uMaterialNameCount;
            MIBool bIgnoredByTraceRay = File().ReadBool();
            MIBool bDisableCollision  = File().ReadBool();
            MIBool bDisableResponse   = File().ReadBool();
            mCString strMaterialName = GetUniqueMaterialName( mCXcomReader::s_arrShapeMaterialNames[ uShapeMaterial ], mapMaterialUsage ) +
                                       ( bIgnoredByTraceRay ? "_i" : "" ) +
                                       ( bDisableCollision ? "_c" : "" ) +
                                       ( bDisableResponse ? "_r" : "" );
            a_sceneDest.AddNewMaterial().AccessName() = strMaterialName;
            mCNode * pNode = u < a_sceneDest.GetNumNodes() ? a_sceneDest.AccessNodeAt( u ) : 0;
            if ( pNode )
                pNode->AccessMaterialName() = strMaterialName;
        }
    }
    for ( MIUInt u = 0; u != a_sceneDest.GetNumNodes(); ++u )
        if ( a_sceneDest.GetNodeAt( u )->GetMaterialName() == "" )
            a_sceneDest.AccessNodeAt( u )->AccessMaterialName() = a_sceneDest.AddNewMaterial().AccessName() = GetUniqueMaterialName( "none", mapMaterialUsage );
    a_sceneDest.AddNewMaterial();  // Prevent the merged material from being named 'earth' or similar
    a_sceneDest.SetName( strSceneName );
    a_sceneDest.MakeOneMeshScene();
    a_sceneDest.AccessNodeAt( 0 )->AccessName() = strSceneName;
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( *a_sceneDest.AccessNodeAt( 0 )->AccessMesh() );
    return mEResult_Ok;
}
