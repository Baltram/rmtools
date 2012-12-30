template< typename T, typename U >
struct _SZeroHelper;

template< typename T >
struct _SZeroHelper< T, mSTagScalar >
{
    static void Zero( T & a_Dest ) { a_Dest = 0; }
};

template< typename T >
struct _SZeroHelper< T, mSTagNonScalar >
{
    static void Zero( T & ) {}
};

template< typename T > inline
void g_zero( T & a_Dest )
{
    _SZeroHelper< T, mSTypeTagHelper< T >::msTag >::Zero( a_Dest );
}

template< typename T >
typename mSTypeTagHelper< T >::msTag mTArray< T >::s_Tag;

template< typename T >
mTArray< T >::mTArray( T a_Value, MIUInt a_uCount )
{
    Init();
    Resize( a_uCount );
    for ( MIUInt u = a_uCount; u--; m_pElements[ u ] = a_Value );
}

template< typename T >
mTArray< T >::mTArray( MIUInt a_uMinCapacity )
{
    Init();
    Reserve( a_uMinCapacity );
}

template< typename T >
mTArray< T >::mTArray( mTArray< T > const & a_arrSource )
{
    Init();
    Reserve( a_arrSource.m_uCapacity );
    SetAt( 0, a_arrSource.m_pElements, a_arrSource.m_uCount );
}

template< typename T >
mTArray< T >::mTArray( void )
{
    Init();
}

template< typename T >
mTArray< T >::~mTArray( void )
{
    Clear();
}

template< typename T >
T & mTArray< T >::operator [] ( MIUInt a_uIndex )
{
    return m_pElements[ a_uIndex ];
}

template< typename T >
T const & mTArray< T >::operator [] ( MIUInt a_uIndex ) const
{
    return m_pElements[ a_uIndex ];
}

template< typename T >
mTArray< T > & mTArray< T >::operator = ( mTArray< T > const & a_arrSource )
{
    if ( this == &a_arrSource )
        return *this;
    mTArray< T > arrCopy( a_arrSource );
    Swap( arrCopy );
    return *this;
}

template< typename T >
MIBool mTArray< T >::operator == ( mTArray< T > const & a_arrOther ) const
{
    if ( GetCount() != a_arrOther.GetCount() )
        return MIFalse;
    for ( MIUInt u = GetCount(); u--; )
        if ( ( *this )[ u ] != a_arrOther[ u ] )
            return MIFalse;
    return MITrue;
}

template< typename T >
MIBool mTArray< T >::operator != ( mTArray< T > const & a_arrOther ) const
{
    return !( *this == a_arrOther )
}

template< typename T >
T * mTArray< T >::AccessBuffer( void )
{
    return m_pElements;
}

template< typename T >
void mTArray< T >::Add( T const & a_Element )
{
    MIUInt const uIndex = GetCount();
    Resize( uIndex + 1 );
    m_pElements[ uIndex ] = a_Element;
}

template< typename T >
T & mTArray< T >::AddNew( void )
{
    MIUInt const uIndex = GetCount();
    Resize( uIndex + 1 );
    return m_pElements[ uIndex ];
}

template< typename T >
T const & mTArray< T >::Back( void ) const
{
    return GetBuffer()[ GetCount() - 1 ];
}

template< typename T >
T & mTArray< T >::Back( void )
{
    return AccessBuffer()[ GetCount() - 1 ];
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::Begin( void ) const
{
    return CConstIterator( m_pElements, 0 );
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::Begin( void )
{
    return CIterator( m_pElements, 0 );
}

template< typename T >
void mTArray< T >::Clear( void )
{
    if ( m_pElements )
    {
        Delete( m_pElements, GetCount() );
        Free( m_pElements );
    }
    Init();
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::End( void ) const
{
    return CConstIterator( m_pElements, GetCount() );
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::End( void )
{
    return CIterator( m_pElements, GetCount() );
}

template< typename T >
T const & mTArray< T >::Front( void ) const
{
    return *GetBuffer();
}

template< typename T >
T & mTArray< T >::Front( void )
{
    return *AccessBuffer();
}

template< typename T >
T const * mTArray< T >::GetBuffer( void ) const
{
    return m_pElements;
}

template< typename T >
MIUInt mTArray< T >::GetCapacity( void ) const
{
    return m_uCapacity;
}

template< typename T >
MIUInt mTArray< T >::GetCount( void ) const
{
    return m_uCount;
}

template< typename T >
MIUInt mTArray< T >::IndexOf( T const & a_Element ) const
{
    MIUInt uIndex = GetCount();
    while ( uIndex-- )
        if ( a_Element == m_pElements[ uIndex ] )
            return uIndex;
    return MI_DW_INVALID;
}

template< typename T >
void mTArray< T >::InsertAt( MIUInt a_uIndex, T const & a_Element )
{
    MIUInt const uOldCount = GetCount();
    if ( uOldCount == a_uIndex )
    {
        Add( a_Element );
    }
    else
    {
        Resize( uOldCount + 1 );
        Delete( ( m_pElements + uOldCount ), 1 );
        Move( ( m_pElements + a_uIndex + 1 ), ( m_pElements + a_uIndex ), ( uOldCount -  a_uIndex ) );
        New( ( m_pElements + a_uIndex ), 1 );
        m_pElements[ a_uIndex ] = a_Element;
    }
}

template< typename T >
T & mTArray< T >::InsertNewAt( MIUInt a_uIndex )
{
    InsertAt( a_uIndex, T() );
    return m_pElements[ a_uIndex ];
}

template< typename T >
void mTArray< T >::Remove( T const & a_Element )
{
    RemoveAt( IndexOf( a_Element ) );
}

template< typename T >
void mTArray< T >::RemoveAt( MIUInt a_uIndex )
{
    MIUInt const uLastIndex = GetCount() - 1;
    if ( ( 0 <= a_uIndex ) && ( a_uIndex <= uLastIndex ) )
    {
        Delete( ( m_pElements + a_uIndex ), 1 );
        Move( ( m_pElements + a_uIndex ), ( m_pElements + a_uIndex + 1 ), ( uLastIndex - a_uIndex ) );
        --m_uCount;
    }
}

template< typename T >
void mTArray< T >::RemoveDuplicates( void )
{
    for ( MIUInt u = GetCount(); u--; )
        for ( MIUInt v = u; v--; )
            if ( m_pElements[ u ] == m_pElements[ v ] )
                RemoveAt( u ), v = 0;
}

template< typename T >
void mTArray< T >::Reserve( MIUInt a_uCount )
{
    if ( a_uCount > m_uCapacity )
    {
        m_uCapacity = g_max( ( m_uCapacity * 3 / 2 + 4 ), a_uCount );
        m_pElements = static_cast< T * >( Realloc( m_pElements, ( m_uCapacity * sizeof( T ) ) ) );
    }
}

template<> inline
void mCCharArray::Reserve( MIUInt a_uCount )
{
    if ( a_uCount > m_uCapacity )
    {
        m_uCapacity = g_max( ( m_uCapacity * 3 / 2 ), a_uCount );
        m_pElements = static_cast< MIChar * >( Realloc( m_pElements, ( ( m_uCapacity + 1 ) * sizeof( MIChar ) ) ) );
        New( ( m_pElements + m_uCount ), ( m_uCapacity - m_uCount + 1 ) );  // Zeroes memory.
    }
}

template< typename T >
void mTArray< T >::Resize( MIUInt a_uCount )
{
    MIUInt const uCount = GetCount();
    if ( a_uCount < uCount )
    {
        Delete( ( m_pElements + a_uCount ), ( uCount - a_uCount ) );
    }
    else if ( a_uCount > uCount )
    {
        Reserve( a_uCount );
        New( ( m_pElements + uCount ), ( a_uCount - uCount ) );
    }
    m_uCount = a_uCount;
}

template< typename T >
void mTArray< T >::SetAt( MIUInt a_uIndex, T const * a_pSource, MIUInt a_uCount )
{
    if ( ( a_uIndex + a_uCount ) > GetCount() )
    {
        Resize( a_uIndex + a_uCount );
    }
    Copy( ( m_pElements + a_uIndex ), a_pSource, a_uCount );
}

template< typename T >
void mTArray< T >::Swap( mTArray< T > & a_arrOther )
{
    if ( this == &a_arrOther )
        return;
    g_swap( m_pElements, a_arrOther.m_pElements );
    g_swap( m_uCapacity, a_arrOther.m_uCapacity );
    g_swap( m_uCount, a_arrOther.m_uCount );
}

template< typename T >
void mTArray< T >::UnReserve( void )
{
    m_uCapacity = 0;
    Reserve( m_uCount );
}

namespace
{
    template< typename T >
    void Copy_( T * a_pDestination, T const * a_pSource, MIUInt a_uCount, mSTagScalar )
    {
        if ( a_uCount )
            g_memcpy( a_pDestination, a_pSource, ( a_uCount * sizeof( T ) ) );
    }

    template< typename T >
    void Copy_( T * a_pDestination, T const * a_pSource, MIUInt a_uCount, mSTagNonScalar )
    {
        while ( a_uCount-- )
            *a_pDestination++ = *a_pSource++;
    }
}

template< typename T >
void mTArray< T >::Copy( T * a_pDestination, T const * a_pSource, MIUInt a_uCount )
{
    Copy_( a_pDestination, a_pSource, a_uCount, s_Tag );
}

namespace
{
    template< typename T >
    void Delete_( T *, MIUInt, mSTagScalar )
    {
    }

    template< typename T >
    void Delete_( T * a_pElements, MIUInt a_uCount, mSTagNonScalar )
    {
        for ( ; a_uCount--; ++a_pElements )
            a_pElements->~T();
    }
}

template< typename T >
void mTArray< T >::Delete( T * a_pElements, MIUInt a_uCount )
{
    Delete_( a_pElements, a_uCount, s_Tag );
}

template<> inline
void mCCharArray::Delete( MIChar * a_pElements, MIUInt a_uCount )
{
    New( a_pElements, a_uCount );  // Zeroes memory.
}

template< typename T >
void mTArray< T >::Free( MILPVoid a_pMemory )
{
    free( a_pMemory );
}

template< typename T >
void mTArray< T >::Move( T * a_pDestination, T const * a_pSource, MIUInt a_uCount )
{
    if ( a_uCount )
        g_memmove( a_pDestination, a_pSource, ( a_uCount * sizeof( T ) ) );
}

namespace
{
    template< typename T >
    void New_( T * a_pElements, MIUInt a_uCount, mSTagScalar )
    {
        g_memset( a_pElements, 0, ( a_uCount * sizeof( T ) ) );
    }

    template< typename T >
    void New_( T * a_pElements, MIUInt a_uCount, mSTagNonScalar )
    {
        //g_memset( a_pElements, 0, ( a_uCount * sizeof( T ) ) );
        for( ; a_uCount--; ++a_pElements )
            ::new( a_pElements ) T;
    }
}

template< typename T >
void mTArray< T >::New( T * a_pElements, MIUInt a_uCount )
{
    New_( a_pElements, a_uCount, s_Tag );
}

template< typename T >
MILPVoid mTArray< T >::Realloc( MILPVoid a_pMemory, MIUInt a_uNewSize )
{
    return realloc( a_pMemory, a_uNewSize );
}

template< typename T >
void mTArray< T >::Init( void )
{
    m_pElements = 0;
    m_uCapacity = 0;
    m_uCount = 0;
}

template< typename T >
mTArray< T >::CConstIterator::CConstIterator( CConstIterator const & a_itIterator ) :
    m_uIndex( a_itIterator.m_uIndex ),
    m_pElements( a_itIterator.m_pElements )
{
}

template< typename T >
mTArray< T >::CConstIterator::CConstIterator( T const * a_pElements, MIUInt a_uIndex ) :
    m_uIndex( a_uIndex ),
    m_pElements( const_cast< T * >( a_pElements ) )
{
}

template< typename T >
mTArray< T >::CConstIterator::CConstIterator( void ) :
    m_uIndex( 0 ),
    m_pElements( 0 )
{
}

template< typename T >
typename mTArray< T >::CConstIterator & mTArray< T >::CConstIterator::operator = ( CConstIterator const & a_itIterator )
{
    if ( this == &a_itIterator )
        return *this;
    m_uIndex = a_itIterator.m_uIndex;
    m_pElements = a_itIterator.m_pElements;
    return *this;
}

template< typename T >
T const & mTArray< T >::CConstIterator::operator * ( void ) const
{
    return m_pElements[ m_uIndex ];
}

template< typename T >
T const * mTArray< T >::CConstIterator::operator -> ( void ) const
{
    return &**this;
}

template< typename T >
T const & mTArray< T >::CConstIterator::operator [] ( MIUInt a_uIndex ) const
{
    return m_pElements[ m_uIndex + a_uIndex ];
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::CConstIterator::operator ++ ( MIInt )
{
    CConstIterator itResult( *this );
    ++( *this );
    return itResult;
}

template< typename T >
typename mTArray< T >::CConstIterator & mTArray< T >::CConstIterator::operator ++ ( void )
{
    ++m_uIndex;
    return *this;
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::CConstIterator::operator -- ( MIInt )
{
    CConstIterator itResult( *this );
    --( *this );
    return itResult;
}

template< typename T >
typename mTArray< T >::CConstIterator & mTArray< T >::CConstIterator::operator -- ( void )
{
    --m_uIndex;
    return *this;
}

template< typename T >
typename mTArray< T >::CConstIterator & mTArray< T >::CConstIterator::operator += ( MIInt a_iCount )
{
    m_uIndex += a_iCount;
    return *this;
}

template< typename T >
typename mTArray< T >::CConstIterator & mTArray< T >::CConstIterator::operator -= ( MIInt a_iCount )
{
    m_uIndex -= a_iCount;
    return *this;
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::CConstIterator::operator + ( MIInt a_iCount ) const
{
    CConstIterator itResult( *this );
    return itResult += a_iCount;  
}

template< typename T >
typename mTArray< T >::CConstIterator mTArray< T >::CConstIterator::operator - ( MIInt a_iCount ) const
{
    CConstIterator itResult( *this );
    return itResult -= a_iCount;  
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator == ( CConstIterator const & a_itOther ) const
{
    return ( m_uIndex == a_itOther.m_uIndex ) && ( m_pElements == a_itOther.m_pElements );
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator != ( CConstIterator const & a_itOther ) const
{
    return !( *this == a_itOther );
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator < ( CConstIterator const & a_itOther ) const
{
    return ( m_pElements + m_uIndex ) < ( a_itOther.m_pElements + a_itOther.m_uIndex );
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator > ( CConstIterator const & a_itOther ) const
{
    return ( m_pElements + m_uIndex ) > ( a_itOther.m_pElements + a_itOther.m_uIndex );
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator <= ( CConstIterator const & a_itOther ) const
{
    return !( *this > a_itOther );
}

template< typename T >
MIBool mTArray< T >::CConstIterator::operator >= ( CConstIterator const & a_itOther ) const
{
    return !( *this < a_itOther );
}

template< typename T >
mTArray< T >::CIterator::CIterator( CIterator const & a_itIterator ) :
    CConstIterator( a_itIterator )
{
}

template< typename T >
mTArray< T >::CIterator::CIterator( T * a_pElements, MIUInt a_uIndex ) :
    CConstIterator( a_pElements, a_uIndex )
{
}

template< typename T >
mTArray< T >::CIterator::CIterator( void )
{
}

template< typename T >
typename mTArray< T >::CIterator & mTArray< T >::CIterator::operator = ( CIterator const & a_itIterator )
{
    CConstIterator::operator = ( a_itIterator );
    return *this;
}

template< typename T >
T & mTArray< T >::CIterator::operator * ( void ) const
{
    return m_pElements[ m_uIndex ];
}

template< typename T >
T * mTArray< T >::CIterator::operator -> ( void ) const
{
    return &**this;
}

template< typename T >
T & mTArray< T >::CIterator::operator [] ( MIUInt a_uIndex ) const
{
    return m_pElements[ m_uIndex + a_uIndex ];
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::CIterator::operator ++ ( MIInt )
{
    CIterator itResult( *this );
    ++( *this );
    return itResult;
}

template< typename T >
typename mTArray< T >::CIterator & mTArray< T >::CIterator::operator ++ ( void )
{
    ++m_uIndex;
    return *this;
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::CIterator::operator -- ( MIInt )
{
    CIterator itResult( *this );
    --( *this );
    return itResult;
}

template< typename T >
typename mTArray< T >::CIterator & mTArray< T >::CIterator::operator -- ( void )
{
    --m_uIndex;
    return *this;
}

template< typename T >
typename mTArray< T >::CIterator & mTArray< T >::CIterator::operator += ( MIInt a_iCount )
{
    m_uIndex += a_iCount;
    return *this;
}

template< typename T >
typename mTArray< T >::CIterator & mTArray< T >::CIterator::operator -= ( MIInt a_iCount )
{
    m_uIndex -= a_iCount;
    return *this;
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::CIterator::operator + ( MIInt a_iCount ) const
{
    CIterator itResult( *this );                                                \
    return itResult += a_iCount;  
}

template< typename T >
typename mTArray< T >::CIterator mTArray< T >::CIterator::operator - ( MIInt a_iCount ) const
{
    CIterator itResult( *this );                                                \
    return itResult -= a_iCount; 
}

template< typename T >
MIBool mTArray< T >::CIterator::operator == ( CIterator const & a_itOther ) const
{
    return ( m_uIndex == a_itOther.m_uIndex ) && ( m_pElements == a_itOther.m_pElements );
}

template< typename T >
MIBool mTArray< T >::CIterator::operator != ( CIterator const & a_itOther ) const
{
    return !( *this == a_itOther );
}

template< typename T >
MIBool mTArray< T >::CIterator::operator < ( CIterator const & a_itOther ) const
{
    return ( m_pElements + m_uIndex ) < ( a_itOther.m_pElements + a_itOther.m_uIndex );
}

template< typename T >
MIBool mTArray< T >::CIterator::operator > ( CIterator const & a_itOther ) const
{
    return ( m_pElements + m_uIndex ) > ( a_itOther.m_pElements + a_itOther.m_uIndex );
}

template< typename T >
MIBool mTArray< T >::CIterator::operator <= ( CIterator const & a_itOther ) const
{
    return !( *this > a_itOther );
}

template< typename T >
MIBool mTArray< T >::CIterator::operator >= ( CIterator const & a_itOther ) const
{
    return !( *this < a_itOther );
}
