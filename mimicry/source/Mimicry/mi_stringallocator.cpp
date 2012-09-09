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
mCStringAllocator::CChunkManager * mCStringAllocator::s_arrChunkManagers[ 6 ];

MILPVoid mCStringAllocator::Alloc( MIUInt a_uSize )
{
    static SInitializer s_Initializer;
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
    MIUInt uLevel = 0;
    for ( MIUInt uBlockSize = TMemChunk< 0 >::EBlockSize; a_uSize > uBlockSize; ++uLevel, uBlockSize <<= 1 );
    SMemChunkBase * pChunk = s_arrChunkManagers[ uLevel ]->GetActiveChunk();
    SMemChunkBase::SVacantBlockHeader * pBlock = static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pChunk->GetFirstFreeBlock() ) );
    if ( pBlock->m_u32Next )
        static_cast< SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( pChunk, pBlock->m_u32Next ) )->m_u32Previous = 0;
    pChunk->SetFirstFreeBlock( pBlock->m_u32Next );
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
        pChunk->SetFirstFreeBlock( pAttachedBlock->m_u32Next );
    return a_pMemory;
}

inline
void mCStringAllocator::FreeBlock( SMemChunkBase * a_pChunk, MILPVoid a_pBlock, MIUInt a_uBlockFlags )
{
    static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( a_pBlock )->m_u32Previous = 0;
    static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( a_pBlock )->m_u32Next = a_pChunk->GetFirstFreeBlock();
    if ( a_pChunk->GetFirstFreeBlock() )
        static_cast< mCStringAllocator::SMemChunkBase::SVacantBlockHeader * >( s_getComposedPtr( a_pChunk, a_pChunk->GetFirstFreeBlock() ) )->m_u32Previous = s_getDecomposedPtr( a_pBlock );
    a_pChunk->SetFirstFreeBlock( s_getDecomposedPtr( a_pBlock ) );
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
    return m_u32BlockSizeExp;
}

inline
MIU32 mCStringAllocator::SMemChunkBase::GetFirstFreeBlock( void )
{
    return m_u32FirstFreeBlock;
}

inline
MIUInt mCStringAllocator::SMemChunkBase::GetNumBlocks( void ) const
{
    return m_u32NumBlocks;
}

inline
void mCStringAllocator::SMemChunkBase::SetFirstFreeBlock( MIU32 a_uFirstFreeBlock )
{
    if ( a_uFirstFreeBlock )
    {
        if ( !m_u32FirstFreeBlock )
            ( *reinterpret_cast< CChunkManager ** >( &m_u64ChunkManager ) )->ActivateChunk( m_u32Index );
    }
    else
    {
        if ( m_u32FirstFreeBlock )
            ( *reinterpret_cast< CChunkManager ** >( &m_u64ChunkManager ) )->DeactivateChunk( m_u32Index );
    }
    m_u32FirstFreeBlock = a_uFirstFreeBlock;
}

mCStringAllocator::SMemChunkBase::SMemChunkBase( MIUInt a_uBlockSize, MIUInt a_uNumBlocks, CChunkManager * a_pChunkManager, MIUInt a_uIndex ) :
    m_u32NumBlocks( static_cast< MIU16 >( a_uNumBlocks ) ),
    m_u32BlockSizeExp( 0 ),
    m_u32FirstFreeBlock( 0 ),
    m_u32Index( static_cast< MIU32 >( a_uIndex ) )
{
    for ( ; a_uBlockSize != 1; a_uBlockSize >>= 1, ++m_u32BlockSizeExp );
    *reinterpret_cast< CChunkManager ** >( &m_u64ChunkManager ) = a_pChunkManager;
}

mCStringAllocator::SMemChunkBase::~SMemChunkBase( void )
{
    ( *reinterpret_cast< CChunkManager ** >( &m_u64ChunkManager ) )->RemoveChunk( m_u32Index );
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
mCStringAllocator::TMemChunk< N > * mCStringAllocator::TMemChunk< N >::GetNewInstance( CChunkManager * a_pChunkManager, MIUInt a_uIndex )
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
    return new ( pMemory ) TMemChunk< N >( a_pChunkManager, a_uIndex );
}

template< MIUInt N >
mCStringAllocator::TMemChunk< N >::TMemChunk( CChunkManager * a_pChunkManager, MIUInt a_uIndex ) :
    SMemChunkBase( EBlockSize, ENumBlocks, a_pChunkManager, a_uIndex )
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
    SetFirstFreeBlock( s_getDecomposedPtr( pFirstBlock ) );
}

template< MIUInt N >
mCStringAllocator::TMemChunk< N >::~TMemChunk( void )
{
}

mCStringAllocator::CChunkManager::CChunkManager( MIUInt a_uLevel ) :
    m_uLevel( a_uLevel ),
    m_uCount( 0 ),
    m_uCapacity( 0 ),
    m_uFirstActiveChunk( MI_DW_INVALID ),
    m_pRecords( 0 )
{
}

mCStringAllocator::CChunkManager::~CChunkManager( void )
{
    delete [] m_pRecords;
}

inline
mCStringAllocator::SMemChunkBase * mCStringAllocator::CChunkManager::GetActiveChunk( void )
{
    if ( m_uFirstActiveChunk == MI_DW_INVALID )
        GenerateActiveChunk();
    return m_pRecords[ m_uFirstActiveChunk ].m_pChunk;
}

inline
void mCStringAllocator::CChunkManager::ActivateChunk( MIUInt a_uIndex )
{
    m_pRecords[ a_uIndex ].m_uNextActive = m_uFirstActiveChunk;
    m_pRecords[ a_uIndex ].m_uPreviousActive = MI_DW_INVALID;
    if ( m_uFirstActiveChunk != MI_DW_INVALID )
        m_pRecords[ m_uFirstActiveChunk ].m_uPreviousActive = a_uIndex;
    m_uFirstActiveChunk = a_uIndex;
}

inline
void mCStringAllocator::CChunkManager::DeactivateChunk( MIUInt a_uIndex )
{
    SRecord & Record = m_pRecords[ a_uIndex ];
    if ( Record.m_uNextActive != MI_DW_INVALID )
        m_pRecords[ Record.m_uNextActive ].m_uPreviousActive = Record.m_uPreviousActive;
    if ( Record.m_uPreviousActive != MI_DW_INVALID )
        m_pRecords[ Record.m_uPreviousActive ].m_uNextActive = Record.m_uNextActive;
    else
        m_uFirstActiveChunk = Record.m_uNextActive;
    Record.m_uPreviousActive = MI_DW_INVALID;
}

void mCStringAllocator::CChunkManager::GenerateActiveChunk( void )
{
    if ( m_uCapacity == m_uCount )
    {
        m_uCapacity = m_uCapacity * 5 / 4 + 256;
        SRecord * pRecords = new SRecord [ m_uCapacity ];
        g_memcpy( pRecords, m_pRecords, sizeof( SRecord ) * m_uCount );
        delete [] m_pRecords;
        m_pRecords = pRecords;
    }
    switch ( m_uLevel )
    {
    case 0:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 0 >::GetNewInstance( this, m_uCount );
        break;
    case 1:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 1 >::GetNewInstance( this, m_uCount );
        break;
    case 2:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 2 >::GetNewInstance( this, m_uCount );
        break;
    case 3:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 3 >::GetNewInstance( this, m_uCount );
        break;
    case 4:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 4 >::GetNewInstance( this, m_uCount );
        break;
    case 5:
        m_pRecords[ m_uCount ].m_pChunk = TMemChunk< 5 >::GetNewInstance( this, m_uCount );
        break;
    }
    ++m_uCount;
}

void mCStringAllocator::CChunkManager::RemoveChunk( MIUInt a_uIndex )
{
    SRecord & Record = m_pRecords[ a_uIndex ];
    if ( ( Record.m_uPreviousActive != MI_DW_INVALID ) || ( m_uFirstActiveChunk == a_uIndex ) )
        DeactivateChunk( a_uIndex );
    Record.m_pChunk = 0;
}

mCStringAllocator::SInitializer::SInitializer( void )
{
    static MIBool s_bIsInitialized = MIFalse;
    if ( s_bIsInitialized )
        return;
    s_bIsInitialized = MITrue;
    mCStringAllocator::s_arrChunkManagers[ 0 ] = new CChunkManager( 0 );
    mCStringAllocator::s_arrChunkManagers[ 1 ] = new CChunkManager( 1 );
    mCStringAllocator::s_arrChunkManagers[ 2 ] = new CChunkManager( 2 );
    mCStringAllocator::s_arrChunkManagers[ 3 ] = new CChunkManager( 3 );
    mCStringAllocator::s_arrChunkManagers[ 4 ] = new CChunkManager( 4 );
    mCStringAllocator::s_arrChunkManagers[ 5 ] = new CChunkManager( 5 );
}

template struct mCStringAllocator::TMemChunk< 0 >;
template struct mCStringAllocator::TMemChunk< 1 >;
template struct mCStringAllocator::TMemChunk< 2 >;
template struct mCStringAllocator::TMemChunk< 3 >;
template struct mCStringAllocator::TMemChunk< 4 >;
template struct mCStringAllocator::TMemChunk< 5 >;