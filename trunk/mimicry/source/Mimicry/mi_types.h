#ifndef MI_TYPES_H_INCLUDED
#define MI_TYPES_H_INCLUDED

#define MITrue true
#define MIFalse false

typedef signed char           MII8;
typedef unsigned char         MIU8;
typedef short                 MII16;
typedef unsigned short        MIU16;

MI_ASSERT_32_BIT
typedef long                  MII32;
typedef unsigned long         MIU32;

typedef int                   MIInt;
typedef unsigned int          MIUInt;
typedef __int64               MII64;
typedef unsigned __int64      MIU64;
typedef bool                  MIBool;
typedef float                 MIFloat;
typedef double                MIDouble;
typedef void *                MILPVoid;
typedef void const *          MILPCVoid;
typedef char                  MIChar;
typedef char *                MILPChar;
typedef char const *          MILPCChar;
typedef unsigned char         MIByte;
typedef unsigned char *       MILPByte;
typedef unsigned char const * MILPCByte;
typedef wchar_t               MIUnicodeChar;
typedef wchar_t *             MILPUnicodeChar;
typedef wchar_t const *       MILPCUnicodeChar;

typedef size_t                MISize;

#endif