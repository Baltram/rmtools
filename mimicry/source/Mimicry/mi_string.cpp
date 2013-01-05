#include "mi_include_standard.h"
#include "mi_include_string.h"

MIUInt mCString::s_uBufferSize = EMinStaticBufferSize;
MILPChar mCString::s_pcBuffer = new MIChar [ EMinStaticBufferSize ];

mCString::mCString( MIUInt a_uCount ) :
    m_pcText( Alloc( 0, a_uCount ) )
{
}

mCString::mCString( MILPCChar a_pcText ) :
    m_pcText( Alloc( a_pcText ) )
{
}

mCString::mCString( MILPCChar a_pcText, MIUInt a_uCount ) :
m_pcText( Alloc( a_uCount ? a_pcText : 0, a_uCount ) )
{
}

mCString::mCString( MIChar a_cChar, MIUInt a_uCount ) :
    m_pcText( 0 )
{
    SetText( a_cChar, a_uCount );
}

mCString::mCString( MILPCChar a_pcText1, MILPCChar a_pcText2 ) :
    m_pcText( Alloc( a_pcText1, 0, a_pcText2, 0 ) )
{
}

mCString::mCString( MILPCChar a_pcText1, MIUInt uLength1, MILPCChar a_pcText2, MIUInt uLength2 ) :
    m_pcText( Alloc( a_pcText1, uLength1, a_pcText2, uLength2 ) )
{
}

mCString::mCString( mCString const & a_strText ) :
    m_pcText( Alloc( a_strText.GetText(), a_strText.GetLength() ) )
{
}

mCString::mCString( void ) :
    m_pcText( Alloc( 0, 0u ) )
{
}

mCString::~mCString( void )
{
    mCStringAllocator::Free( m_pcText );
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
    return AccessText()[ a_uIndex ];
}

MIChar mCString::operator [] ( MIInt a_uIndex ) const
{
    return GetText()[ a_uIndex ];
}

mCString mCString::operator + ( mCString const & a_strText ) const
{
    return mCString( GetText(), GetLength(), a_strText.GetText(), a_strText.GetLength() );
}

mCString mCString::operator + ( MIChar a_cChar ) const
{
    return mCString( GetText(), GetLength(), &a_cChar, 1 );
}

mCString mCString::operator + ( MILPCChar a_pcText ) const
{
    return mCString( GetText(), GetLength(), a_pcText, 0 );
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
    return ( *m_pcText == -1 ) ? m_pcText + 5 : m_pcText + 1;
}

mCString & mCString::Append( mCString const & a_strText )
{
    return Append( a_strText.GetText(), a_strText.GetLength() );
} 

mCString & mCString::Append( MILPCChar a_pcText, MIUInt a_uLength )
{
    if ( a_uLength == MI_DW_INVALID )
        a_uLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MIUInt const uLength = GetLength();
    if ( ( uLength < 0xFF ) && ( uLength + a_uLength >= 0xFF ) )
    {
        mCString strTemp( GetText(), uLength, a_pcText, a_uLength );
        Swap( strTemp );
    }
    else
    {
        m_pcText = Alloc( 0, GetLength(), a_pcText, a_uLength, m_pcText );
    }
    return *this;
}

void mCString::Clear( void )
{
    SetText( ( MILPCChar ) 0, 0 );
}

MIInt mCString::Compare( mCString const & a_strOther ) const
{
    return Compare( a_strOther.GetText() );
}

MIInt mCString::Compare( MILPCChar a_pcOther ) const
{
    return g_strcmp( GetText(), a_pcOther );
}

MIInt mCString::CompareNoCase( mCString const & a_strOther ) const
{
    return CompareNoCase( a_strOther.GetText() );
}

MIInt mCString::CompareNoCase( MILPCChar a_pcOther ) const
{
    MILPCChar pcText = GetText();
    do
    {
        MIInt iDiff = toupper( *pcText ) - toupper( *a_pcOther );
        if ( iDiff )
            return ( iDiff > 0 ? 1 : -1 );
    }
    while ( *pcText++ && *a_pcOther++ );
    return 0;
}

MIBool mCString::Contains( MIChar a_cChar ) const
{
    return ( FirstOf( a_cChar ) != MI_DW_INVALID );
}

MIBool mCString::Contains( MILPCChar a_pcString ) const
{
    return ( FirstOf( a_pcString ) != MI_DW_INVALID );
}

MIUInt mCString::Count( MILPCChar a_pcText ) const
{
    MIUInt uResult = 0;
    MIUInt uSeekStringLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    for ( MILPCChar pcText = GetText(), pcEndText = pcText + GetLength(); NextOf( pcText, pcEndText, a_pcText, uSeekStringLength ); pcText += uSeekStringLength )
        ++uResult;
    return uResult;
}

MIUInt mCString::Count( MIChar a_cChar ) const
{
    MIUInt uResult = 0;
    for ( MILPCChar pcText = GetText(), pcEndText = pcText + GetLength(); NextOf( pcText, pcEndText, a_cChar ); ++pcText )
        ++uResult;
    return uResult;
}

MIUInt mCString::FirstOf( MILPCChar a_pcText ) const
{
    MILPCChar pcText = GetText(), pcTextIt = pcText;
    MILPCChar pcOccurance = mCString::NextOf( pcTextIt, pcText + GetLength(), a_pcText, static_cast< MIUInt >( g_strlen( a_pcText ) ) );
    return ( pcOccurance ? static_cast< MIUInt >( pcOccurance - pcText ) : MI_DW_INVALID );
}

MIUInt mCString::FirstOf( MIChar a_cChar ) const
{
    MILPCChar pcText = GetText();
    MILPCChar pcOccurance = g_strchr( pcText, a_cChar );
    return ( pcOccurance ? static_cast< MIUInt >( pcOccurance - pcText ) : MI_DW_INVALID );
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
    return ( *m_pcText != -1 ) ? *reinterpret_cast< MIU8 * >( m_pcText ) : *reinterpret_cast< MIU32 * >( m_pcText + 1 );
}

MILPCChar mCString::GetText( void ) const
{
    return ( *m_pcText == -1 ) ? m_pcText + 5 : m_pcText + 1;
}

MIUInt mCString::LastOf( MILPCChar a_pcText ) const
{
    MIUInt uSeekStringLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MILPCChar pcText = GetText();
    MILPCChar pcTextIt = pcText + GetLength() - uSeekStringLength;
    for ( ; pcTextIt >= pcText; --pcTextIt )
        if ( ( *pcTextIt == *a_pcText ) && ( !g_strncmp( pcTextIt, a_pcText, uSeekStringLength ) ) )
            return static_cast< MIUInt >( pcTextIt - pcText );
    return MI_DW_INVALID;
}

MIUInt mCString::LastOf( MIChar a_cChar ) const
{
    MILPCChar pcText = GetText();
    MILPCChar pcTextIt = pcText + GetLength() - 1;
    for ( ; pcTextIt >= pcText; --pcTextIt )
        if ( *pcTextIt == a_cChar )
            return static_cast< MIUInt >( pcTextIt - pcText );
    return MI_DW_INVALID;
}

mCString mCString::Left( MIUInt a_uCount ) const
{
    return mCString( GetText(), a_uCount );
}

mCString & mCString::Replace( MILPCChar a_pcText, MILPCChar a_pcNewText, MIUInt & a_uNumReplaced )
{
    a_uNumReplaced = 0;
    MIUInt const uSearchTextLength  = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MIUInt const uReplaceTextLength = static_cast< MIUInt >( g_strlen( a_pcNewText ) );
    AssureStaticBufferSize( GetLength() + uReplaceTextLength );
    MILPCChar pcTextIt1    = GetText();
    MILPCChar pcTextIt2    = pcTextIt1;
    MILPCChar pcEndText    = pcTextIt1 + GetLength();
    MILPChar  pcNewText    = AccessStaticBuffer();
    MILPChar  pcNewTextIt  = pcNewText;
    MILPChar  pcEndNewText = pcNewText + GetStaticBufferSize();
    while ( NextOf( pcTextIt2, pcEndText, a_pcText, uSearchTextLength ), MITrue )
    {
        if ( !pcTextIt2 )
            pcTextIt2 = pcEndText;
        MIUInt const uCopyLength = static_cast< MIUInt >( pcTextIt2 - pcTextIt1 );
        if ( uCopyLength + uReplaceTextLength > static_cast< MIUInt >( pcEndNewText - pcNewTextIt ) )
        {
            mCString str1( pcNewText, static_cast< MIUInt >( pcNewTextIt - pcNewText ) );
            mCString str2( pcTextIt1, static_cast< MIUInt >( pcEndText - pcTextIt1 ) );
            return *this = str1 + str2.Replace( a_pcText, a_pcNewText );
        }
        g_memcpy( pcNewTextIt, pcTextIt1, uCopyLength );
        pcNewTextIt += uCopyLength;
        if ( pcTextIt2 == pcEndText )
            break;
        g_memcpy( pcNewTextIt, a_pcNewText, uReplaceTextLength );
        pcNewTextIt += uReplaceTextLength;
        pcTextIt2 += uSearchTextLength;
        pcTextIt1 = pcTextIt2;
        ++a_uNumReplaced;
    }
    SetText( pcNewText, static_cast< MIUInt >( pcNewTextIt - pcNewText ) );
    return *this;
}

mCString & mCString::Replace( MILPCChar a_pcText, MILPCChar a_pcNewText )
{
    MIUInt uCount;
    return Replace( a_pcText, a_pcNewText, uCount );
}

mCString & mCString::Replace( MIChar a_cChar, MIChar a_cNewChar, MIUInt & a_uNumReplaced )
{
    a_uNumReplaced = 0;
    MILPChar pcText = AccessText();
    MILPCChar pcEndText = pcText + GetLength();
    while ( NextOf( *const_cast< MILPCChar * >( &pcText ), pcEndText, a_cChar ) )
    {
        *( pcText++ ) = a_cNewChar;
        ++a_uNumReplaced;
    }
    return *this;
}

mCString & mCString::Replace( MIChar a_cChar, MIChar a_cNewChar )
{
    MIUInt uCount;
    return Replace( a_cChar, a_cNewChar, uCount );
}

mCString mCString::Right( MIUInt a_uCount ) const
{
    return mCString( GetText() + GetLength() - a_uCount, a_uCount );
}

MIInt mCString::Scan( MILPCChar a_pcFormat, ... ) const
{
    va_list argp;
    va_start( argp, a_pcFormat );
    MIInt iCount = VScan( a_pcFormat, argp );
    va_end( argp );
    return iCount;
}

void mCString::SetText( mCString const & a_strText )
{
    if ( &a_strText != this )
        m_pcText = Alloc( a_strText.GetText(), a_strText.GetLength(), m_pcText );
}

void mCString::SetText( MILPCChar a_pcText )
{
    m_pcText = Alloc( a_pcText, 0u, m_pcText );
}

void mCString::SetText( MILPCChar a_pcText, MIUInt a_uCount )
{
    m_pcText = Alloc( a_uCount ? a_pcText : 0, a_uCount, m_pcText );
}

void mCString::SetText( MIChar a_cChar, MIUInt a_uCount )
{
    m_pcText = Alloc( 0, a_uCount, m_pcText );
    for ( MILPChar pcText = AccessText(); a_uCount--; pcText[ a_uCount ] = a_cChar );
}

void mCString::Swap( mCString & a_strOther )
{
    g_swap( m_pcText, a_strOther.m_pcText );
}

mCString & mCString::ToLower( void )
{
    for ( MILPChar pcText = AccessText(); *pcText; ++pcText )
        *pcText = static_cast< MIChar >( tolower( *pcText ) );
    return *this;
}

mCString & mCString::ToUpper( void )
{
    for ( MILPChar pcText = AccessText(); *pcText; ++pcText )
        *pcText = static_cast< MIChar >( toupper( *pcText ) );
    return *this;
}

mCString & mCString::TrimLeft( MILPCChar a_pcChars )
{
    MILPCChar pcText = GetText(), pcTextIt = pcText;
    for ( ; *pcTextIt && g_strchr( a_pcChars, *pcTextIt ); ++pcTextIt );
    return TrimLeft( static_cast< MIUInt >( pcTextIt - pcText ) );
}

mCString & mCString::TrimLeft( MIChar a_cChar )
{
    if ( a_cChar == 0 )
        return *this;
    MIUInt u = 0;
    for ( MILPChar pcText = AccessText(); pcText[ u ] == a_cChar; ++u );
    return TrimLeft( u );
}

mCString & mCString::TrimLeft( MIUInt a_uCount )
{
    if ( !a_uCount )
        return *this;
    MIUInt uNewLength = GetLength();
    a_uCount = g_min( uNewLength, a_uCount );
    uNewLength -= a_uCount;
    AssureStaticBufferSize( uNewLength );
    g_memcpy( AccessStaticBuffer(), AccessText() + a_uCount, uNewLength );
    SetText( AccessStaticBuffer(), uNewLength );
    return *this;
}

mCString & mCString::TrimRight( MIChar a_cChar )
{
    MIUInt uLength = GetLength();
    MIUInt u = uLength;
    for ( MILPChar pcText = AccessText(); u && ( pcText[ u - 1 ] == a_cChar ); --u );
    TrimRight( uLength - u );
    return *this;
}

mCString & mCString::TrimRight( MILPCChar a_pcChars )
{
    MIUInt const uLength = GetLength();
    if ( !uLength )
        return *this;
    MILPCChar pcText = AccessText();
    MILPCChar pcTextIt = pcText + uLength;
    while ( ( pcTextIt-- >= pcText ) && g_strchr( a_pcChars, *pcTextIt ) );
    return TrimRight( uLength - static_cast< MIUInt >( pcTextIt - pcText + 1 ) );
}

mCString & mCString::TrimRight( MIUInt a_uCount )
{
    a_uCount = g_min( a_uCount, GetLength() );
    m_pcText = Alloc( 0, GetLength() - a_uCount, m_pcText );
    return *this;
}

mCString & mCString::VFormat( MILPCChar a_pcFormat, va_list a_Arguments )
{
    MI_CRT_NO_WARNINGS( MIUInt uSize = vsprintf( s_pcBuffer, a_pcFormat, a_Arguments ); )
    SetText( s_pcBuffer, uSize );
    return *this;
}

MIInt mCString::VScan( MILPCChar a_pcFormat, va_list a_Arguments ) const
{
    MIInt iCount = 0;
#ifdef _MSC_VER
    MILPCChar pcFormat = a_pcFormat;
    MILPCChar pcEndFormat = a_pcFormat + g_strlen( a_pcFormat );
    MIInt ( MI_CDECL * funcSscanf )( MILPCChar, MILPCChar, ... );
    MI_CRT_NO_WARNINGS( funcSscanf = &sscanf; )
    while ( NextOf( pcFormat, pcEndFormat, '%' ) )
    {
        ++pcFormat;
        if ( ( *pcFormat != '%' ) && ( *pcFormat != '*' ) )
            ++iCount;
        ++pcFormat;
    }
    if ( iCount )
    {
        MILPCChar pcText = GetText();
        MIU32 ** pLastArg = &va_arg( a_Arguments, MIU32 * ) + ( iCount - 1 );
        MI_ASSERT_32_BIT
        MI_ASM {
            mov  esi, esp
            mov  eax, iCount
            mov  edx, pLastArg
label_loop:
            push [ edx ]
            sub  edx, 4
            sub  eax, 1
            test eax, eax
            jnz  label_loop
            push a_pcFormat
            mov  eax, pcText
            push eax
            call funcSscanf
            mov  iCount, eax
            mov  esp, esi
        }
    }
#else
    iCount = vsscanf( GetText(), a_pcFormat, a_Arguments );
#endif
    return iCount;
}

namespace
{
    inline
    MILPChar _Alloc( MIUInt a_uTextLength, MILPChar & a_pcTextPart, MILPVoid a_Memory )
    {
        MILPChar pcResult; 
        if ( a_uTextLength > 0xFE )
        {
            pcResult = static_cast< MILPChar >( mCStringAllocator::Realloc( a_Memory, a_uTextLength + 6 ) );
            pcResult[ a_uTextLength + 5 ] = 0;
            *pcResult = -1;
            *reinterpret_cast< MIU32 * >( pcResult + 1 ) = static_cast< MIU32 >( a_uTextLength );
            a_pcTextPart = pcResult + 5;
        }
        else
        {
            pcResult = static_cast< MILPChar >( mCStringAllocator::Realloc( a_Memory, a_uTextLength + 2 ) );
            pcResult[ a_uTextLength + 1 ] = 0;
            *reinterpret_cast< MIU8 * >( pcResult ) = static_cast< MIU8 >( a_uTextLength );
            a_pcTextPart = pcResult + 1;
        }
        return pcResult;
    }
}

MILPChar mCString::Alloc( MILPCChar a_pcText, MIUInt a_uLength, MILPVoid a_Memory )
{
    if ( !a_uLength && a_pcText )
        a_uLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    MILPChar pcText; 
    MILPChar pcResult = _Alloc( a_uLength, pcText, a_Memory );
    if ( a_pcText )
        g_memcpy( pcText, a_pcText, a_uLength );
    return pcResult;
}

MILPChar mCString::Alloc( MILPCChar a_pcText1, MIUInt a_uLength1, MILPCChar a_pcText2, MIUInt a_uLength2, MILPVoid a_Memory )
{
    if ( !a_uLength1 && a_pcText1 )
        a_uLength1 = static_cast< MIUInt >( g_strlen( a_pcText1 ) );
    if ( !a_uLength2 && a_pcText2 )
        a_uLength2 = static_cast< MIUInt >( g_strlen( a_pcText2 ) );
    MILPChar pcText; 
    MILPChar pcResult = _Alloc( a_uLength1 + a_uLength2, pcText, a_Memory );
    if ( a_pcText1 )
        g_memcpy( pcText, a_pcText1, a_uLength1 );
    g_memcpy( pcText + a_uLength1, a_pcText2, a_uLength2 );
    return pcResult;
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
    return mCString( &a_cLeft, 1, a_strRight.GetText(), a_strRight.GetLength() );
}

mCString operator + ( MILPCChar a_pcLeft, mCString const & a_strRight )
{
    return mCString( a_pcLeft, static_cast< MIUInt >( g_strlen( a_pcLeft ) ), a_strRight.GetText(), a_strRight.GetLength() );
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

mCString g_GetFileExt( mCString const & a_strFilePath )
{
    MIUInt const uLastPoint = a_strFilePath.LastOf( '.' );
    return ( uLastPoint == MI_DW_INVALID ) ? "" : a_strFilePath.Right( a_strFilePath.GetLength() - uLastPoint - 1 );
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

