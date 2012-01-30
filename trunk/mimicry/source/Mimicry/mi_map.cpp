#include "mi_include_map.h"

void g_MurmurHash3( MILPCVoid a_pKey, MIUInt a_uKeyLength, MIU32 * a_pOut )
{
    MIUInt const uBlockCount = a_uKeyLength / 4;

    MIU32 h1 = 0x21CD6723;  // Seed
    MIU32 const c1 = 0xCC9E2D51;
    MIU32 const c2 = 0x1B873593;

    // Body

    MIU32 const * pBlocks = static_cast< MIU32 const * >( a_pKey ) + uBlockCount;
    for ( MIInt i = 0 - uBlockCount; i; ++i )
    {
      MIU32 k1 = pBlocks[ i ];
      k1 *= c1;
      k1 = ( k1 << 15 ) | ( k1 >> ( 32 - 15 ) );
      k1 *= c2;
      h1 ^= k1;
      h1 = ( h1 << 13 ) | ( h1 >> ( 32 - 13 ) );
      h1 = h1 * 5 + 0xE6546B64;
    }

    // Tail

    MIU8 const * const pTail = static_cast< MIU8 const * >( a_pKey ) + uBlockCount * sizeof( MIU32 );
    MIU32 k1 = 0;
    switch( a_uKeyLength & 3 )
    {
    case 3:
        k1 ^= pTail[ 2 ] << 16;
    case 2:
        k1 ^= pTail[ 1 ] << 8;
    case 1:
        k1 ^= pTail[ 0 ];
        k1 *= c1;
        k1 = ( k1 << 15 ) | ( k1 >> ( 32 - 15 ) );
        k1 *= c2;
        h1 ^= k1;
    };

    // Finalization

    h1 ^= a_uKeyLength;
    h1 ^= h1 >> 16;
    h1 *= 0x85EBCA6B;
    h1 ^= h1 >> 13;
    h1 *= 0xC2B2AE35;
    h1 ^= h1 >> 16;

    *a_pOut = h1;
}