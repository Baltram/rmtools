#pragma once

#ifdef RM_PHYSX

#include <PhysXLoader.h>
#include <NxCooking.h>
#include "rmOut.h"

static NxPhysicsSDK * gPhysicsSdk = 0;
static NxCookingInterface * gCooking = 0;

static void initSdk(void)
{
    if (gPhysicsSdk == 0) {
	gPhysicsSdk = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, 0);
	if (!gPhysicsSdk) {
	    rmOut out("rmPhysxGlobals.h");
	    OUT_SERR("Wrong SDK DLL version?")
	}
    }
    if (gCooking == 0) {
	gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if (!gCooking) {
	    rmOut out("rmPhysxGlobals.h");
	    OUT_SERR("Wrong SDK DLL version?")
	}
	else {
	    gCooking->NxInitCooking();
	}
    }
}

static void releaseSdk(void)
{
    if (gPhysicsSdk != 0) {
	gPhysicsSdk->release();
	gPhysicsSdk = 0;
    }
    if (gCooking != 0) {
	gCooking->NxCloseCooking();
	gCooking = 0;
    }
}

#endif // RM_PHYSX
