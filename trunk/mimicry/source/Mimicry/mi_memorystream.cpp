#include "mi_include_unity.h"

#ifdef MI_IN_UNITY_FILE

#include "mi_include_standard.h"
#include "mi_include_streams.h"

mCMemoryStream::~mCMemoryStream( void )
{
}

mCMemoryStream::mCMemoryStream( void )
{
}

mCMemoryStream::mCMemoryStream( mCMemoryStream const & a_Source ) :
    mCMemoryStreamBaseBinary( a_Source )
{
}

mCMemoryStream::mCMemoryStream( mCIOStreamBinary & a_Source ) :
    mCMemoryStreamBaseBinary( a_Source )
{
}

mCMemoryStream & mCMemoryStream::operator = ( mCMemoryStream const & a_Source )
{
    if ( &a_Source != this )
        mCMemoryStreamBaseBinary::operator = ( a_Source );
    return *this;
}

mEResult mCMemoryStream::Read( mCString & a_strDest )
{
    // Note: m_arrBuffer (a mCCharArray) is guaranteed to be 0-terminated.
    if ( m_uPosition == m_arrBuffer.GetCount() )
    {
        MI_ERROR( mCStreamError, ESizeExceeded, "Buffer size exceeded when reading binary data." );
        return mEResult_False;
    }
    MILPCChar const pcText = m_arrBuffer.GetBuffer() + m_uPosition;
    MIUInt const uSize = static_cast< MIUInt >( g_strlen( pcText ) );
    a_strDest.SetText( pcText, uSize );
    m_uPosition += uSize;
    return mEResult_Ok;
}

mEResult mCMemoryStream::ReadFormatted( MILPVoid, MILPCChar )  // Not used.
{    
    return mEResult_Ok;
}

mEResult mCMemoryStream::WriteFormatted( MILPCVoid, MILPCChar )  // Not used.
{
    return mEResult_Ok;
}

#endif