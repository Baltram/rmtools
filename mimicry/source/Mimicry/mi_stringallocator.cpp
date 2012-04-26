#include "mi_include_string.h"

namespace
{
    inline
    ptrdiff_t s_andPtr_i( MILPVoid a_Ptr, ptrdiff_t a_Val )
    {
        return reinterpret_cast< ptrdiff_t >( a_Ptr ) & a_Val;
    }

    inline
    MILPVoid s_andPtr( MILPVoid a_Ptr, ptrdiff_t a_Val )
    {
        return reinterpret_cast< MILPVoid >( s_andPtr_i( a_Ptr, a_Val ) );
    }

    inline
    ptrdiff_t s_alignPtr_i( MILPVoid a_Ptr, ptrdiff_t a_Val )
    {
        return s_andPtr_i( a_Ptr, ~a_Val );
    }

    inline
    MILPVoid s_alignPtr( MILPVoid a_Ptr, ptrdiff_t a_Val )
    {
        return reinterpret_cast< MILPVoid >( s_alignPtr_i( a_Ptr, a_Val ) );
    }

    inline
    MIU32 s_getDecomposedPtr( MILPVoid a_Ptr )
    {
        return static_cast< MIU32 >( s_andPtr_i( a_Ptr, 0xFFFFFFFF ) );
    }

    inline
    MILPVoid s_getComposedPtr( MILPVoid a_Ptr, MIU32 a_u32Low )
    {
        return reinterpret_cast< MILPVoid >( s_alignPtr_i( a_Ptr, 0xFFFFFFFF ) + a_u32Low );
    }
}

MIU32 ** mCStringAllocator::s_pNewMemoryPtrs = 0;
MIUInt mCStringAllocator::s_uNewMemoryPtrsCapacity = 0;
MIUInt mCStringAllocator::s_uFirstFreeNewMemoryPtr = 0;
MILPVoid mCStringAllocator::SMemChunkBase::s_pChunkBuffer = 0;
mCStringAllocator::SMemChunkBase * mCStringAllocator::s_arrChunkCircles[ 6 ];
mCStringAllocator::SInitializer mCStringAllocator::s_Initializer;

MILPVoid mCStringAllocator::Alloc( MIUInt a_uSize )
{
    if ( a_uSize > TMemChunk< 5 >::EBlockSize )
    {
        a_uSize = ( a_uSize + 3 ) / 4;
        if ( !s_uFirstFreeNewMemoryPtr )
            ReallocNewPtrs();
        MIU32 *& pResult = s_pNewMemoryPtrs[ s_uFirstFreeNewMemoryPtr ];
        MIUInt const uNextFree = static_cast< MIUInt >( *reinterpret_cast< ptrdiff_t * >( &pResult ) );
        pResult = new MIU32 [ a_uSize + 2 ];
        pResult[ 0 ] = a_uSize * 4;
        pResult[ 1 ] = s_uFirstFreeNewMemoryPtr ^ EMask;
        s_uFirstFreeNewMemoryPtr = uNextFree;
        return pResult + 2;
    }
    SMemChunkBase * pChunk = FindMemChunk( a_uSize );
    SMemChunkBase::SVacantBlockHeader * pBlock = static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pChunk->m_u32FirstFreeBlock ) );
    if ( pBlock->m_u32Next )
        static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pBlock->m_u32Next ) )->m_u32Previous = 0;
    pChunk->m_u32FirstFreeBlock = pBlock->m_u32Next;
    g_setbit( pChunk->m_arrBlocks, static_cast< MIUInt >( s_andPtr_i( pBlock, SMemChunkBase::EChunkSize - 1 ) >> ( pChunk->GetBlockSizeExp() - 1 ) ) );
    return pBlock;
}

void mCStringAllocator::Free( MILPVoid const a_pMemory )
{
    if ( !a_pMemory )
        return;
    MIUInt const uNewPtrIndex = static_cast< MIU32 const * >( a_pMemory )[ -1 ] ^ EMask;
    if ( ( uNewPtrIndex < s_uNewMemoryPtrsCapacity ) && ( s_pNewMemoryPtrs[ uNewPtrIndex ] + 2 == a_pMemory ) )
    {
        delete [] s_pNewMemoryPtrs[ uNewPtrIndex ];
        reinterpret_cast< ptrdiff_t * >( s_pNewMemoryPtrs )[ uNewPtrIndex ] = s_uFirstFreeNewMemoryPtr;
        s_uFirstFreeNewMemoryPtr = uNewPtrIndex;
        return;
    }
    SMemChunkBase * pChunk = static_cast< SMemChunkBase * >( s_alignPtr( a_pMemory, SMemChunkBase::EChunkSize - 1 ) );
    MILPByte pBlock = static_cast< MILPByte >( a_pMemory );
    MIUInt uBlockSize = 1 << pChunk->GetBlockSizeExp();
    MIUInt uBlockFlags = static_cast< MIUInt >( s_andPtr_i( pBlock, SMemChunkBase::EChunkSize - 1 ) >> ( pChunk->GetBlockSizeExp() - 1 ) );
    FreeBlock( pChunk, pBlock, uBlockFlags );
    // Traversing attached blocks.
    for ( uBlockFlags += 2, pBlock += uBlockSize; g_getbit( pChunk->m_arrBlocks, uBlockFlags + 1 ); uBlockFlags += 2, pBlock += uBlockSize )
        FreeBlock( pChunk, pBlock, uBlockFlags );
}

MILPVoid mCStringAllocator::Realloc( MILPVoid const a_pMemory, MIUInt const a_uNewSize )
{
    if ( !a_pMemory )
        return Alloc( a_uNewSize );
    MIUInt const uNewPtrIndex = static_cast< MIU32 const * >( a_pMemory )[ -1 ] ^ EMask;
    if ( ( uNewPtrIndex < s_uNewMemoryPtrsCapacity ) && ( s_pNewMemoryPtrs[ uNewPtrIndex ] + 2 == a_pMemory ) )
    {
        MIUInt uSize = s_pNewMemoryPtrs[ uNewPtrIndex ][ 0 ];
        if ( ( a_uNewSize < uSize ) && ( a_uNewSize > uSize * 2 / 5 ) )
            return a_pMemory;
        MILPVoid pResult = g_memcpy( Alloc( a_uNewSize ), a_pMemory, g_min( uSize, a_uNewSize ) );
        Free( a_pMemory );
        return pResult;
    }
    SMemChunkBase * pChunk = static_cast< SMemChunkBase * >( s_alignPtr( a_pMemory, SMemChunkBase::EChunkSize - 1 ) );
    MIUInt uBlockSize = 1 << pChunk->GetBlockSizeExp();
    MIUInt uTotalSize = uBlockSize;
    MIUInt uFlagAttached = 0;
    for ( ; ; )
    {
        if ( a_uNewSize <= uTotalSize )
        {
            if ( a_uNewSize < uTotalSize / 3 )
            {
                MILPVoid pResult = a_uNewSize ? g_memcpy( Alloc( a_uNewSize ), a_pMemory, a_uNewSize ) : 0;
                Free( a_pMemory );
                return pResult;
            }
            return a_pMemory;
        }
        if ( uFlagAttached )
            break;
        uFlagAttached = static_cast< MIUInt >( s_andPtr_i( a_pMemory, SMemChunkBase::EChunkSize - 1 ) >> ( pChunk->GetBlockSizeExp() - 1 ) ) + 1;
        // Traverse attached blocks.
        for ( uFlagAttached += 2; g_getbit( pChunk->m_arrBlocks, uFlagAttached ); uFlagAttached += 2 )
            uTotalSize += uBlockSize;
    }
    // If attaching the next block isn't sufficient or the block already has been allocated.
    if ( ( a_uNewSize > uTotalSize + uBlockSize ) || ( g_getbit( pChunk->m_arrBlocks, uFlagAttached - 1 ) ) )
    {
        MILPVoid pResult = g_memcpy( Alloc( a_uNewSize ), a_pMemory, uTotalSize );
        Free( a_pMemory );
        return pResult;
    }
    // Attach the next block.
    g_setbit( pChunk->m_arrBlocks, uFlagAttached );
    SMemChunkBase::SVacantBlockHeader * pAttachedBlock = reinterpret_cast< SMemChunkBase::SVacantBlockHeader * >( static_cast< MILPByte >( a_pMemory ) + uTotalSize );
    if ( pAttachedBlock->m_u32Next )
        static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pAttachedBlock->m_u32Next ) )->m_u32Previous = pAttachedBlock->m_u32Previous;
    if ( pAttachedBlock->m_u32Previous )
        static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pAttachedBlock->m_u32Previous ) )->m_u32Next = pAttachedBlock->m_u32Next;
    else
        pChunk->m_u32FirstFreeBlock = pAttachedBlock->m_u32Next;
    return a_pMemory;
}

inline
mCStringAllocator::SMemChunkBase * mCStringAllocator::FindMemChunk( MIUInt a_uSize )
{
    MIUInt u = 0;
    for ( MIUInt uBlockSize = TMemChunk< 0 >::EBlockSize; a_uSize > uBlockSize; ++u, uBlockSize <<= 1 );
    SMemChunkBase * pChunk = s_arrChunkCircles[ u ];
    if ( pChunk->m_u32FirstFreeBlock )
        return pChunk;
    for ( SMemChunkBase * p = pChunk->GetNext(); p != pChunk; p = p->GetNext() )
        if ( p->m_u32FirstFreeBlock )
            return s_arrChunkCircles[ u ] = p;
    switch ( u )
    {
    case 0:
        return s_arrChunkCircles[ u ] = TMemChunk< 0 >::GetNewInstance( static_cast< TMemChunk< 0 > * >( pChunk ) );
    case 1:
        return s_arrChunkCircles[ u ] = TMemChunk< 1 >::GetNewInstance( static_cast< TMemChunk< 1 > * >( pChunk ) );
    case 2:
        return s_arrChunkCircles[ u ] = TMemChunk< 2 >::GetNewInstance( static_cast< TMemChunk< 2 > * >( pChunk ) );
    case 3:
        return s_arrChunkCircles[ u ] = TMemChunk< 3 >::GetNewInstance( static_cast< TMemChunk< 3 > * >( pChunk ) );
    case 4:
        return s_arrChunkCircles[ u ] = TMemChunk< 4 >::GetNewInstance( static_cast< TMemChunk< 4 > * >( pChunk ) );
    case 5:
        return s_arrChunkCircles[ u ] = TMemChunk< 5 >::GetNewInstance( static_cast< TMemChunk< 5 > * >( pChunk ) );
    }
    return 0;
}

inline
void mCStringAllocator::FreeBlock( SMemChunkBase * a_pChunk, MILPVoid a_pBlock, MIUInt a_uBlockFlags )
{
    static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( a_pBlock )->m_u32Previous = 0;
    static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( a_pBlock )->m_u32Next = a_pChunk->m_u32FirstFreeBlock;
    if ( a_pChunk->m_u32FirstFreeBlock )
        static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( a_pChunk, a_pChunk->m_u32FirstFreeBlock ) )->m_u32Previous = s_getDecomposedPtr( a_pBlock );
    a_pChunk->m_u32FirstFreeBlock = s_getDecomposedPtr( a_pBlock );
    g_unsetbit( a_pChunk->m_arrBlocks, a_uBlockFlags );
    g_unsetbit( a_pChunk->m_arrBlocks, a_uBlockFlags + 1 );
}

void mCStringAllocator::ReallocNewPtrs( void )
{
    MIUInt const uNewCapacity = s_uNewMemoryPtrsCapacity * 5 / 4 + 512;
    MIU32 ** pPtrs = new MIU32 * [ uNewCapacity ];
    g_memcpy( pPtrs, s_pNewMemoryPtrs, s_uNewMemoryPtrsCapacity * sizeof( MIU32 * ) );
    for ( MIUInt u = uNewCapacity - 1; u != s_uNewMemoryPtrsCapacity; --u )
        reinterpret_cast< ptrdiff_t * >( pPtrs )[ u ] = u - 1;
    reinterpret_cast< ptrdiff_t * >( pPtrs )[ s_uNewMemoryPtrsCapacity ] = s_uFirstFreeNewMemoryPtr;
    s_uFirstFreeNewMemoryPtr = uNewCapacity - 1;
    delete [] s_pNewMemoryPtrs;
    s_pNewMemoryPtrs = pPtrs;
    s_uNewMemoryPtrsCapacity = uNewCapacity;
}

inline
MIUInt mCStringAllocator::SMemChunkBase::GetBlockSizeExp( void ) const
{
    return m_u16BlockSizeExp;
}

inline
MIUInt mCStringAllocator::SMemChunkBase::GetNumBlocks( void ) const
{
    return m_u16NumBlocks;
}

inline
mCStringAllocator::SMemChunkBase * mCStringAllocator::SMemChunkBase::GetNext( void ) const
{
    return *reinterpret_cast< SMemChunkBase * const * >( &m_u64NextChunk );
}

mCStringAllocator::SMemChunkBase::SMemChunkBase( MIUInt a_uBlockSize, MIUInt a_uNumBlocks, SMemChunkBase * a_pCreator ) :
    m_u16NumBlocks( static_cast< MIU16 >( a_uNumBlocks ) ),
    m_u16BlockSizeExp( 0 )
{
    for ( ; a_uBlockSize != 1; a_uBlockSize >>= 1, ++m_u16BlockSizeExp );
    SMemChunkBase *& pNextChunk = *reinterpret_cast< SMemChunkBase ** >( &m_u64NextChunk );
    if ( !a_pCreator )
    {
        pNextChunk = this;
        return;
    }
    pNextChunk = a_pCreator;
    while ( a_pCreator->GetNext() != pNextChunk )
        a_pCreator = a_pCreator->GetNext();
    *reinterpret_cast< SMemChunkBase ** >( &a_pCreator->m_u64NextChunk ) = this;
}

mCStringAllocator::SMemChunkBase::~SMemChunkBase( void )
{
    SMemChunkBase * pNextChunk = *reinterpret_cast< SMemChunkBase ** >( &m_u64NextChunk );
    SMemChunkBase * pPreviousChunk = pNextChunk;
    while ( pPreviousChunk->GetNext() != this )
        pPreviousChunk = pPreviousChunk->GetNext();
    *reinterpret_cast< SMemChunkBase ** >( &pPreviousChunk->m_u64NextChunk ) = pNextChunk;
}

mCStringAllocator::SMemChunkBase::SMemChunkBase( SMemChunkBase const & )
{
}

mCStringAllocator::SMemChunkBase::SMemChunkBase( void )
{
}

mCStringAllocator::SMemChunkBase & mCStringAllocator::SMemChunkBase::operator = ( SMemChunkBase const & )
{
    return *this;
}

template< MIUInt N >
mCStringAllocator::TMemChunk< N > * mCStringAllocator::TMemChunk< N >::GetNewInstance( TMemChunk< N > * a_pCreator )
{
    if ( ( !s_pChunkBuffer ) || ( !*static_cast< MIByte * >( s_pChunkBuffer ) ) )
    {
        MIUInt const uSize = EChunkSize * ( ENumChunksPerBuffer + 1 ) - 1 + sizeof( MILPVoid ) * 3;
        MILPByte pNewChunkBuffer = new MIByte [ uSize ];
        pNewChunkBuffer[ 0 ] = ENumChunksPerBuffer;
        reinterpret_cast< MILPVoid * >( pNewChunkBuffer )[ 1 ] = s_alignPtr( pNewChunkBuffer + sizeof( MILPVoid ) * 3 - 1, EChunkSize - 1 );
        reinterpret_cast< MILPVoid * >( pNewChunkBuffer )[ 2 ] = s_pChunkBuffer;
        s_pChunkBuffer = pNewChunkBuffer;
    }
    MILPByte pMemory = static_cast< MILPByte >( static_cast< MILPVoid * >( s_pChunkBuffer )[ 1 ] );
    pMemory += EChunkSize * ( *static_cast< MIByte * >( s_pChunkBuffer ) )--;
    return new ( pMemory ) TMemChunk< N >( a_pCreator );
}

template< MIUInt N >
mCStringAllocator::TMemChunk< N >::TMemChunk( TMemChunk< N > * a_pCreator ) :
    SMemChunkBase( EBlockSize, ENumBlocks, a_pCreator )
{
    enum { EBlockQWORDs = EBlockSize / sizeof( MIU64 ) };
    enum { EReservedQWORDs = ( EReservedSize + 7 ) / 8 };
    SVacantBlockHeader * const pFirstBlock = reinterpret_cast< SVacantBlockHeader * >( m_arrBlocks + EReservedQWORDs );
    SVacantBlockHeader * const pEndBlocks = pFirstBlock + ENumBlocks * EBlockQWORDs;

    g_memset( m_arrBlocks, 0, EReservedSize );
    // Preventing the (nonexistent) block past the last from being attached.
    g_setbit( m_arrBlocks, static_cast< MIUInt >( ( reinterpret_cast< ptrdiff_t >( pEndBlocks ) % EChunkSize ) * 2 / EBlockSize ) );
    g_setbit( m_arrBlocks, static_cast< MIUInt >( ( reinterpret_cast< ptrdiff_t >( pEndBlocks - EBlockQWORDs ) % EChunkSize ) * 2 / EBlockSize ) + 2 );

    SVacantBlockHeader * p, * q;
    for ( p = pFirstBlock, q = p + 2 * EBlockQWORDs; q < pEndBlocks; p = q, q += 2 * EBlockQWORDs )
    {
        p->m_u32Next = s_getDecomposedPtr( q );
        q->m_u32Previous = s_getDecomposedPtr( p );
    }
    for ( q = pFirstBlock + EBlockQWORDs; q < pEndBlocks; p = q, q += 2 * EBlockQWORDs )
    {
        p->m_u32Next = s_getDecomposedPtr( q );
        q->m_u32Previous = s_getDecomposedPtr( p );
    }
    p->m_u32Next = 0;
    pFirstBlock->m_u32Previous = 0;
    m_u32FirstFreeBlock = s_getDecomposedPtr( pFirstBlock );
}

template< MIUInt N >
mCStringAllocator::TMemChunk< N >::~TMemChunk( void )
{
}

mCStringAllocator::SInitializer::SInitializer( void )
{
    static MIBool s_bIsInitialized = MIFalse;
    if ( s_bIsInitialized )
        return;
    s_bIsInitialized = MITrue;
    mCStringAllocator::s_arrChunkCircles[ 0 ] = TMemChunk< 0 >::GetNewInstance( 0 );
    mCStringAllocator::s_arrChunkCircles[ 1 ] = TMemChunk< 1 >::GetNewInstance( 0 );
    mCStringAllocator::s_arrChunkCircles[ 2 ] = TMemChunk< 2 >::GetNewInstance( 0 );
    mCStringAllocator::s_arrChunkCircles[ 3 ] = TMemChunk< 3 >::GetNewInstance( 0 );
    mCStringAllocator::s_arrChunkCircles[ 4 ] = TMemChunk< 4 >::GetNewInstance( 0 );
    mCStringAllocator::s_arrChunkCircles[ 5 ] = TMemChunk< 5 >::GetNewInstance( 0 );
}

template struct mCStringAllocator::TMemChunk< 0 >;
template struct mCStringAllocator::TMemChunk< 1 >;
template struct mCStringAllocator::TMemChunk< 2 >;
template struct mCStringAllocator::TMemChunk< 3 >;
template struct mCStringAllocator::TMemChunk< 4 >;
template struct mCStringAllocator::TMemChunk< 5 >;