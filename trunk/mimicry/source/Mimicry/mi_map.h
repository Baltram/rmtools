#ifndef MI_MAP_H_INCLUDED
#define MI_MAP_H_INCLUDED

void g_MurmurHash3( MILPCVoid a_pKey, MIUInt a_uKeyLength, MIU32 * a_pOut );  // Based on the MurmurHash3_x86_32 function by Austin Appleby.

template< typename K > class mTKeyManager;
template< typename K > class mTPointerKeyManager;
template< typename K > class mTStringKeyManager;

template< typename K, typename T, class C = mTKeyManager< K > >
class mTMap  // Based on the BitmapHopscotchHashMap class by Maurice Herlihy, Nir Shavit and Moran Tzafrir.
{
public:
    class CConstIterator;
    class CIterator;
public:
    explicit mTMap( MIUInt a_uMinCapacity = 28 );
             mTMap( mTMap< K, T, C > const & a_mapOther );
public:
    T &                operator [] ( K const & a_Key );
    T const &          operator [] ( K const & a_Key ) const;
    mTMap< K, T, C > & operator =  ( mTMap< K, T, C > const & a_mapOther );
public:
    CConstIterator Begin( void ) const;
    CIterator      Begin( void );
    void           Clear( void );
    MIBool         Contains( K const & a_Key ) const;
    CConstIterator End( void ) const;
    CIterator      End( void );
    MIUInt         GetCapacity( void ) const;
    MIUInt         GetCount( void ) const;
    MIUInt         GetRemainingCapacity( void ) const;
    void           RemoveAt( K const & a_Key, T * a_pDataDest = 0, MIBool a_bMayShrink = MIFalse, MIBool * a_pHasShrunk = 0 );
    void           Reserve( MIUInt a_uCount );
    MIBool         Shrink( void );
    void           Swap( mTMap< K, T, C > & a_mapOther );
private:
    enum
    {
        EHopRange = 32,
        EMaxInsertRange = 300
    };
private:
    struct SBucket;
private:
    static MIU32 Hash( K const & a_Key );
private:
    // The following methods expect as a a_u32Hash argument either the proper hash value or 0.
    T &    Add( MIU32 a_u32Hash, K const & a_Key );
    MIBool Locate( K const & a_Key, MIU32 & a_u32Hash, SBucket const *& a_pBaseBucket, SBucket const *& a_pBucket ) const;
    MIBool Locate( K const & a_Key, MIU32 & a_u32Hash, SBucket       *& a_pBaseBucket, SBucket       *& a_pBucket );
    void   PullFreeBucket( SBucket *& a_pFreeBucket );
private:
    mTArray< SBucket > m_arrBuckets;
    MIUInt             m_uElementCount;
    MIU32              m_u32BucketMask;
};

template< typename K, typename T, class C >
class mTMap< K, T, C >::CConstIterator
{
public:
    CConstIterator( CConstIterator const & a_itIterator );
    CConstIterator( mTArray< SBucket > const * a_pBuckets, MIUInt a_uIndex );
    CConstIterator( void );
public:
    CConstIterator & operator =  ( CConstIterator const & a_itIterator );
    T const &        operator *  ( void ) const;
    T const *        operator -> ( void ) const;
    CConstIterator   operator ++ ( MIInt );
    CConstIterator & operator ++ ( void );
    CConstIterator   operator -- ( MIInt );
    CConstIterator & operator -- ( void );
    MIBool           operator == ( CConstIterator const & a_itOther ) const;
    MIBool           operator != ( CConstIterator const & a_itOther ) const;
public:
    K const & GetKey( void ) const;
protected:
    mTArray< SBucket > * m_pBuckets;
    MIUInt               m_uIndex;
};

template< typename K, typename T, class C >
class mTMap< K, T, C >::CIterator :
    public CConstIterator
{
public:
    CIterator( CIterator const & a_itIterator );
    CIterator( mTArray< SBucket > * a_pBuckets, MIUInt a_uIndex );
    CIterator( void );
public:
    CIterator & operator =  ( CIterator const & a_itIterator );
    T &         operator *  ( void ) const;
    T *         operator -> ( void ) const;
    CIterator   operator ++ ( MIInt );
    CIterator & operator ++ ( void );
    CIterator   operator -- ( MIInt );
    CIterator & operator -- ( void );
    MIBool      operator == ( CIterator const & a_itOther ) const;
    MIBool      operator != ( CIterator const & a_itOther ) const;
};

template< typename K, typename T, class C >
struct mTMap< K, T, C >::SBucket
{
public:
    SBucket( void );
public:
    MIU32 m_u32HopInfo;
    MIU32 m_u32Hash;
    K     m_Key;
    T     m_Data;
};

template< typename K >
class mTKeyManager
{
public:
    static MIU32  Hash( K const & a_Val );
    static MIBool Compare( K const & a_ValLeft, K const & a_ValRight );
};

template< typename K >
class mTPointerKeyManager
{
public:
    static MIU32  Hash( K * const & a_pVal );
    static MIBool Compare( K * const & a_pValLeft, K * const & a_pValRight );
};

template< typename K >
class mTStringKeyManager
{
public:
    static MIU32  Hash( K const & a_String );
    static MIBool Compare( K const & a_StringLeft, K const & a_StringRight );
};

#include "mi_map.inl"

#endif