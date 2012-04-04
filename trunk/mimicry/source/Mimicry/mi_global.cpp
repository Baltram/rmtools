#include "mi_include_core.h"
#include "mi_include_standard.h"

MIFloat const g_fPi = 3.14159265f;
MIFloat const g_fNegInfinity = FLT_MIN;
MIFloat const g_fPosInfinity = FLT_MAX;

void      ( MI_CDECL * const g_funcFree )( MILPVoid )                        = &free;
MILPVoid  ( MI_CDECL * const g_funcMalloc )( MISize )                        = &malloc;
MILPCVoid ( MI_CDECL * const g_funcMemchr_c )( MILPCVoid, MIInt, MISize )    = &memchr;
MILPVoid  ( MI_CDECL * const g_funcMemchr )( MILPVoid, MIInt, MISize )       = &memchr;
MILPVoid  ( MI_CDECL * const g_funcMemcpy )( MILPVoid, MILPCVoid, MISize )   = &memcpy;
MILPVoid  ( MI_CDECL * const g_funcMemmove )( MILPVoid, MILPCVoid, MISize )  = &memmove;
MILPVoid  ( MI_CDECL * const g_funcMemset )( MILPVoid, MIInt, MISize )       = &memset;
MILPVoid  ( MI_CDECL * const g_funcRealloc )( MILPVoid, MISize )             = &realloc;
MILPCChar ( MI_CDECL * const g_funcStrchr_c )( MILPCChar, MIInt )            = &strchr;
MILPChar  ( MI_CDECL * const g_funcStrchr )( MILPChar, MIInt )               = &strchr;
MIInt     ( MI_CDECL * const g_funcStrcmp )( MILPCChar, MILPCChar )          = &strcmp;
MISize    ( MI_CDECL * const g_funcStrlen )( MILPCChar )                     = &strlen;
MIInt     ( MI_CDECL * const g_funcStrncmp )( MILPCChar, MILPCChar, MISize ) = &strncmp;

MI_CRT_NO_WARNINGS( MILPChar  ( MI_CDECL * const g_funcStrcpy )( MILPChar, MILPCChar ) = &strcpy; )