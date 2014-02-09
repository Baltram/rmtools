#ifndef MI_COOKING_H_INCLUDED
#define MI_COOKING_H_INCLUDED

MI_ERROR_DEFINE_DERIVED( mCPhysXError,
                         mCError,
                         EMissingSystemSoftware,
                         EMissingLegacySystemSoftware )

class mCCooking
{
public:
    static MIBool   InitCooking( MIUInt a_uPhysicsSDKVersion );
    static mEResult InitGothic3Cooking( void );
    static mEResult InitRisenCooking( void );
    static MIBool   ReadCookedMesh( mCIOStreamBinary & a_streamSource, mCMesh & a_meshDest );
    static void     RegisterPhysXLoaderFunctions( MILPCVoid a_pCreatePhysicsSDK, MILPCVoid a_pReleasePhysicsSDK, MILPCVoid a_pGetCookingLib );
    static void     RegisterMultiVersionFunctions( void ( * a_pfuncUnloadModule )( MILPCChar ), void ( * a_pfuncRequestPhysXLoaderFunction )( void ) );
    static void     ReleaseSDK( void );
    static MIBool   WriteCookedMesh( mCIOStreamBinary & a_streamDest, mCMesh a_meshSource, MIBool a_bConvex = MIFalse );
};

#endif
