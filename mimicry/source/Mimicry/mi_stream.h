#ifndef MI_STREAM_H_INCLUDED
#define MI_STREAM_H_INCLUDED

MI_ERROR_DEFINE_DERIVED( mCStreamError, 
                         mCError,
                         EBadFormat,
                         ECantOpenFile,
                         EFileNotOpen,
                         EOutsizedFormattedNumber,
                         ESizeExceeded )

enum mEStreamType
{
    mEStreamType_Binary, 
    mEStreamType_Formatted 
};

class mCEndiannessHandler
{
public:
    virtual ~mCEndiannessHandler( void );
public:
    mCEndiannessHandler( void );
public:
    void SetInvertEndianness( MIBool a_bMode );
protected:
    void Handle16( MILPVoid a_pDest );
    void Handle32( MILPVoid a_pDest );
    void Handle64( MILPVoid a_pDest );
private:
    MIBool m_bInvertEndianness;
};

template< typename T >
T & g_08( T & a_Arg )
{
    MI_STATIC_ASSERT( sizeof( T ) == 1 )
    return a_Arg;
}

template< typename T >
T & g_16( T & a_Arg )
{
    MI_STATIC_ASSERT( sizeof( T ) == 2 )
    return a_Arg;
}

template< typename T >
T & g_32( T & a_Arg )
{
    MI_STATIC_ASSERT( sizeof( T ) == 4 )
    return a_Arg;
}

template< typename T >
T & g_64( T & a_Arg )
{
    MI_STATIC_ASSERT( sizeof( T ) == 8 )
    return a_Arg;
}

#include "mi_stream.inl"

#endif