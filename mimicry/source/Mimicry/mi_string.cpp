#include "mi_include_standard.h"
#include "mi_include_string.h"

MIUInt mCString::s_uBufferSize = EMinStaticBufferSize;
MILPChar mCString::s_pcBuffer = static_cast< MILPChar >( malloc( EMinStaticBufferSize ) );

mCString::mCString( MIUInt a_uCount ) :
    m_pcText( Alloc( 0, a_uCount ) )
{
}

mCString::mCString( MILPCChar a_pcText ) :
    m_pcText( Alloc( a_pcText ) )
{
}

mCString::mCString( MILPCChar a_pcText, MIUInt a_uCount ) :
    m_pcText( Alloc( a_pcText, a_uCount ) )
{
}

mCString::mCString( MIChar a_cChar, MIUInt a_uCount ) :
    m_pcText( 0 )
{
    SetText( a_cChar, a_uCount );
}

mCString::mCString( MILPCChar a_pcText1, MILPCChar a_pcText2 ) :
    m_pcText( Alloc( a_pcText1, a_pcText2 ) )
{
}

mCString::mCString( mCString const & a_strText ) :
    m_pcText( Alloc( a_strText.m_pcText ) )
{
}

mCString::mCString( void ) :
    m_pcText( Alloc( 0, 0u ) )
{
}

mCString::~mCString( void )
{
    free( m_pcText );
}

mCString & mCString::operator = ( mCString const & a_strText )
{
    if ( this == &a_strText )
        return *this;
    SetText( a_strText );
    return *this;
}

mCString & mCString::operator = ( MIChar a_cChar )
{
    SetText( &a_cChar, 1 );
    return *this;
}

mCString & mCString::operator = ( MILPCChar a_pcText )
{
    SetText( a_pcText );
    return *this;
}

MIBool mCString::operator == ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) == 0 );
}

MIBool mCString::operator == ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) == 0 );
}

MIBool mCString::operator != ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) != 0 );
}

MIBool mCString::operator != ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) != 0 );
}

MIChar & mCString::operator [] ( MIInt a_uIndex )
{
    return m_pcText[ a_uIndex ];
}

MIChar mCString::operator [] ( MIInt a_uIndex ) const
{
    return m_pcText[ a_uIndex ];
}

mCString mCString::operator + ( mCString const & a_strText ) const
{
    return mCString( m_pcText, a_strText.m_pcText );
}

mCString mCString::operator + ( MIChar a_cChar ) const
{
    return mCString( m_pcText, Stringize( a_cChar ) );
}

mCString mCString::operator + ( MILPCChar a_pcText ) const
{
    return mCString( m_pcText, a_pcText );
}

MIBool mCString::operator < ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) == -1 );
}

MIBool mCString::operator < ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) == -1 );
}

MIBool mCString::operator <= ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) != 1 );
}

MIBool mCString::operator <= ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) != 1 );
}

MIBool mCString::operator > ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) == 1 );
}

MIBool mCString::operator > ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) == 1 );
}

MIBool mCString::operator >= ( mCString const & a_strOther ) const
{
    return ( Compare( a_strOther ) != -1 );
}

MIBool mCString::operator >= ( MILPCChar a_pcOther ) const
{
    return ( Compare( a_pcOther ) != -1 );
}

mCString & mCString::operator += ( mCString const & a_strText )
{
    Append( a_strText );
    return *this;
}

mCString & mCString::operator += ( MIChar a_cChar )
{
    Append( Stringize( a_cChar ) );
    return *this;
}

mCString & mCString::operator += ( MILPCChar a_pcText )
{
    Append( a_pcText );
    return *this;
}

MILPChar mCString::AccessText( void )
{
    return m_pcText;
}

mCString & mCString::Append( mCString const & a_strText )
{
    return Append( a_strText.m_pcText );
} 

mCString & mCString::Append( MILPCChar a_pcText )
{
    mCString strNew( m_pcText, a_pcText );
    Swap( strNew );
    return *this;
}

void mCString::Clear( void )
{
    SetText( "" );
}

MIInt mCString::Compare( mCString const & a_strOther ) const
{
    return Compare( a_strOther.m_pcText );
}

MIInt mCString::Compare( MILPCChar a_pcOther ) const
{
    return g_strcmp( m_pcText, a_pcOther );
}

MIInt mCString::CompareNoCase( mCString const & a_strOther ) const
{
    return CompareNoCase( a_strOther.m_pcText );
}

MIInt mCString::CompareNoCase( MILPCChar a_pcOther ) const
{
    MILPCChar pcText = m_pcText;
    do
    {
        MIInt iDiff = toupper( *pcText ) - toupper( *a_pcOther );
        if ( iDiff )
            return ( iDiff > 0 ? 1 : -1 );
    }
    while ( *pcText++ && *a_pcOther++ );
    return 0;
}

MIUInt mCString::Count( MILPCChar a_pcText ) const
{
    MIUInt uResult = 0;
    MIUInt uSeekStringLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MILPCChar pcText = m_pcText;
    MILPCChar pcEndText = ( pcText-- ) + GetLength();
    while ( NextOf( ( pcText += uSeekStringLength ), pcEndText, a_pcText, uSeekStringLength ) )
        ++uResult;
    return uResult;
}

MIUInt mCString::Count( MIChar a_cChar ) const
{
    MIUInt uResult = 0;
    MILPCChar pcText = m_pcText;
    MILPCChar pcEndText = ( pcText-- ) + GetLength();
    while ( NextOf( ++pcText, pcEndText, a_cChar ) )
        ++uResult;
    return uResult;
}

MIUInt mCString::FirstOf( MILPCChar a_pcText ) const
{
    MILPCChar pcText = m_pcText;
    MILPCChar pcOccurance = mCString::NextOf( pcText, ( m_pcText + GetLength() ), a_pcText, static_cast< MIUInt >( g_strlen( a_pcText ) ) );
    return ( pcOccurance ? static_cast< MIUInt >( pcOccurance - m_pcText ) : MI_DW_INVALID );
}

MIUInt mCString::FirstOf( MIChar a_cChar ) const
{
    MILPCChar pcText = m_pcText;
    MILPCChar pcOccurance = mCString::NextOf( pcText, ( m_pcText + GetLength() ), a_cChar );
    return ( pcOccurance ? static_cast< MIUInt >( pcOccurance - m_pcText ) : MI_DW_INVALID );
}

mCString & mCString::Format( MILPCChar a_pcFormat, ... )
{
    va_list argp;
    va_start( argp, a_pcFormat );
    VFormat( a_pcFormat, argp );
    va_end( argp );
    return *this;
}

MIUInt mCString::GetLength( void ) const
{
    return static_cast< MIUInt >( g_strlen( m_pcText ) );
}

MILPCChar mCString::GetText( void ) const
{
    return m_pcText;
}

MIUInt mCString::LastOf( MILPCChar a_pcText ) const
{
    MIUInt uSeekStringLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    for ( MILPCChar pcText = g_max( ( m_pcText + GetLength() - uSeekStringLength ), m_pcText ); pcText != m_pcText; --pcText )
    {
        if ( *pcText == *a_pcText )
            if ( !g_strncmp( pcText, a_pcText, uSeekStringLength ) )
                return static_cast< MIUInt >( pcText - m_pcText );
    }
    return MI_DW_INVALID;
}

MIUInt mCString::LastOf( MIChar a_cChar ) const
{
    for ( MILPCChar pcText = g_max( ( m_pcText + GetLength() - 1 ), m_pcText ); pcText != m_pcText; --pcText )
    {
        if ( *pcText == a_cChar )
            return static_cast< MIUInt >( pcText - m_pcText );
    }
    return MI_DW_INVALID;
}

mCString mCString::Left( MIUInt a_uCount ) const
{
    return mCString( m_pcText, a_uCount );
}

mCString & mCString::Replace( MILPCChar a_pcText, MILPCChar a_pcNewText )
{
    MILPCChar pcText           = m_pcText;
    MILPCChar pcEndText        = pcText + GetLength();
    MIUInt    uStringLength    = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MIUInt    uNewStringLength = static_cast< MIUInt >( g_strlen( a_pcNewText ) );

    if ( MIInt uMaxBufferOverhead = g_max( static_cast< MIInt >( ( ( pcEndText - pcText ) / uStringLength * sizeof( MILPCChar ) ) - s_uBufferSize ), 0 ) )
    {
        s_pcBuffer = static_cast< MILPChar >( realloc( s_pcBuffer, ( s_uBufferSize += uMaxBufferOverhead ) ) );
    }

    MILPCChar * ppcOccurrences = reinterpret_cast< MILPCChar * >( s_pcBuffer ) - 1;
    while ( *( ++ppcOccurrences ) = NextOf( pcText, pcEndText, a_pcText, uStringLength ) )
        ++pcText;
    pcText = m_pcText;
    
    MIInt iSizeIncrease = ( uNewStringLength - uStringLength ) * ( ppcOccurrences - reinterpret_cast< MILPCChar * >( s_pcBuffer ) );
    ppcOccurrences = reinterpret_cast< MILPCChar * >( s_pcBuffer ) - 1;
    MILPChar pcNewText = Alloc( 0, static_cast< MIUInt >( ( pcEndText - pcText ) + iSizeIncrease ) );
    MILPChar pcNewTextIt = pcNewText;
    while ( *( ++ppcOccurrences ) )
    {
        MIUInt uLength = static_cast< MIUInt >( *ppcOccurrences - pcText );
        g_memcpy( pcNewTextIt, pcText, uLength );
        pcText += ( uLength + uStringLength );
        g_memcpy( ( pcNewTextIt += uLength ), a_pcNewText, uNewStringLength );
        pcNewTextIt += uNewStringLength;
    }
    g_memcpy( pcNewTextIt, pcText, ( pcEndText - pcText ) );

    free( m_pcText );
    m_pcText = pcNewText;
    return *this;
}

mCString & mCString::Replace( MIChar a_cChar, MIChar a_cNewChar )
{
    MILPChar pcText = m_pcText;
    MILPCChar pcEndText = pcText + GetLength();
    while ( NextOf( *const_cast< MILPCChar * >( &pcText ), pcEndText, a_cChar ) )
        *( pcText++ ) = a_cNewChar;
    return *this;
}

mCString mCString::Right( MIUInt a_uCount ) const
{
    return mCString( ( m_pcText + GetLength() - a_uCount ), a_uCount );
}

MIInt mCString::Scan( MILPCChar a_pcFormat, ... ) const
{
    va_list argp;
    va_start( argp, a_pcFormat );
    MIU32 uCount = 0;
    MILPCChar pcFormat = a_pcFormat;
    MILPCChar pcEndFormat = a_pcFormat + g_strlen( a_pcFormat );
    MIInt ( MI_CDECL * funcSscanf )( MILPCChar, MILPCChar, ... );
    MI_CRT_NO_WARNINGS( funcSscanf = &sscanf; )
    while ( NextOf( pcFormat, pcEndFormat, '%' ) )
    {
        ++pcFormat;
        if ( ( *pcFormat != '%' ) && ( *pcFormat != '*' ) )
            ++uCount;
        else
            ++pcFormat;
    }
    if ( uCount )
    {
        MIU32 ** pLastArg = &va_arg( argp, MIU32 * ) + ( uCount - 1 );
        MI_ASSERT_32_BIT
        MI_ASSERT_MSVC
        MI_ASM {
            mov  esi, esp
            mov  eax, uCount
            mov  edx, pLastArg
label_loop:
            push [ edx ]
            sub  edx, 4
            sub  eax, 1
            test eax, eax
            jnz  label_loop
            push a_pcFormat
            mov  eax, this
            push [ eax ]
            call funcSscanf
            mov  uCount, eax
            mov  esp, esi
        }
    }
    va_end( argp );
    return uCount;
}

void mCString::SetText( mCString const & a_strText )
{
    if ( &a_strText != this )
        m_pcText = Alloc( a_strText.m_pcText, static_cast< MIUInt >( 0 ), m_pcText );
}

void mCString::SetText( MILPCChar a_pcText )
{
    m_pcText = Alloc( a_pcText, static_cast< MIUInt >( 0 ), m_pcText );
}

void mCString::SetText( MILPCChar a_pcText, MIUInt a_uCount )
{
    m_pcText = Alloc( a_pcText, a_uCount, m_pcText );
}

void mCString::SetText( MIChar a_cChar, MIUInt a_uCount )
{
    m_pcText = Alloc( 0, a_uCount, m_pcText );
    for ( MIUInt i = 0; i != a_uCount; ++i )
        m_pcText[ i ] = a_cChar;
}

void mCString::Swap( mCString & a_strOther )
{
    if ( this == &a_strOther )
        return;
    g_swap( m_pcText, a_strOther.m_pcText );
}

mCString & mCString::ToLower( void )
{
    MILPChar pcText = m_pcText;
    while ( *pcText )
        *pcText++ = static_cast< MIChar >( tolower( *pcText ) );
    return *this;
}

mCString & mCString::ToUpper( void )
{
    MILPChar pcText = m_pcText;
    while ( *pcText )
        *pcText++ = static_cast< MIChar >( toupper( *pcText ) );
    return *this;
}

mCString & mCString::TrimLeft( MIChar a_cChar )
{
    if ( a_cChar == 0 )
        return *this;
    MILPChar pcText = m_pcText;
    while ( *pcText == a_cChar )
        ++pcText;
    MIUInt uCount = static_cast< MIUInt >( pcText - m_pcText );
    if ( uCount )
        TrimLeft( uCount );
    return *this;
}

mCString & mCString::TrimLeft( MIUInt a_uCount )
{
    mCString strNew( ( m_pcText + a_uCount ), ( GetLength() - a_uCount ) );
    Swap( strNew );
    return *this;
}

mCString & mCString::TrimRight( MIChar a_cChar )
{
    if ( a_cChar == 0 )
        return *this;
    MIUInt uLength = GetLength();
    MILPChar pcText = m_pcText + uLength;
    while ( pcText-- != m_pcText )
        if ( *pcText != a_cChar )
            break;
    MIUInt uCount = uLength - ( pcText - m_pcText + 1 );
    if ( uCount )
        TrimRight( uCount );
    return *this;
}

mCString & mCString::TrimRight( MIUInt a_uCount )
{
    m_pcText[ GetLength() - a_uCount ] = 0;
    return *this;
}

mCString & mCString::VFormat( MILPCChar a_pcFormat, va_list a_Arguments )
{
    MI_CRT_NO_WARNINGS( vsprintf( s_pcBuffer, a_pcFormat, a_Arguments ); )
    m_pcText = Alloc( s_pcBuffer );
    return *this;
}

MILPChar mCString::Alloc( MILPCChar a_pcText, MIUInt a_uCount, MILPVoid a_Memory )
{
    if ( a_pcText )
        if ( !a_uCount )
            a_uCount = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MILPChar pcText = static_cast< MILPChar >( realloc( a_Memory, ( a_uCount + 1 ) ) );
    pcText[ a_uCount ] = 0;
    if ( a_pcText )
        g_memcpy( pcText, a_pcText, a_uCount );
    return pcText;
}

MILPChar mCString::Alloc( MILPCChar a_pcText1, MILPCChar a_pcText2, MILPVoid a_Memory )
{
    MIUInt uLength1 = static_cast< MIUInt >( g_strlen( a_pcText1 ) );
    MIUInt uLength2 = static_cast< MIUInt >( g_strlen( a_pcText2 ) );
    MILPChar pcText = static_cast< MILPChar >( realloc( a_Memory, ( uLength1 + uLength2 + 1 ) ) );
    pcText[ uLength1 + uLength2 ] = 0;
    g_memcpy( pcText, a_pcText1, uLength1 );
    g_memcpy( ( pcText + uLength1 ), a_pcText2, uLength2 );
    return pcText;
}

MIBool operator == ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 == a_pcText1 );
}

MIBool operator != ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 != a_pcText1 );
}

mCString operator + ( MIChar a_cLeft, mCString const & a_strRight )
{
    return mCString( mCString::Stringize( a_cLeft ), a_strRight.GetText() );
}

mCString operator + ( MILPCChar a_pcLeft, mCString const & a_strRight )
{
    return mCString( a_pcLeft, a_strRight.GetText() );
}

MIBool operator < ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 >= a_pcText1 );
}

MIBool operator <= ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 > a_pcText1 );
}

MIBool operator > ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 <= a_pcText1 );
}

MIBool operator >= ( MILPCChar a_pcText1, mCString const & a_strText2 )
{
    return ( a_strText2 < a_pcText1 );
}

mCString g_GetFileName( mCString const & a_strFilePath )
{
    MIUInt const uLastSlash = a_strFilePath.LastOf( '/' );
    MIUInt const uLastBackSlash = a_strFilePath.LastOf( '\\' );
    MIUInt const uBeginFileNameA = uLastSlash != MI_DW_INVALID ? uLastSlash + 1 : 0;
    MIUInt const uBeginFileNameB = uLastBackSlash != MI_DW_INVALID ? uLastBackSlash + 1 : 0;
    return a_strFilePath.Right( a_strFilePath.GetLength() - g_max( uBeginFileNameA, uBeginFileNameB ) );
}

mCString g_GetFileNameNoExt( mCString const & a_strFilePath )
{
    mCString strResult = g_GetFileName( a_strFilePath );
    MIUInt const uLastPoint = strResult.LastOf( '.' );
    if ( uLastPoint != MI_DW_INVALID )
        strResult.TrimRight( strResult.GetLength() - uLastPoint );
    return strResult;
}

mCString g_GetDirectoryPath( mCString const & a_strFilePath )
{
    MIUInt const uLastSlash = a_strFilePath.LastOf( '/' );
    MIUInt const uLastBackSlash = a_strFilePath.LastOf( '\\' );
    return a_strFilePath.Left( g_max( ( uLastSlash != MI_DW_INVALID ? uLastSlash : 0 ), 
                                      ( uLastBackSlash != MI_DW_INVALID ? uLastBackSlash : 0 ) ) );
}

mCString g_GetDirectoryName( mCString const & a_strFilePath )
{
    mCString strResult = g_GetDirectoryPath( a_strFilePath );
    MIUInt const uLastSlash = strResult.LastOf( '/' );
    MIUInt const uLastBackSlash = strResult.LastOf( '\\' );
    strResult.TrimLeft( g_max( ( uLastSlash != MI_DW_INVALID ? uLastSlash + 1 : 0 ), 
                               ( uLastBackSlash != MI_DW_INVALID ? uLastBackSlash + 1 : 0 ) ) );
    return strResult;
}

