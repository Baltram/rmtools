#include "mi_include_converters.h"

#define MI_PVFD_DUMMY virtual void MI_PASTE_TOKENS( DoNotCall_, __COUNTER__ )( void ) = 0;

namespace
{
    mCIOStreamBinary * s_pStream = 0;

    class CPhysicsStream
    {
    public:
                                    CPhysicsStream( mCIOStreamBinary & a_streamTarget ) { s_pStream = &a_streamTarget; }
        virtual                    ~CPhysicsStream( void ) {}
        virtual    MIU8             readByte( void ) const   { return s_pStream->ReadU8(); }
        virtual    MIU16            readWord( void ) const   { return s_pStream->ReadU16(); }
        virtual    MIU32            readDword( void ) const  { return s_pStream->ReadU32(); }
        virtual    MIFloat          readFloat( void ) const  { return s_pStream->ReadFloat(); }
        virtual    MIDouble         readDouble( void ) const { return s_pStream->ReadDouble(); }
        virtual    void             readBuffer( MILPVoid a_pBuffer, MIU32 a_u32Size ) const { s_pStream->Read( a_pBuffer, a_u32Size ); }
        virtual    CPhysicsStream & storeByte( MIU8 a_u8Data )      { *s_pStream << a_u8Data;  return *this; }
        virtual    CPhysicsStream & storeWord( MIU16 a_u16Data )    { *s_pStream << a_u16Data; return *this; }
        virtual    CPhysicsStream & storeDword( MIU32 a_u32Data )   { *s_pStream << a_u32Data; return *this; }
        virtual    CPhysicsStream & storeFloat( MIFloat a_fData )   { *s_pStream << a_fData;   return *this; }
        virtual    CPhysicsStream & storeDouble( MIDouble a_dData ) { *s_pStream << a_dData;   return *this; }
        virtual    CPhysicsStream & storeBuffer( MILPCVoid a_pBuffer, MIU32 a_u32Size ) { s_pStream->Write( a_pBuffer, a_u32Size ); return *this; }
    };

    struct NxTriangleMeshDesc_Dummy
    {
        MIU32     numVertices;
        MIU32     numTriangles;
        MIU32     pointStrideBytes;
        MIU32     triangleStrideBytes;
        MILPCVoid points;
        MILPCVoid triangles;
        MIU32     flags;
        MIU32     materialIndexStride;
        MILPCVoid materialIndices;
        MIU32     heightFieldVerticalAxis;
        MIFloat   heightFieldVerticalExtent;
        MILPVoid  pmap;
        MIFloat   convexEdgeThreshold;
    };

    class NxTriangleMesh_Dummy
    {
    public:
                          MI_PVFD_DUMMY MI_PVFD_DUMMY
        virtual MIU32     getCount( MIU32, MIU32 ) const = 0;
        virtual MIU32     getFormat( MIU32, MIU32 ) const = 0;
        virtual MILPCVoid getBase( MIU32, MIU32 ) const = 0;
        virtual MIU32     getStride( MIU32, MIU32 ) const = 0;
    };

    typedef NxTriangleMesh_Dummy NxConvexMesh_Dummy;
    typedef NxTriangleMesh_Dummy CTriangleOrConvexMesh;

    class NxPhysicsSDK_Dummy
    {
    public:
                                       MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY
        virtual NxTriangleMesh_Dummy * createTriangleMesh( CPhysicsStream const & ) = 0;
        virtual void                   releaseTriangleMesh( NxTriangleMesh_Dummy & ) = 0;
                                       MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY //MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY
        virtual NxConvexMesh_Dummy *   createConvexMesh( CPhysicsStream const & ) = 0;
        virtual void                   releaseConvexMesh( NxConvexMesh_Dummy & ) = 0;
    };

    class NxCookingInterface_Dummy
    {
    public:
                       MI_PVFD_DUMMY MI_PVFD_DUMMY MI_PVFD_DUMMY
        virtual MIBool NxInitCooking( MILPVoid = 0, MILPVoid = 0 ) = 0;
        virtual void   NxCloseCooking( void ) = 0;
        virtual MIBool NxCookTriangleMesh( NxTriangleMeshDesc_Dummy const &, CPhysicsStream & ) = 0;
    };

    NxPhysicsSDK_Dummy * s_pPhysicsSDK = 0;
    NxCookingInterface_Dummy * s_pCooking = 0;

    NxPhysicsSDK_Dummy * ( __cdecl * s_pfuncCreatePhysicsSDK )( MIU32, MILPVoid, MILPVoid, MILPCVoid, MILPVoid ) = 0;
    void ( __cdecl * s_pfuncReleasePhysicsSDK )( NxPhysicsSDK_Dummy * ) = 0;
    NxCookingInterface_Dummy * ( __cdecl * s_pfuncGetCookingLib )( MIU32 ) = 0;

    void ( * s_pfuncUnloadModule )( MILPCChar );
    void ( * s_pfuncRequestPhysXLoaderFunction )( void );
}

MIBool mCCooking::InitPhysicsSDK( MIUInt a_uPhysicsSDKVersion )
{
    static MIUInt s_uPhysicsSDKVersion = 0;
    if ( s_uPhysicsSDKVersion == a_uPhysicsSDKVersion )
        return MITrue;
    ReleaseSDK();
    s_uPhysicsSDKVersion = 0;
    if ( s_pfuncRequestPhysXLoaderFunction )
        ( *s_pfuncRequestPhysXLoaderFunction )();
    if ( !s_pfuncCreatePhysicsSDK || !s_pfuncGetCookingLib )
        return MIFalse;
    MII32 arrPhysicsSDKDescElements[ 7 ] = { 65536, 256, 2048, 0, 1, 128, -1 };
    s_pPhysicsSDK = ( *s_pfuncCreatePhysicsSDK )( a_uPhysicsSDKVersion, 0, 0, arrPhysicsSDKDescElements, 0 );
    if ( !s_pPhysicsSDK )
        return MIFalse;
    s_pCooking = ( *s_pfuncGetCookingLib )( a_uPhysicsSDKVersion );
    if ( !s_pCooking )
        return MIFalse;
    s_pCooking->NxInitCooking();
    s_uPhysicsSDKVersion = a_uPhysicsSDKVersion;
    return MITrue;
}

MIBool mCCooking::ReadCookedMesh( mCIOStreamBinary & a_streamSource, mCMesh & a_meshDest )
{
    a_meshDest.Clear();
    if ( !s_pCooking || !s_pPhysicsSDK || a_streamSource.GetSize() - a_streamSource.Tell() < 8 )
        return MIFalse;
    CTriangleOrConvexMesh * pPhysicsMesh = 0;
    a_streamSource.Skip( 4 );
    MIBool bIsConvex = a_streamSource.ReadString( 4 ) == "CVXM";
    a_streamSource.Skip( -8 );
    CPhysicsStream streamSource( a_streamSource );
    pPhysicsMesh = ( bIsConvex ? s_pPhysicsSDK->createConvexMesh( streamSource ) : s_pPhysicsSDK->createTriangleMesh( streamSource ) );
    if ( !pPhysicsMesh )
        return MIFalse;
    MIUInt const uVertCount = pPhysicsMesh->getCount( 0, 1 );
    MIUInt const uVertStride = pPhysicsMesh->getStride( 0, 1 );
    MIUInt const uFaceCount = pPhysicsMesh->getCount( 0, 0 );
    MIUInt const uFaceStride = pPhysicsMesh->getStride( 0, 0 );
    a_meshDest.SetNumVerts( uVertCount );
    mCVec3 * pVert = a_meshDest.AccessVerts();
    for ( MILPCByte pFloats = static_cast< MILPCByte >( pPhysicsMesh->getBase( 0, 1 ) ), pEnd = pFloats + uVertStride * uVertCount; pFloats != pEnd; pFloats += uVertStride )
        *pVert++ = *reinterpret_cast< mCVec3 const * >( pFloats ) * 100;
    MIU32 uIndexSize = 4;
    if ( pPhysicsMesh->getFormat( 0, 0 ) == 3 )
        uIndexSize = 2;
    else if ( pPhysicsMesh->getFormat( 0, 0 ) == 2 )
        uIndexSize = 1;
    a_meshDest.SetNumFaces( uFaceCount );
    MILPByte pIndicesDest = reinterpret_cast< MILPByte >( a_meshDest.AccessFaces() ) + ( g_isBigEndian() ? sizeof( MIUInt ) - uIndexSize : 0 );
    MILPCByte pIndicesSource = static_cast< MILPCByte >( pPhysicsMesh->getBase( 0, 0 ) );
    for ( MIUInt u = uFaceCount; u--; pIndicesDest += sizeof( mCMaxFace ) - sizeof( MIUInt ) * 3, pIndicesSource += uFaceStride - uIndexSize * 3 )
        for ( MIUInt v = 0; v != 3; ++v, pIndicesDest += sizeof( MIUInt ) - uIndexSize )
            for ( MIUInt w = uIndexSize; w--; )
                *pIndicesDest++ = *pIndicesSource++;
    mCMaxRisenCoordShifter::GetInstance().ShiftMeshCoords( a_meshDest );
    if ( bIsConvex )
        s_pPhysicsSDK->releaseConvexMesh( *pPhysicsMesh );
    else
        s_pPhysicsSDK->releaseTriangleMesh( *pPhysicsMesh );
    return MITrue;
}

void mCCooking::RegisterPhysXLoaderFunctions( MILPCVoid a_pCreatePhysicsSDK, MILPCVoid a_pReleasePhysicsSDK, MILPCVoid a_pGetCookingLib )
{
    *reinterpret_cast< MILPCVoid * >( &s_pfuncCreatePhysicsSDK ) = a_pCreatePhysicsSDK;
    *reinterpret_cast< MILPCVoid * >( &s_pfuncReleasePhysicsSDK ) = a_pReleasePhysicsSDK;
    *reinterpret_cast< MILPCVoid * >( &s_pfuncGetCookingLib ) = a_pGetCookingLib;
}

void mCCooking::ReleaseSDK( void )
{
    if ( s_pPhysicsSDK && s_pfuncReleasePhysicsSDK )
        ( *s_pfuncReleasePhysicsSDK )( s_pPhysicsSDK );
    if ( s_pCooking )
    {
        s_pCooking->NxCloseCooking();
        if ( s_pfuncUnloadModule )
        {
            ( *s_pfuncUnloadModule )( "PhysXCooking" );
            ( *s_pfuncUnloadModule )( "PhysXLoader" );
            ( *s_pfuncUnloadModule )( "PhysXLoader" );
            s_pfuncCreatePhysicsSDK = 0;
            s_pfuncReleasePhysicsSDK = 0;
            s_pfuncGetCookingLib = 0;
        }
    }
    s_pCooking = 0;
    s_pPhysicsSDK = 0;
}

void mCCooking::RegisterMultiVersionFunctions( void ( * a_pfuncUnloadModule )( MILPCChar ), void ( * a_pfuncRequestPhysXLoaderFunction )( void ) )
{
    if ( !a_pfuncUnloadModule || !a_pfuncRequestPhysXLoaderFunction )
        return;
    s_pfuncUnloadModule = a_pfuncUnloadModule;
    s_pfuncRequestPhysXLoaderFunction = a_pfuncRequestPhysXLoaderFunction;
}

MIBool mCCooking::WriteCookedMesh( mCIOStreamBinary & a_streamDest, mCMesh const & a_meshSource )
{
    a_streamDest; a_meshSource;
    if ( !s_pCooking || !s_pPhysicsSDK )
        return MIFalse;
    return MIFalse;
}
