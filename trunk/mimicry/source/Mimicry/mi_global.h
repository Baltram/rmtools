#ifndef MI_GLOBAL_H_INCLUDED
#define MI_GLOBAL_H_INCLUDED

template< MIBool B >
struct mTStaticAssert;
template<>
struct mTStaticAssert< MITrue > {};
template< MIInt N >
struct mTStaticAssert_Test {};

#define MI_STATIC_ASSERT( _EXPR_ ) typedef mTStaticAssert_Test< sizeof( mTStaticAssert< _EXPR_ > ) > MI_PASTE_TOKENS( mi_static_assert_, __COUNTER__ );

template< typename T > T const & g_limit( T const & a_Number, T const & a_LowerBound, T const & a_UpperBound );
template< typename T > T const & g_max( T const & a_1, T const & a_2 );
template< typename T > T const & g_min( T const & a_1, T const & a_2 );

template< typename T > void      g_memswap( T & a_1, T & a_2 );
template< typename T > void      g_reorder( T * a_pElements, MIUInt const * a_pPattern, MIUInt a_uElementCount );
template< typename T > void      g_swap( T & a_1, T & a_2 );

MIBool g_getbit( MILPVoid a_pBase, MIUInt uOffset );
void   g_setbit( MILPVoid a_pBase, MIUInt uOffset );
void   g_unsetbit( MILPVoid a_pBase, MIUInt uOffset );

void      g_free( MILPVoid a_pDest );
MILPVoid  g_malloc( MISize a_sizeSize );
MILPCVoid g_memchr( MILPCVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum );
MILPVoid  g_memchr( MILPVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum );
MIInt     g_memcmp( MILPCVoid a_p1, MILPCVoid a_p2, MISize a_sizeSize );
MILPVoid  g_memcpy( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize );
MILPVoid  g_memmove( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize );
MILPVoid  g_memset( MILPVoid a_pDestination, MIInt a_iValue, MISize a_sizeSize );
MILPVoid  g_realloc( MILPVoid a_pDest, MISize a_sizeSize );
MILPCChar g_strchr( MILPCChar a_pcText, MIInt a_iValue );
MILPChar  g_strchr( MILPChar a_pcText, MIInt a_iValue );
MIInt     g_strcmp( MILPCChar a_pcText1, MILPCChar a_pcText2 );
MILPChar  g_strcpy( MILPChar a_pcDestination, MILPCChar a_pcSource );
MISize    g_strlen( MILPCChar a_pcText );
MIInt     g_strncmp( MILPCChar a_pcText1, MILPCChar a_pcText2, MISize a_sizeNum );

MI_EXTERN void      ( MI_CDECL * const g_funcFree )( MILPVoid );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMalloc )( MISize );
MI_EXTERN MILPCVoid ( MI_CDECL * const g_funcMemchr_c )( MILPCVoid, MIInt, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemchr )( MILPVoid, MIInt, MISize );
MI_EXTERN MIInt     ( MI_CDECL * const g_funcMemcmp )( MILPCVoid, MILPCVoid, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemcpy )( MILPVoid, MILPCVoid, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemmove )( MILPVoid, MILPCVoid, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemset )( MILPVoid, MIInt, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcRealloc )( MILPVoid, MISize );
MI_EXTERN MILPCChar ( MI_CDECL * const g_funcStrchr_c )( MILPCChar, MIInt );
MI_EXTERN MILPChar  ( MI_CDECL * const g_funcStrchr )( MILPChar, MIInt );
MI_EXTERN MIInt     ( MI_CDECL * const g_funcStrcmp )( MILPCChar, MILPCChar );
MI_EXTERN MILPChar  ( MI_CDECL * const g_funcStrcpy )( MILPChar, MILPCChar );
MI_EXTERN MISize    ( MI_CDECL * const g_funcStrlen )( MILPCChar );
MI_EXTERN MIInt     ( MI_CDECL * const g_funcStrncmp )( MILPCChar, MILPCChar, MISize );

MI_EXTERN MIFloat const g_fPi;
MI_EXTERN MIFloat const g_fNegInfinity;
MI_EXTERN MIFloat const g_fPosInfinity;

#include "mi_global.inl"

#endif