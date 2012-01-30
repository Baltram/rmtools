#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_standard.h"
#include "mi_include_streams.h"

template< mEStreamType M >
mTMemoryStreamBase< M >::~mTMemoryStreamBase( void )
{
}

template< mEStreamType M >
mTMemoryStreamBase< M >::mTMemoryStreamBase( void ) :
    m_uPosition( 0 )
{
}

template< mEStreamType M >
mTMemoryStreamBase< M >::mTMemoryStreamBase( mTMemoryStreamBase< M > const & a_Source ) :
    m_arrBuffer( a_Source.m_arrBuffer ),
    m_uPosition( a_Source.m_uPosition )
{
}

template< mEStreamType M >
mTMemoryStreamBase< M >::mTMemoryStreamBase( mCIOStreamBinary & a_Source ) :
    m_uPosition( 0 )
{
    MIUInt const uSize = a_Source.GetSize();
    MIUInt const uPosition = a_Source.Tell();
    a_Source.Seek( 0 );
    m_arrBuffer.Resize( uSize );
    a_Source.Read( m_arrBuffer.AccessBuffer(), uSize );
    a_Source.Seek( uPosition );
}

template< mEStreamType M >
mTMemoryStreamBase< M >::mTMemoryStreamBase( mCIOStreamFormatted & a_Source ) :
    m_uPosition( 0 )
{
    MIUInt const uSize = a_Source.GetSize();
    MIUInt const uPosition = a_Source.Tell();
    a_Source.Seek( 0 );
    m_arrBuffer.Resize( uSize );
    a_Source.Read( m_arrBuffer.AccessBuffer(), uSize );
    a_Source.Seek( uPosition );
}

template< mEStreamType M >
mTMemoryStreamBase< M > & mTMemoryStreamBase< M >::operator = ( mTMemoryStreamBase< M > const & a_Source )
{
    if ( &a_Source != this )
    {
        m_arrBuffer = a_Source.m_arrBuffer;
        m_uPosition = a_Source.m_uPosition;
    }
    return *this;
}

template< mEStreamType M >
MIUInt mTMemoryStreamBase< M >::GetSize( void ) const
{
    return m_arrBuffer.GetCount();
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::Read( MILPVoid a_pDest, MIUInt a_uSize )
{
    if ( ( m_uPosition + a_uSize ) > GetSize() )
    {
        m_uPosition = GetSize();
        MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when reading binary data." );
        return mEResult_False;
    }
    g_memcpy( a_pDest, ( m_arrBuffer.GetBuffer() + m_uPosition ), a_uSize );
    m_uPosition += a_uSize;
    return mEResult_Ok;
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::Read( mCString & a_strDest, MIUInt a_uSize )
{
    a_strDest.SetText( static_cast< MILPCChar >( 0 ), a_uSize );
    return Read( a_strDest.AccessText(), a_uSize );
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::Seek( MIUInt a_uPosition, mEStreamSeekMode a_enuMode )
{
    if ( a_enuMode == mEStreamSeekMode_Current )
    {
        m_uPosition += a_uPosition;
    }
    else if ( a_enuMode == mEStreamSeekMode_End )
    {
        m_uPosition = m_arrBuffer.GetCount() - a_uPosition;
    }
    else
    {
        m_uPosition = a_uPosition;
    }
    if ( m_uPosition <= m_arrBuffer.GetCount() )
        return mEResult_Ok;

    m_uPosition = m_arrBuffer.GetCount();
    MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when seeking." );
    return mEResult_False;
}

template< mEStreamType M >
MIUInt mTMemoryStreamBase< M >::Tell( void ) const
{
    return m_uPosition;
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::Write( MILPCVoid a_pSource, MIUInt a_uSize )
{
    m_arrBuffer.SetAt( m_uPosition, static_cast< MILPCChar >( a_pSource ), a_uSize );
    m_uPosition += a_uSize;
    return mEResult_Ok;
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::Write( mCString const & a_strSource )
{
    return Write( a_strSource.GetText(), a_strSource.GetLength() );
}

template< mEStreamType M >
void mTMemoryStreamBase< M >::Clear( void )
{
    m_arrBuffer.Clear();
    m_uPosition = 0;
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::FromFile( mCString a_strFileName )
{
    MI_CRT_NO_WARNINGS( FILE * pSourceFile = fopen( a_strFileName.GetText(), "rb" ); )
    if ( !pSourceFile )
    {
        MI_ERROR( mCStreamError, ECantOpenFile, ( "Cannot open the file \"" + a_strFileName + "\" for reading." ).GetText() );
        return mEResult_False;
    }

    Clear();
    fseek( pSourceFile, 0, SEEK_END );
    MIUInt const uSize = ftell( pSourceFile );
    fseek( pSourceFile, 0, SEEK_SET );

    MIByte * pTempBuffer = new MIByte [ uSize ];
    fread( pTempBuffer, sizeof( MIByte ), uSize, pSourceFile );
    Write( pTempBuffer, uSize );
    delete [] pTempBuffer;

    fclose( pSourceFile );
    Seek( 0 );
    return mEResult_Ok;
}

template< mEStreamType M >
mEResult mTMemoryStreamBase< M >::ToFile( mCString a_strFileName )
{
    MI_CRT_NO_WARNINGS( FILE * pDestFile = fopen( a_strFileName.GetText(), "wb" ); )
    if ( !pDestFile )
    {
        MI_ERROR( mCStreamError, ECantOpenFile, ( "Cannot open the file \"" + a_strFileName + "\" for writing." ).GetText() );
        return mEResult_False;
    }

    MIUInt const uSize = GetSize();
    MIUInt const uPosition = Tell();
    Seek( 0 );

    MIByte * pTempBuffer = new MIByte [ uSize ];
    Read( pTempBuffer, uSize );
    fwrite( pTempBuffer, sizeof( MIByte ), uSize, pDestFile );
    delete [] pTempBuffer;

    fclose( pDestFile );
    Seek( uPosition );
    return mEResult_Ok;
}

template< mEStreamType M >
void mTMemoryStreamBase< M >::Swap( mTMemoryStreamBase< M > & a_Other )
{
    m_arrBuffer.Swap( a_Other.m_arrBuffer );
    g_swap( m_uPosition, a_Other.m_uPosition );
}

template class mTMemoryStreamBase< mEStreamType_Binary >;
template class mTMemoryStreamBase< mEStreamType_Formatted >;

#endif