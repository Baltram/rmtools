#include "rmMurmurHash.h"

FORCE_INLINE uint32_t getblock ( const uint32_t * p, int i )
{
  return p[i];
}

FORCE_INLINE void bmix32 ( uint32_t & h1, uint32_t & k1,
			   uint32_t & c1, uint32_t & c2 )
{
  c1 = c1*5+0x7b7d159c;
  c2 = c2*5+0x6bce6396;

  k1 *= c1;
  k1 = ROTL32(k1,11);
  k1 *= c2;

  h1 = ROTL32(h1,13);
  h1 = h1*5+0x52dce729;
  h1 ^= k1;
}

void MurmurHash ( const void * key, int len,
			  uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;

  uint32_t h1 = 0x971e137b ^ seed;

  uint32_t c1 = 0x95543787;
  uint32_t c2 = 0x2ad7eb25;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    bmix32(h1,k1,c1,c2);
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
      bmix32(h1,k1,c1,c2);
  };

  //----------
  // finalization

  h1 ^= len;

  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  h1 ^= seed;

  *(uint32_t*)out = h1;
}

void MurmurHash ( const void * key, int len, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;

  uint32_t h1 = 0x971e137b;

  uint32_t c1 = 0x95543787;
  uint32_t c2 = 0x2ad7eb25;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    bmix32(h1,k1,c1,c2);
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
      bmix32(h1,k1,c1,c2);
  };

  //----------
  // finalization

  h1 ^= len;

  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  *(uint32_t*)out = h1;
}
