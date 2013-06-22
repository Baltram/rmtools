#ifndef MI_COOKING_H_INCLUDED
#define MI_COOKING_H_INCLUDED

class mCCooking
{
public:
    static MIBool InitPhysicsSDK( MIUInt a_uPhysicsSDKVersion );
    static MIBool ReadCookedMesh( mCIOStreamBinary & a_streamSource, mCMesh & a_meshDest );
    static void   RegisterPhysXLoaderFunctions( MILPCVoid a_pCreatePhysicsSDK, MILPCVoid a_pReleasePhysicsSDK, MILPCVoid a_pGetCookingLib );
    static void   RegisterMultiVersionFunctions( void ( * a_pfuncUnloadModule )( MILPCChar ), void ( * a_pfuncRequestPhysXLoaderFunction )( void ) );
    static void   ReleaseSDK( void );
    static MIBool WriteCookedMesh( mCIOStreamBinary & a_streamDest, mCMesh const & a_meshSource );
};

#endif
