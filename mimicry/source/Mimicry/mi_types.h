#ifndef MI_TYPES_H_INCLUDED
#define MI_TYPES_H_INCLUDED

#define MITrue true
#define MIFalse false

#if defined( _MSC_VER ) && _MSC_VER < 1800

typedef signed char           MII8;
typedef unsigned char         MIU8;
typedef short                 MII16;
typedef unsigned short        MIU16;
typedef long                  MII32;
typedef unsigned long         MIU32;
typedef __int64               MII64;
typedef unsigned __int64      MIU64;

typedef unsigned char         MIByte;
typedef unsigned char *       MILPByte;
typedef unsigned char const * MILPCByte;

#else

typedef int8_t       MII8;
typedef uint8_t      MIU8;
typedef int16_t      MII16;
typedef uint16_t     MIU16;
typedef int32_t      MII32;
typedef uint32_t     MIU32;
typedef int64_t      MII64;
typedef uint64_t     MIU64;

typedef MIU8         MIByte;
typedef MIU8 *       MILPByte;
typedef MIU8 const * MILPCByte;

#endif

#if UINT_MAX == 0xFFFFFFFFFFFFFFFF

typedef MII64           MIInt;
typedef MIU64           MIUInt;

#elif UINT_MAX == 0xFFFFFFFF

typedef MII32           MIInt;
typedef MIU32           MIUInt;

#elif UINT_MAX == 0xFFFF

typedef MII16           MIInt;
typedef MIU16           MIUInt;

#endif

typedef bool            MIBool;

typedef float           MIFloat;
typedef double          MIDouble;

typedef void *          MILPVoid;
typedef void const *    MILPCVoid;

typedef char            MIChar;
typedef char *          MILPChar;
typedef char const *    MILPCChar;

typedef wchar_t         MIUnicodeChar;
typedef wchar_t *       MILPUnicodeChar;
typedef wchar_t const * MILPCUnicodeChar;

typedef size_t          MISize;

#endif