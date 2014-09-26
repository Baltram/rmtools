#include "mi_include_streams.h"

mCDocParser::mCDocParser( mCStringStream & a_streamIn, mCIOStreamBinary & a_streamOut ) :
    m_streamIn( a_streamIn ),
    m_streamOut( a_streamOut ),
    m_uLastErrorLine( 0 )
{
}

MIBool mCDocParser::EnterArray( MIBool a_bSetLastErrorLine )
{
    return MatchImmediate( "[", a_bSetLastErrorLine, MITrue );
}

MIBool mCDocParser::EnterBlock( mCString * a_pName, MIBool a_bSetLastErrorLine, MIBool a_bCaseSensitive )
{
    mCString strTemp;
    if ( !a_pName )
        a_pName = &strTemp;
    if ( *a_pName != "" )
        return MatchImmediate( *a_pName + " {", a_bSetLastErrorLine, a_bCaseSensitive );
    if ( MatchImmediate( "{", a_bSetLastErrorLine, MITrue ) )
        return MITrue;
    mCStringArray arrTokens;
    if ( !Match( "{", arrTokens, a_bSetLastErrorLine, MITrue ) )
        return MIFalse;
    for ( MIUInt u = 0, ue = arrTokens.GetCount(); u != ue; ++u )
        *a_pName += " " + arrTokens[ u ];
    a_pName->ReplaceLeft( " " );
    return MITrue;
}

MIBool mCDocParser::EnterBlock( mCString const & a_strName, MIBool a_bSetLastErrorLine, MIBool a_bCaseSensitive )
{
    mCString strName = a_strName;
    return EnterBlock( &strName, a_bSetLastErrorLine, a_bCaseSensitive );
}

MIUInt mCDocParser::GetLastErrorLine( void )
{
    return m_uLastErrorLine;
}

MIBool mCDocParser::LeaveArray( MIBool a_bSetLastErrorLine )
{
    return MatchImmediate( "]", a_bSetLastErrorLine, MITrue );
}

MIBool mCDocParser::LeaveBlock( MIBool a_bSetLastErrorLine )
{
    return MatchImmediate( "}", a_bSetLastErrorLine, MITrue );
}

MIBool mCDocParser::Match( mCString a_strText, mCStringArray & a_arrTokens, MIBool a_bSetLastErrorLine, MIBool a_bCaseSensitive, MIBool a_bFulltext )
{
    MIUInt const uOffset = m_streamIn.Tell();
    while ( !m_streamIn.IsAtEnd() )
    {
        if ( MatchImmediate( a_strText, a_bSetLastErrorLine, a_bCaseSensitive, a_bFulltext ) )
            return MITrue;
        m_streamIn >> a_arrTokens.AddNew();
    }
    return m_streamIn.Seek( uOffset ), mCDocParser::SetLastErrorLine( a_bSetLastErrorLine ), MIFalse;
}

MIBool mCDocParser::MatchImmediate( mCString a_strText, MIBool a_bSetLastErrorLine, MIBool a_bCaseSensitive, MIBool a_bFulltext, MIBool a_bAtStart )
{
    if ( m_streamIn.IsAtEnd() )
        return mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, a_bAtStart ), MIFalse;
    MIUInt const uOffset = m_streamIn.Tell();
    if ( a_strText.Contains( ' ' ) )
    {
        mCString strLeft, strRight;
        a_strText.Split( a_strText.FirstOf( ' ' ), strLeft, strRight, MITrue );
        if ( MatchImmediate( strLeft, a_bSetLastErrorLine, a_bCaseSensitive, a_bFulltext ) && MatchImmediate( strRight, a_bSetLastErrorLine, a_bCaseSensitive, a_bFulltext ) )
            return MITrue;
        return m_streamIn.Seek( uOffset ), mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, a_bAtStart ), MIFalse;
    }
    mCString strText = m_streamIn.ReadString();
    if ( !a_bCaseSensitive )
        strText.ToLower(), a_strText.ToLower();
    if ( strText.FirstOf( a_strText.GetText() ) != 0 )
        return m_streamIn.Seek( uOffset ), mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, a_bAtStart ), MIFalse;
    if ( strText.GetLength() != a_strText.GetLength() )
    {
        if ( a_bFulltext )
            return m_streamIn.Seek( uOffset ), mCDocParser::SetLastErrorLine( a_bSetLastErrorLine, a_bAtStart ), MIFalse;
        m_streamIn.Skip( -static_cast< MIInt >( strText.GetLength() - a_strText.GetLength() ) );
    }
    return MITrue;
}

void mCDocParser::SetLastErrorLine( MIBool a_bForReal, MIBool a_bSkipWhitespaces )
{
    if ( !a_bForReal )
        return;
    if ( a_bSkipWhitespaces && !m_streamIn.IsAtEnd() )
        m_streamIn.Skip( -static_cast< MIInt >( m_streamIn.ReadString().GetLength() ) );
    m_uLastErrorLine = mCString( static_cast< MILPCChar >( m_streamIn.GetBuffer() ), m_streamIn.Tell() ).Count( '\n' ) + 1;
}