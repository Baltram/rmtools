#ifndef MI_STRINGALLOCATOR_H_INCLUDED
#define MI_STRINGALLOCATOR_H_INCLUDED

class mCStringAllocator
{ 
public:
    static MILPVoid Alloc( MIUInt a_uSize );
    static void     Free( MILPVoid a_pMemory );
    static MILPVoid Realloc( MILPVoid a_pMemory, MIUInt a_uNewSize );
private:
    enum { EMask = 0xDEADBEEF };
    struct SMemChunkBase;
    template< MIUInt N >
    struct TMemChunk;
    class  CChunkManager;
    struct SMemChunkSizeAssertion;
    struct SInitializer;
private:
    static void FreeBlock( SMemChunkBase * a_pChunk, MILPVoid a_pBlock, MIUInt a_uBlockFlags );
    static void ReallocNewPtrs( void );
private:
    static MIU32 **        s_pNewMemoryPtrs;
    static MIUInt          s_uNewMemoryPtrsCapacity;
    static MIUInt          s_uFirstFreeNewMemoryPtr;
    static CChunkManager * s_arrChunkManagers[ 6 ];
    static SInitializer    s_Initializer;
};

struct mCStringAllocator::SMemChunkBase
{
public:
    enum { EChunkSize = 1 << 12 };  // 4096
    struct SVacantBlockHeader
    {
        MIU32 m_u32Next;
        MIU32 m_u32Previous;
    };
public:
    MIUInt GetBlockSizeExp( void ) const;
    MIU32  GetFirstFreeBlock( void );
    MIUInt GetNumBlocks( void ) const;
    void   SetFirstFreeBlock( MIU32 a_uFirstFreeBlock );
protected:
    SMemChunkBase( MIUInt a_uBlockSize, MIUInt a_uNumBlocks, CChunkManager * a_pChunkManager, MIUInt a_uIndex );
   ~SMemChunkBase( void );
protected:
    static MILPVoid s_pChunkBuffer;
private:
    SMemChunkBase( SMemChunkBase const & );
    SMemChunkBase( void );
private:
    SMemChunkBase & operator = ( SMemChunkBase const & );
private:
    MIU64 m_u64ChunkManager;
    MIU32 m_u32NumBlocks;
    MIU32 m_u32BlockSizeExp;
    MIU32 m_u32Index;
    MIU32 m_u32FirstFreeBlock;
public:
    MIU64 m_arrBlocks[ EChunkSize / sizeof( MIU64 ) - 3 ];
    // The first ( EChunkSize * 2 / EBlockSize ) bits (plus two spare bits) are reserved.
    // If the bit at ( ( BlockAddress % EChunkSize ) * 2 / EBlockSize ) is set, the block at BlockAddress is allocated.
    // If the bit at ( ( BlockAddress % EChunkSize ) * 2 / EBlockSize + 1 ) is set, the block at BlockAddress is attached.
};

template< MIUInt N >  // 0 <= N <= 5
struct mCStringAllocator::TMemChunk : 
    public SMemChunkBase
{
public:
    enum { EBlockSize = 8 << N };  // 8, 16, ..., 256
    enum { EReservedSize = ( EChunkSize * 2 / EBlockSize + 2 + 7 ) / 8 };
    enum { ENumBlocks = ( EChunkSize - 24 - EReservedSize ) / EBlockSize };
    enum { ENumChunksPerBuffer = 7 };
public:
    static TMemChunk< N > * GetNewInstance( CChunkManager * a_pChunkManager, MIUInt a_uIndex );
private:
    TMemChunk( CChunkManager * a_pChunkManager, MIUInt a_uIndex );
   ~TMemChunk( void );
};

class mCStringAllocator::CChunkManager
{
public:
    explicit CChunkManager( MIUInt a_uLevel );
            ~CChunkManager( void );
public:
    SMemChunkBase * GetActiveChunk( void );
private:
    void ActivateChunk( MIUInt a_uIndex );
    void DeactivateChunk( MIUInt a_uIndex );
    void GenerateActiveChunk( void );
    void RemoveChunk( MIUInt a_uIndex );
private:
    struct SRecord
    {
        SMemChunkBase * m_pChunk;
        MIUInt          m_uNextActive;
        MIUInt          m_uPreviousActive;
    };
private:
    MIUInt    m_uLevel;
    MIUInt    m_uCount;
    MIUInt    m_uCapacity;
    MIUInt    m_uFirstActiveChunk;
    SRecord * m_pRecords;
private:
    friend struct mCStringAllocator::SMemChunkBase;
};

struct mCStringAllocator::SMemChunkSizeAssertion
{
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == SMemChunkBase::EChunkSize );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 0 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 1 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 2 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 3 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 4 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase ) == sizeof( TMemChunk< 5 > ) );
    MI_STATIC_ASSERT( sizeof( SMemChunkBase::SVacantBlockHeader ) == sizeof( MIU64 ) )
};

struct mCStringAllocator::SInitializer
{
    SInitializer( void );
};

#endif