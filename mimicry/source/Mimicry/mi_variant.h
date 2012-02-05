#ifndef MI_VARIANT_H_INCLUDED
#define MI_VARIANT_H_INCLUDED

class mCVariant
{
public:
    mCVariant( void );
    mCVariant( mCVariant const & a_vSource );
   ~mCVariant( void );
public:
    mCVariant & operator =  ( mCVariant const & a_vSource );
    MIBool      operator == ( mCVariant const & a_vOther ) const;
    MIBool      operator != ( mCVariant const & a_vOther ) const;
public:
                           void     Clear( void );
    template< typename T > T        GetData( void ) const;
    template< typename T > mEResult GetData( T & a_Dest ) const;
                           MIBool   IsEmpty( void ) const;
    template< typename T > MIBool   IsOfType( void ) const;
    template< typename T > void     SetData( T const & a_Source );
                           void     Swap( mCVariant & a_vOther );
    template< typename T > mEResult SwapData( T & a_Element );
private:
    struct SId  // Instances of this struct may not be declared as const.
    {
    public:
        enum { EIndexExp = 7 };  // 3..8
        enum { EPoolIndexExp = 32 - EIndexExp };
    public:
        MIBool operator == ( SId const & a_idOther ) const;
    public:
        void   Invalidate( void );
        MIBool IsValid( void ) const;
    public:
        MIUInt m_uPoolIndex : EPoolIndexExp;
        MIUInt m_uIndex     : EIndexExp;
    };
    class SPool;
private:
    SId m_idElem;
};

class mCVariant::SPool
{
public:
    enum { EIndexCardinality = ( 1 << SId::EIndexExp ) - 1 };
    enum { EMigrateRatio = 6 };  // 2..8
    enum { EMaxMigrateCount = EIndexCardinality / EMigrateRatio };
    enum { EMinMapCapacity = EMaxMigrateCount * 4 };
public:
    template< typename T > static T *    Access( SId const & a_idID );
                           static SId    Clone( SId const & a_idID );
                           static MIBool Compare( SId const & a_idLeft, SId const & a_idRight );
                           static void   Free( SId & a_idID );
    template< typename T > static SId    New( T const * a_pData = 0 );
private:
    typedef mTMap< SId, SId > Map;
private:
    struct SFunctionAccessor
    {
    public:
        template< typename T > static SFunctionAccessor         GetObject( void );
        template< typename T > static SFunctionAccessor const * GetPointer( void );
    public:
        void   ( * m_funcClone )( SId &, SId const & );
        MIBool ( * m_funcCompare )( SId const &, SId const & );
        void   ( * m_funcFree )( SPool *, SId & );
    };
private:
    template< typename T > static void    CloneTemplated( SId & a_idDest, SId const & a_idSource );
    template< typename T > static void    Collapse( MIUInt a_uPoolIndex );
    template< typename T > static MIBool  CompareTemplated( SId const & a_idLeft, SId const & a_idRight );
    template< typename T > static void    FreeTemplated( SPool * a_pPool, SId & a_idID );
                           static SPool * GetPool( SId const & a_idID );
    template< typename T > static void    HandleVacantPoolIndex( MIUInt & a_uVacantPoolIndex );
    template< typename T > static void    Init( SPool * a_pPool );
    template< typename T > static void    Uncollapse( MIUInt a_uPoolIndex );
private:
    static mTArray< SPool * > s_arrPools;
    static mTArray< MIUInt >  s_arrCollapsedPoolIndices;
    static mTArray< Map * >   s_arrMaps;
    static mTArray< MIUInt >  s_arrVacantMapIndices;
    static MIUInt             s_uCollapsablePoolCount;
private:
    MIU8 *                    m_arrVacantIndices;
    void *                    m_arrElements;
    SFunctionAccessor const * m_FunctionAccessor;
    MIUInt                    m_uMigratingMapIndex;
    MIUInt                    m_uElementCount;
    MIUInt                    m_uNextVacantPoolIndex;
};

#include "mi_variant.inl"

#endif