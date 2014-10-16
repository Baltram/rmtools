#include "mi_include_streams.h"

mCDoc::mCDoc( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut, mCString const & a_strIndent ) :
    m_strIndent( a_strIndent ),
    m_streamIn( a_streamIn ),
    m_streamOut( a_streamOut )
{
}

void mCDoc::EndArray( MIBool a_bNewLine )
{
    DecrementIndent();
    if ( a_bNewLine )
        WriteLine( "]" );
    else
        Write( "]" );
}

void mCDoc::EndBlock( MIBool a_bNewLine )
{
    DecrementIndent();
    if ( a_bNewLine )
        WriteLine( "}" );
    else
        Write( "}" );
}

void mCDoc::StartArray( mCString const & a_strName )
{
    if ( a_strName != "" )
        Write( a_strName + " = " );
    WriteLine( "[" );
    IncrementIndent();
}

void mCDoc::StartBlock( mCString const & a_strTitle )
{
    WriteLine( a_strTitle + " {" );
    IncrementIndent();
}

namespace
{
    MIBool s_bMidLine = MIFalse;
}

void mCDoc::Write( mCString const & a_strText )
{
    if ( !s_bMidLine )
        m_streamOut << m_strIndent;
    m_streamOut << a_strText;
    s_bMidLine = MITrue;
}

void mCDoc::WriteLine( mCString const & a_strText )
{
    Write( a_strText );
    m_streamOut << "\r\n";
    s_bMidLine = MIFalse;
}

void mCDoc::DecrementIndent( void )
{
    m_strIndent.TrimRight( 4U );
}

void mCDoc::IncrementIndent( void )
{
    m_strIndent += "    ";
}