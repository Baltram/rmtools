#include "mi_include_streams.h"

mCRefStream::~mCRefStream( void )
{
}

mCRefStream::mCRefStream( MILPVoid a_pTarget, MIUInt a_uSize ) :
    m_pMemory( static_cast< MIByte * >( a_pTarget ) ),
    m_uSize( a_uSize )
{
}

mCRefStream::mCRefStream( mCBuffer & a_bufTarget ) :
    m_pMemory( a_bufTarget.AccessMemory() ),
    m_uSize( a_bufTarget.GetSize() )
{
}

mEResult mCRefStream::Read( MILPVoid a_pDest, MIUInt a_uSize )
{
    if ( a_uSize > m_uSize )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Exceeded valid memory range." );
        return mEResult_False;
    }
    g_memcpy( a_pDest, m_pMemory, a_uSize );
    m_pMemory += a_uSize;
    m_uSize -= a_uSize;
    return mEResult_Ok;
}

mEResult mCRefStream::Read( mCString & a_strDest, MIUInt a_uSize )
{
    a_strDest.SetText( ( MILPCChar ) 0, a_uSize );
    return Read( a_strDest.AccessText(), a_uSize );
}

mEResult mCRefStream::Read( mCString & a_strDest )
{
    MILPChar pcDest = mCString::AccessStaticBuffer(), pcIt = pcDest;
    while ( m_uSize-- )
        if ( !( *pcIt++ = *reinterpret_cast< MIChar * >( m_pMemory++ ) ) )
            break;
    if ( m_uSize == -1 )
    {
        m_uSize = 0;
        *pcIt++ = 0;
    }
    a_strDest.SetText( pcDest, static_cast< MIUInt >( pcIt - pcDest ) - 1 );
    return mEResult_Ok;
}

mEResult mCRefStream::Write( MILPCVoid a_pSource, MIUInt a_uSize )
{
    if ( a_uSize > m_uSize )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Exceeded valid memory range." );
        return mEResult_False;
    }
    g_memcpy( m_pMemory, a_pSource, a_uSize );
    m_pMemory += a_uSize;
    m_uSize -= a_uSize;
    return mEResult_Ok;
}

mEResult mCRefStream::Write( mCString const & a_strSource )
{
    return Write( a_strSource.GetText(), a_strSource.GetLength() );
}

MIUInt mCRefStream::GetRemainingSize( void ) const
{
    return m_uSize;
}

mEResult mCRefStream::Skip( MIUInt a_uCount )
{
    if ( a_uCount > m_uSize )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Exceeded valid memory range." );
        return mEResult_False;
    }
    m_pMemory += a_uCount;
    m_uSize -= a_uCount;
    return mEResult_Ok;
}

mCRefStream::mCRefStream( mCRefStream const & )
{
}

mCRefStream::mCRefStream( void )
{
}

mCRefStream & mCRefStream::operator = ( mCRefStream const & )
{
    return *this;
}