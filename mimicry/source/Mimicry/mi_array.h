#ifndef MI_ARRAY_H_INCLUDED
#define MI_ARRAY_H_INCLUDED

template< typename T >
void g_zero( T & a_Dest );

template< typename T >
class mTArray
{
public:
    class CIterator;
    class CConstIterator;
public:
             mTArray( T a_Value, MIUInt a_uCount );
    explicit mTArray( MIUInt a_uMinCapacity );
             mTArray( mTArray< T > const & a_arrSource );
             mTArray( void );
            ~mTArray( void );
public:
    T &            operator [] ( MIUInt a_uIndex );
    T const &      operator [] ( MIUInt a_uIndex ) const;
    mTArray< T > & operator =  ( mTArray< T > const & a_arrSource );
    MIBool         operator == ( mTArray< T > const & a_arrOther ) const;
    MIBool         operator != ( mTArray< T > const & a_arrOther ) const;
public:
    T *            AccessBuffer( void );
    void           Add( T const & a_Element );
    void           Add( mTArray< T > const & a_arrSource );
    T &            AddNew( void );
    T const &      Back( void ) const;
    T &            Back( void );
    CConstIterator Begin( void ) const;
    CIterator      Begin( void );
    void           Clear( void );
    CConstIterator End( void ) const;
    CIterator      End( void );
    T const &      Front( void ) const;
    T &            Front( void );
    T const *      GetBuffer( void ) const;
    MIUInt         GetCapacity( void ) const;
    MIUInt         GetCount( void ) const;
    MIUInt         IndexOf( T const & a_Element ) const;
    void           InsertAt( MIUInt a_uIndex, T const & a_Element );
    T &            InsertNewAt( MIUInt a_uIndex );
    void           Remove( T const & a_Element );
    void           RemoveAt( MIUInt a_uIndex );
    void           RemoveDuplicates( void );
    void           Reserve( MIUInt a_uCount );
    void           Resize( MIUInt a_uCount );
    void           SetAt( MIUInt a_uIndex, T const * a_pSource, MIUInt a_uCount );
    void           Swap( mTArray< T > & a_arrOther );
    void           UnReserve( void );
private:
    static void     Copy( T * a_pDestination, T const * a_pSource, MIUInt a_uCount );
    static void     Delete( T * a_pElements, MIUInt a_uCount );
    static void     Free( MILPVoid a_pMemory );
    static void     Move( T * a_pDestination, T const * a_pSource, MIUInt a_uCount );
    static void     New( T * a_pElements, MIUInt a_uCount );
    static MILPVoid Realloc( MILPVoid a_pMemory, MIUInt a_uNewSize );
private:
    void Init( void );
private:
    static typename mSTypeTagHelper< T >::msTag s_Tag;
private:
    T *    m_pElements;
    MIUInt m_uCount;
    MIUInt m_uCapacity;
};

typedef mTArray< MIByte > mCByteArray;
typedef mTArray< MIChar > mCCharArray;  // Guaranteed to be 0-terminated.

template< typename T >
class mTArray< T >::CConstIterator
{
public:
    CConstIterator( CConstIterator const & a_itIterator );
    CConstIterator( T const * a_pElements, MIUInt a_uIndex );
    CConstIterator( void );
public:
    CConstIterator & operator =  ( CConstIterator const & a_itIterator );
    T const &        operator *  ( void ) const;
    T const *        operator -> ( void ) const;
    T const &        operator [] ( MIUInt a_uIndex ) const;
    CConstIterator   operator ++ ( MIInt );
    CConstIterator & operator ++ ( void );
    CConstIterator   operator -- ( MIInt );
    CConstIterator & operator -- ( void );
    CConstIterator & operator += ( MIInt a_iCount );
    CConstIterator & operator -= ( MIInt a_iCount );
    CConstIterator   operator +  ( MIInt a_iCount ) const;
    CConstIterator   operator -  ( MIInt a_iCount ) const;
    MIBool           operator == ( CConstIterator const & a_itOther ) const;
    MIBool           operator != ( CConstIterator const & a_itOther ) const;
    MIBool           operator <  ( CConstIterator const & a_itOther ) const;
    MIBool           operator >  ( CConstIterator const & a_itOther ) const;
    MIBool           operator <= ( CConstIterator const & a_itOther ) const;
    MIBool           operator >= ( CConstIterator const & a_itOther ) const;
protected:
    MIUInt m_uIndex;
    T *    m_pElements;
};

template< typename T >
class mTArray< T >::CIterator :
    public CConstIterator
{
public:
    CIterator( CIterator const & a_itIterator );
    CIterator( T * a_pElements, MIUInt a_uIndex );
    CIterator( void );
public:
    CIterator & operator =  ( CIterator const & a_itIterator );
    T &         operator *  ( void ) const;
    T *         operator -> ( void ) const;
    T &         operator [] ( MIUInt a_uIndex ) const;
    CIterator   operator ++ ( MIInt );
    CIterator & operator ++ ( void );
    CIterator   operator -- ( MIInt );
    CIterator & operator -- ( void );
    CIterator & operator += ( MIInt a_iCount );
    CIterator & operator -= ( MIInt a_iCount );
    CIterator   operator +  ( MIInt a_iCount ) const;
    CIterator   operator -  ( MIInt a_iCount ) const;
    MIBool      operator == ( CIterator const & a_itOther ) const;
    MIBool      operator != ( CIterator const & a_itOther ) const;
    MIBool      operator <  ( CIterator const & a_itOther ) const;
    MIBool      operator >  ( CIterator const & a_itOther ) const;
    MIBool      operator <= ( CIterator const & a_itOther ) const;
    MIBool      operator >= ( CIterator const & a_itOther ) const;
};

#include "mi_array.inl"

#endif
