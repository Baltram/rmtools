#include "mi_include_standard.h"
#include "mi_include_streams.h"

namespace
{
    MILPCChar GetWhiteSpaceIndicator( void )
    {
        static MIChar s_pcWhiteSpaceIndicator[ 256 ] = "";
        if ( !*s_pcWhiteSpaceIndicator )
        {
            g_memset( s_pcWhiteSpaceIndicator, 1, 256 );
            s_pcWhiteSpaceIndicator[ '\f' ] = 0;
            s_pcWhiteSpaceIndicator[ '\n' ] = 0;
            s_pcWhiteSpaceIndicator[ '\r' ] = 0;
            s_pcWhiteSpaceIndicator[ '\t' ] = 0;
            s_pcWhiteSpaceIndicator[ '\v' ] = 0;
            s_pcWhiteSpaceIndicator[ ' ' ] = 0;
        }
        return s_pcWhiteSpaceIndicator;
    }

    inline
    void SkipWhiteSpaceCharacters( MILPChar & pcIt, MILPCChar a_pcWhiteSpaceIndicator )
    {
        while ( !a_pcWhiteSpaceIndicator[ *pcIt ] )
            ++pcIt;
    }

    inline
    void SkipNonWhiteSpaceCharacters( MILPChar & pcIt, MILPCChar a_pcWhiteSpaceIndicator )
    {
        *const_cast< MILPChar >( a_pcWhiteSpaceIndicator ) = 0;
        while ( a_pcWhiteSpaceIndicator[ *pcIt ] )
            ++pcIt;
        *const_cast< MILPChar >( a_pcWhiteSpaceIndicator ) = 1;
    }

    inline
    void DetectString( MILPChar & pcBegin, MILPChar & pcEnd, MILPCChar a_pcWhiteSpaceIndicator )
    {
        SkipWhiteSpaceCharacters( pcBegin, a_pcWhiteSpaceIndicator );
        pcEnd = pcBegin;
        SkipNonWhiteSpaceCharacters( pcEnd, a_pcWhiteSpaceIndicator );
    }
}

mCStringStream::~mCStringStream( void )
{
}

mCStringStream::mCStringStream( void )
{
}

mCStringStream::mCStringStream( mCStringStream const & a_Source ) :
    mCMemoryStreamBaseFormatted( a_Source )
{
}

mCStringStream::mCStringStream( mCIOStreamFormatted & a_Source ) :
    mCMemoryStreamBaseFormatted( a_Source )
{
}

mCStringStream::mCStringStream( mCString const & a_strSource )
{
    Write( a_strSource );
    Seek( 0 );
}

mCStringStream & mCStringStream::operator = ( mCStringStream const & a_Source )
{
    if ( &a_Source != this )
        mCMemoryStreamBaseFormatted::operator = ( a_Source );
    return *this;
}

MIBool mCStringStream::IsAtEnd( void ) const
{
    static MILPCChar const s_pcWhiteSpaceIndicator = GetWhiteSpaceIndicator();
    MILPChar pcBegin = const_cast< MILPChar >( m_arrBuffer.GetBuffer() + m_uPosition ), pcEnd;
    DetectString( pcBegin, pcEnd, s_pcWhiteSpaceIndicator );
    return pcBegin == pcEnd;
}

mEResult mCStringStream::Read( mCString & a_strDest )
{
    static MILPCChar const s_pcWhiteSpaceIndicator = GetWhiteSpaceIndicator();

    MILPChar const pcBuffer = m_arrBuffer.AccessBuffer();
    MILPChar pcBegin = pcBuffer + m_uPosition;
    MILPChar pcEnd;

    DetectString( pcBegin, pcEnd, s_pcWhiteSpaceIndicator );
    m_uPosition = static_cast< MIUInt >( pcEnd - pcBuffer );
    a_strDest.SetText( pcBegin, static_cast< MIUInt >( pcEnd - pcBegin ) );
    if ( *pcBegin )
        return mEResult_Ok;

    if ( m_uPosition == m_arrBuffer.GetCount() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when reading formatted data." );
    }
    else
    {
        MI_ERROR( mCStreamError, EBadFormat, "Stream data doesn't fit the requested format." );
    }
    return mEResult_False;
}

mCString mCStringStream::ReadLine( void )
{
    MILPCChar const pcBeginLine = m_arrBuffer.GetBuffer() + m_uPosition;
    MILPCChar pcEndLine = strpbrk( pcBeginLine, "\r\n" );
    if ( !pcEndLine )
    {
        pcEndLine = m_arrBuffer.GetBuffer() + m_arrBuffer.GetCount();
        if ( pcEndLine == pcBeginLine )
            MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when reading formatted data." );
    }
    MIUInt const uLength = static_cast< MIUInt >( pcEndLine - pcBeginLine );
    if ( *pcEndLine == '\r' )
        ++pcEndLine, ++m_uPosition;
    if ( *pcEndLine == '\n' )
        ++m_uPosition;
    m_uPosition += uLength;
    return mCString( pcBeginLine, uLength );
}

mEResult mCStringStream::ReadStringInQuotes( mCString & a_strDest )
{
    MILPChar const pcBuffer = m_arrBuffer.AccessBuffer();
    MILPChar pcBegin = pcBuffer + m_uPosition;
    MILPChar pcEnd;
    SkipWhiteSpaceCharacters( pcBegin, GetWhiteSpaceIndicator() );
    for ( ; ; )
    {
        if ( *pcBegin != '\"' )
            break;
        for ( pcEnd = pcBegin + 1; *pcEnd && ( *pcEnd != '\"' ); ++pcEnd );
        if ( !*pcEnd )
            break;
        a_strDest.SetText( pcBegin + 1, static_cast< MIUInt >( pcEnd - pcBegin - 1 ) );
        m_uPosition = static_cast< MIUInt >( pcEnd - pcBuffer + 1 );
        return mEResult_Ok;
    }
    MI_ERROR( mCStreamError, EBadFormat, "Enclosing quotes are missing." );
    return mEResult_False;

}

void mCStringStream::ToString( mCString & a_strDest )
{
    a_strDest.SetText( m_arrBuffer.GetBuffer(), m_arrBuffer.GetCount() );
}

void mCStringStream::WriteLine( mCString const & a_strText )
{
    Write( a_strText );
    Write( "\r\n", 2 );
}

mEResult mCStringStream::ReadFormatted( MILPVoid a_pDest, MILPCChar a_Format )
{    
    static MILPCChar const s_pcWhiteSpaceIndicator = GetWhiteSpaceIndicator();

    MILPChar const pcBuffer = m_arrBuffer.AccessBuffer();
    MILPChar pcIt = pcBuffer + m_uPosition;
    MILPChar pcEnd;

    for ( ; ; )
    {
        a_Format += 3;
        if ( *a_Format == 'c' )
        {
            SkipWhiteSpaceCharacters( pcIt, s_pcWhiteSpaceIndicator );
            *static_cast< MILPChar >( a_pDest ) = *pcIt;
            if ( *pcIt )
                ++pcIt;
            else
                break;
        }
        else
        {
            DetectString( pcIt, pcEnd, s_pcWhiteSpaceIndicator );
            MILPChar const pcBegin = pcIt;
            MIChar const cTemp = *pcEnd;
            *pcEnd = 0;
            if ( *a_Format == 'f' )
            {
                *static_cast< MIDouble * >( a_pDest ) = strtod( pcIt, &pcIt );
            }
            else if ( *a_Format == 'i' )
            {
                *static_cast< MIInt * >( a_pDest ) = strtol( pcIt, &pcIt, 0 );
            }
            else
            {
                *static_cast< MIUInt * >( a_pDest ) = strtoul( pcIt, &pcIt, 0 );
            }
            *pcEnd = cTemp;
            if ( pcIt == pcBegin )
                break;
        }
        m_uPosition = static_cast< MIUInt >( pcIt - pcBuffer );
        return mEResult_Ok;
    }

    if ( m_uPosition == m_arrBuffer.GetCount() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when reading formatted data." );
        return mEResult_False;
    }
    MI_ERROR( mCStreamError, EBadFormat, "Stream data doesn't fit the requested format." );
    return mEResult_False;
}

mEResult mCStringStream::WriteFormatted( MILPCVoid a_pSource, MILPCChar a_Format )
{
    static MIChar s_pcBuffer[ 20 ];
    MIUInt uSize;
    if ( a_Format[ 1 ] == 'u' )
    {
        MI_CRT_NO_WARNINGS( uSize = sprintf( s_pcBuffer, a_Format, *static_cast< MIUInt const * >( a_pSource ) ); )
    }
    else if ( a_Format[ 1 ] == 'i' )
    {
        MI_CRT_NO_WARNINGS( uSize = sprintf( s_pcBuffer, a_Format, *static_cast< MIInt const * >( a_pSource ) ); )
    }
    else
    {
        MI_CRT_NO_WARNINGS( uSize = sprintf( s_pcBuffer, a_Format, *static_cast< MIDouble const * >( a_pSource ) ); )
    }
    Write( s_pcBuffer, uSize );
    return mEResult_Ok;
}
