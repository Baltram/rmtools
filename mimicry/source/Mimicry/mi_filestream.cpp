#include "mi_include_standard.h"
#include "mi_include_streams.h"

namespace
{
    enum
    {
        mEFileBufferSize = 8191
    };
}

mCFileStream::~mCFileStream( void )
{
    Close();
}

mCFileStream::mCFileStream( void )
{
    Clear();
}

mCFileStream::mCFileStream( mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode )
{
    Clear();
    Open( a_strFileName, a_enuOpenMode );
}

MIUInt mCFileStream::GetSize( void ) const
{
    return g_max( m_uRecentFileSize, ( m_uBufferedPosition + m_uOffset ) );
}

MIBool mCFileStream::IsAtEnd( void ) const
{
    return GetSize() <= Tell();
}

mEResult mCFileStream::Read( MILPVoid a_pDest, MIUInt a_uSize )
{
    if ( m_uOffset + a_uSize <= m_arrBuffer.GetCount() )
    {
        g_memcpy( a_pDest, ( m_arrBuffer.GetBuffer() + m_uOffset ), a_uSize );
        m_uOffset += a_uSize;
        return mEResult_Ok;
    }
    if ( !( m_enuOpenMode & mEFileOpenMode_Read ) )
    {
        MI_ERROR( mCStreamError, EFileNotOpen, "File is not open for reading operations." );
        return mEResult_False;
    }
    if ( ( Tell() + a_uSize ) > GetSize() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "File size exceeded when reading binary data." );
        return mEResult_False;
    }
    MIByte * pDest = static_cast< MIByte * >( a_pDest );
    for ( ; ; )
    {
        MIUInt const uSize = g_min( a_uSize, ( m_arrBuffer.GetCount() - m_uOffset ) );
        g_memcpy( pDest, ( m_arrBuffer.GetBuffer() + m_uOffset ), uSize );
        pDest += uSize;
        a_uSize -= uSize;
        m_uOffset += uSize;
        if ( !a_uSize )
            break;
        Buffer( Tell() );
    }
    return mEResult_Ok;
}

mEResult mCFileStream::Read( mCString & a_strDest, MIUInt a_uSize )
{
    a_strDest.SetText( static_cast< MILPCChar >( 0 ), a_uSize );
    return Read( a_strDest.AccessText(), a_uSize );
}

mEResult mCFileStream::Read( mCString & a_strDest )
{
    // Note: m_arrBuffer (a mCCharArray) is guaranteed to be 0-terminated.
    if ( !( m_enuOpenMode & mEFileOpenMode_Read ) )
    {
        MI_ERROR( mCStreamError, EFileNotOpen, "File is not open for reading operations." );
        return mEResult_False;
    }
    if ( Tell() == GetSize() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "File size exceeded when reading binary data." );
        return mEResult_False;
    }
    MILPCChar const pcText = m_arrBuffer.GetBuffer() + m_uOffset;
    MIUInt const uSize = static_cast< MIUInt >( g_strlen( pcText ) );
    a_strDest.SetText( pcText, uSize );
    m_uOffset += uSize;
    if ( m_uOffset == m_arrBuffer.GetCount() )
    {
        if ( Tell() == GetSize() )
            return mEResult_Ok;
        Buffer( Tell() );
        mCString strTemp;
        Read( strTemp );
        a_strDest.Append( strTemp );
    }
    else
    {
        m_uOffset += 1;
    }
    return mEResult_Ok;
}

mEResult mCFileStream::Seek( MIUInt a_uPosition, mEStreamSeekMode a_enuMode )
{
    if ( m_enuOpenMode == mEFileOpenMode_Invalid )
    {
        MI_ERROR( mCStreamError, EFileNotOpen, "File is not open." );
        return mEResult_False;
    }
    Flush();
    MIUInt uNewPosition;
    if ( a_enuMode == mEStreamSeekMode_Current )
    {
        uNewPosition = Tell() + a_uPosition;
    }
    else if ( a_enuMode == mEStreamSeekMode_End )
    {
        uNewPosition = GetSize() - a_uPosition;
    }
    else
    {
        uNewPosition = a_uPosition;
    }
    if ( uNewPosition > GetSize() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "File size exceeded when seeking." );
        return mEResult_False;
    }
    if ( ( uNewPosition < m_uBufferedPosition ) || ( uNewPosition > ( m_uBufferedPosition + m_arrBuffer.GetCount() ) ) )
    {
        Buffer( uNewPosition );
    }
    else
    {
        m_uOffset = ( uNewPosition - m_uBufferedPosition );
    }
    return mEResult_Ok; 
}

MIUInt mCFileStream::Tell( void ) const
{
    return ( m_uBufferedPosition + m_uOffset );
}

mEResult mCFileStream::Write( MILPCVoid a_pSource, MIUInt a_uSize )
{
    if ( m_bPendingData )
    {
        if ( ( m_uOffset + a_uSize ) <= mEFileBufferSize )
        {
            m_arrBuffer.SetAt( m_uOffset, static_cast< MILPCChar >( a_pSource ), a_uSize );
            m_uOffset += a_uSize;
            return mEResult_Ok;
        }
    }
    if ( !( m_enuOpenMode & mEFileOpenMode_Write ) )
    {
        MI_ERROR( mCStreamError, EFileNotOpen, "File is not open for writing operations." );
        return mEResult_False;
    }
    MILPCChar pSource = static_cast< MILPCChar >( a_pSource );
    for ( ; ; )
    {
        if ( !m_bPendingData )
        {
            m_uOffsetPending = m_uOffset;
            m_bPendingData = MITrue;
        }
        MIUInt const uSize = g_min( a_uSize, ( mEFileBufferSize - m_uOffset ) );
        m_arrBuffer.SetAt( m_uOffset, pSource, uSize );
        m_uOffset += uSize;
        pSource += uSize;
        a_uSize -= uSize;
        if ( !a_uSize )
            break;
        Buffer( Tell() );
    }
    return mEResult_Ok;
}

mEResult mCFileStream::Write( mCString const & a_strSource )
{
    return Write( a_strSource.GetText(), a_strSource.GetLength() );
}

void mCFileStream::Buffer( MIUInt a_uPosition )
{
    // Padding: Room for reading before a_uPosition or writing over end of file.
    enum
    {
        EPadding = mEFileBufferSize / 3
    };
    Flush();
    MIBool const bCanRead             = ( m_enuOpenMode & mEFileOpenMode_Read ? MITrue : MIFalse );
    MIBool const bCanWrite            = ( m_enuOpenMode & mEFileOpenMode_Write ? MITrue : MIFalse );
    MIUInt const uReadPadding         = bCanRead ? EPadding : 0 ;
    MIUInt const uWritePadding        = bCanRead ? ( bCanWrite ? EPadding : 0 ) : mEFileBufferSize;
    MIUInt const uMinBufferSize       = g_min( ( mEFileBufferSize - uWritePadding ), GetSize() );
    MIUInt const uMaxBufferedPosition = GetSize() - uMinBufferSize;
    MIUInt const uOldBufferedPosition = m_uBufferedPosition;
    if ( a_uPosition <= uReadPadding )
    {
        m_uBufferedPosition = 0;
    }
    else
    {
        m_uBufferedPosition = g_min( ( a_uPosition - uReadPadding ), uMaxBufferedPosition );
    }
    m_uOffset = a_uPosition - m_uBufferedPosition;
    if ( !bCanRead )
        return;
    MIUInt const uNewBufferSize = g_min< MIUInt >( mEFileBufferSize, ( GetSize() - m_uBufferedPosition ) );
    if ( !m_arrBuffer.GetCount() )
    {
        m_arrBuffer.Resize( uNewBufferSize );
        DirectRead( m_arrBuffer.AccessBuffer(), m_uBufferedPosition, uNewBufferSize );
        return;
    }
    m_arrBuffer.Resize( mEFileBufferSize );
    MILPChar const pData = m_arrBuffer.AccessBuffer();
    MIInt const iDifference = g_limit< MIInt >( ( m_uBufferedPosition - uOldBufferedPosition ), -mEFileBufferSize, mEFileBufferSize );
    if ( iDifference >= 0 )
    {
        MIUInt uSizeRetainable = ( mEFileBufferSize - iDifference );
        g_memmove( pData, ( pData + iDifference ), uSizeRetainable );
        DirectRead( ( pData + uSizeRetainable ), ( m_uBufferedPosition + uSizeRetainable ), iDifference );
    }
    else
    {
        MIUInt uSizeRetainable = ( mEFileBufferSize + iDifference );
        g_memmove( ( pData - iDifference ), pData, uSizeRetainable );
        DirectRead( pData, m_uBufferedPosition, -iDifference );
    }
    m_arrBuffer.Resize( uNewBufferSize );
}

void mCFileStream::Close( void )
{
    if ( m_enuOpenMode == mEFileOpenMode_Invalid )
        return;
    Flush();
    fclose( static_cast< FILE * >( m_pFile ) );
    Clear();
}

void mCFileStream::Flush( void )
{
    if ( !m_bPendingData )
        return;
    m_uRecentFileSize = GetSize();
    FILE * const pFile = static_cast< FILE * >( m_pFile );
    g_fseek( pFile, ( m_uBufferedPosition + m_uOffsetPending ), SEEK_SET );
    fwrite( ( m_arrBuffer.GetBuffer() + m_uOffsetPending ), sizeof( MIChar ), ( m_uOffset - m_uOffsetPending ), pFile );
    fflush( pFile );
    m_bPendingData = MIFalse;
}

mCString const & mCFileStream::GetFileName( void )
{
    return m_strFileName;
}

MIBool mCFileStream::IsOpen( void )
{
    return ( m_enuOpenMode != mEFileOpenMode_Invalid );
}

mEResult mCFileStream::Open( mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode )
{
    if ( a_enuOpenMode == mEFileOpenMode_Invalid )
        return mEResult_Ok; 
    MILPVoid pFile;
    if ( a_enuOpenMode == mEFileOpenMode_ReadWrite )
    {
        MI_CRT_NO_WARNINGS( pFile = fopen( a_strFileName.GetText(), "rb+" ); )
        if ( !pFile )
            MI_CRT_NO_WARNINGS( pFile = fopen( a_strFileName.GetText(), "wb+" ); )
    }
    else
    {
        MI_CRT_NO_WARNINGS( pFile = fopen( a_strFileName.GetText(), ( a_enuOpenMode == mEFileOpenMode_Read ? "rb" : "wb" ) ); )
    }
    if ( !pFile )
    {
        MI_ERROR( mCStreamError, 
                  ECantOpenFile, 
                  ( "Can't open the file \"" + a_strFileName + ( a_enuOpenMode == mEFileOpenMode_Read ? "\" for reading." : "\" for writing." ) ).GetText() );
        return mEResult_False;
    }
    Init( pFile, a_strFileName, a_enuOpenMode );
    if ( m_enuOpenMode == mEFileOpenMode_Invalid )
        return mEResult_False;
    return mEResult_Ok;
}

void mCFileStream::DirectRead( MILPVoid a_pDest, MIUInt a_uPosition, MIUInt a_uSize )
{
    FILE * const pFile = static_cast< FILE * >( m_pFile );
    g_fseek( pFile, a_uPosition, SEEK_SET );
    fread( a_pDest, sizeof( MIChar ), a_uSize, pFile );
}

void mCFileStream::Init( MILPVoid a_pFile, mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode )
{
    FILE * const pFile = static_cast< FILE * >( a_pFile );
    g_fseek( pFile, 0, SEEK_END );
    Clear();
    m_pFile = pFile;
    m_uRecentFileSize = g_ftell( pFile );
    m_enuOpenMode = a_enuOpenMode;
    m_strFileName = a_strFileName;
    Buffer( 0 );
}

mCFileStream::mCFileStream( mCFileStream const & )  // Not used.
{
}

mCFileStream & mCFileStream::operator = ( mCFileStream const & )  // Not used.
{
    return *this;
}

mEResult mCFileStream::ReadFormatted( MILPVoid, MILPCChar )  // Not used.
{
    return mEResult_Ok;
}

mEResult mCFileStream::WriteFormatted( MILPCVoid, MILPCChar )  // Not used.
{
    return mEResult_Ok;
}

void mCFileStream::Clear( void )
{
    m_arrBuffer.Reserve( 1 );  // AccessBuffer() must not return 0.
    m_arrBuffer.Resize( 0 );
    m_strFileName = "";
    m_enuOpenMode = mEFileOpenMode_Invalid;
    m_pFile = 0;
    m_uRecentFileSize = 0;
    m_uBufferedPosition = 0;
    m_uOffset = 0;
    m_uOffsetPending = 0;
    m_bPendingData = MIFalse;
}