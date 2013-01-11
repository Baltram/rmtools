#include "mi_include_converters.h"

mCGenomeFile::mCGenomeFile( mCIOStreamBinary * a_pStream ) :
    m_pStream( a_pStream ),
    m_bIsOpen( MIFalse )
{
}

mCIOStreamBinary const & mCGenomeFile::operator () ( void ) const
{
    return *m_pStream;
}

mCIOStreamBinary & mCGenomeFile::operator () ( void )
{
    return *m_pStream;
}

mCGenomeFile & mCGenomeFile::operator >> ( mCString & a_strDest )
{
    Read( a_strDest );
    return *this;
}

mCGenomeFile & mCGenomeFile::operator << ( mCString & a_strSource )
{
    Write( a_strSource );
    return *this;
}

void mCGenomeFile::EndWrite( void )
{
    if ( !CanWrite() )
        return;
    MIUInt uEndOffset = m_pStream->Tell();
    m_pStream->Seek( m_uStartOffset + 10 );
    *m_pStream << g_32( uEndOffset - m_uStartOffset );
    m_pStream->Seek( uEndOffset );
    *m_pStream << ( MIU32 ) 0xDEADBEEF << ( MIU8 ) 1 << g_32( m_arrStrings.GetCount() );
    for ( MIUInt u = 0, ue = m_arrStrings.GetCount(); u != ue; ++u )
        *m_pStream << static_cast< MIU16 >( m_arrStrings[ u ].GetLength() ) << m_arrStrings[ u ];
    m_bIsOpen = MIFalse;
}

void mCGenomeFile::Read( mCString & a_strSource )
{
    if ( !CanRead() )
        return;
    if ( m_bIsArchiveFile )
        a_strSource = m_arrStrings[ m_pStream->ReadU16() ];
    else
        m_pStream->Read( a_strSource, m_pStream->ReadU16() );
}

mCString mCGenomeFile::ReadString( void )
{
    mCString strResult;
    Read( strResult );
    return strResult;
}

void mCGenomeFile::StartRead( void )
{
    if ( m_bIsOpen && ( CanRead(), MITrue ) )
        return;
    m_bIsOpen = m_bIsReadMode = MITrue;
    m_uStartOffset = m_pStream->Tell();
    m_bIsArchiveFile = ( m_pStream->GetSize() - m_uStartOffset >= 23 ) && ( m_pStream->ReadString( 8 ) == "GENOMFLE" );
    if ( !m_bIsArchiveFile )
    {
        m_pStream->Seek( m_uStartOffset );
        return;
    }
    m_pStream->Skip( 2 );
    m_pStream->Seek( m_uStartOffset + m_pStream->ReadU32() + 5 );
    MIUInt const uCount = m_pStream->ReadU32();
    m_arrStrings.Resize( uCount );
    m_mapStringIndices.Reserve( uCount );
    for ( MIUInt u = 0; u != uCount; ++u )
        m_mapStringIndices[ m_arrStrings[ u ] = m_pStream->ReadString( m_pStream->ReadU16() ) ] = u;
    m_pStream->Seek( m_uStartOffset + 14 );
}

void mCGenomeFile::StartWrite( void )
{
    if ( m_bIsOpen && ( CanWrite(), MITrue ) )
        return;
    m_bIsOpen = MITrue, m_bIsReadMode = MIFalse;
    m_uStartOffset = m_pStream->Tell();
    m_bIsArchiveFile = MITrue;
    *m_pStream << "GENOMFLE" << ( MIU16 ) 1 << ( MIU32 ) 0;
}

void mCGenomeFile::Write( mCString const & a_strSource )
{
    if ( !CanWrite() )
        return;
    MIUInt uCount = m_mapStringIndices.GetCount();
    MIUInt * pIndex;
    if ( ( pIndex = &m_mapStringIndices[ a_strSource ] ), uCount != m_mapStringIndices.GetCount() )
        *pIndex = uCount;
    *m_pStream << static_cast< MIU16 >( *pIndex );
}

MIBool mCGenomeFile::CanRead( void )
{
    if ( m_bIsOpen || m_bIsReadMode )
        return MITrue;
    return MI_ERROR( mCStreamError, EFileNotOpen, "Genome file not open for read operations." ), MIFalse;
}

MIBool mCGenomeFile::CanWrite( void )
{
    if ( m_bIsOpen || !m_bIsReadMode )
        return MITrue;
    return MI_ERROR( mCStreamError, EFileNotOpen, "Genome file not open for write operations." ), MIFalse;
}

mCGenomeFile::mCGenomeFile( void )
{
}

mCGenomeFile & operator << ( mCString & a_strDest, mCGenomeFile & a_Source )
{
    return a_Source >> a_strDest;
}

mCGenomeFile & operator >> ( mCString & a_strSource, mCGenomeFile & a_Dest )
{
    return a_Dest << a_strSource;
}
