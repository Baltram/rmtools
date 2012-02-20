#ifndef MI_GLOBAL_H_INCLUDED
#define MI_GLOBAL_H_INCLUDED

template< MIBool B >
struct mSStaticAssert
{
};

template<>
struct mSStaticAssert< MIFalse >
{
private:
    ~mSStaticAssert< MIFalse >( void );
};

#define MI_STATIC_ASSERT( _EXPR_ ) { mSStaticAssert< ( _EXPR_ ) > mcrStaticAssert; mcrStaticAssert; }

template< typename T > T const & g_limit( T const & a_Number, T const & a_LowerBound, T const & a_UpperBound );
template< typename T > T const & g_max( T const & a_1, T const & a_2 );
template< typename T > void      g_memswap( T & a_1, T & a_2 );
template< typename T > T const & g_min( T const & a_1, T const & a_2 );
template< typename T > void      g_reorder( T * a_pElements, MIUInt const * a_pPattern, MIUInt a_uElementCount );
template< typename T > void      g_swap( T & a_1, T & a_2 );

void      g_free( MILPVoid a_pDest );
MILPVoid  g_malloc( MISize a_sizeSize );
MILPCVoid g_memchr( MILPCVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum );
MILPVoid  g_memchr( MILPVoid a_pBlock, MIInt a_iValue, MISize a_sizeNum );
MILPVoid  g_memcpy( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize );
MILPVoid  g_memmove( MILPVoid a_pDestination, MILPCVoid a_pSource, MISize a_sizeSize );
MILPVoid  g_memset( MILPVoid a_pDestination, MIInt a_iValue, MISize a_sizeSize );
MILPVoid  g_realloc( MILPVoid a_pDest, MISize a_sizeSize );
MIInt     g_strncmp( MILPCChar a_pcText1, MILPCChar a_pcText2, MISize a_sizeNum );
MIInt     g_strcmp( MILPCChar a_pcText1, MILPCChar a_pcText2 );
MILPChar  g_strcpy( MILPChar a_pcDestination, MILPCChar a_pcSource );
MISize    g_strlen( MILPCChar a_pcText );

MI_EXTERN void      ( MI_CDECL * const g_funcFree )( MILPVoid );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMalloc )( MISize );
MI_EXTERN MILPCVoid ( MI_CDECL * const g_funcMemchr_c )( MILPCVoid, MIInt, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemchr )( MILPVoid, MIInt, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemcpy )( MILPVoid, MILPCVoid, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemmove )( MILPVoid, MILPCVoid, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcMemset )( MILPVoid, MIInt, MISize );
MI_EXTERN MILPVoid  ( MI_CDECL * const g_funcRealloc )( MILPVoid, MISize );
MI_EXTERN MIInt     ( MI_CDECL * const g_funcStrncmp )( MILPCChar, MILPCChar, MISize );
MI_EXTERN MIInt     ( MI_CDECL * const g_funcStrcmp )( MILPCChar, MILPCChar );
MI_EXTERN MILPChar  ( MI_CDECL * const g_funcStrcpy )( MILPChar, MILPCChar );
MI_EXTERN MISize    ( MI_CDECL * const g_funcStrlen )( MILPCChar );

MI_EXTERN MIFloat const g_fPi;
MI_EXTERN MIFloat const g_fNegInfinity;
MI_EXTERN MIFloat const g_fPosInfinity;

#include "mi_global.inl"

#endif