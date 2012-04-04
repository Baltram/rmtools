#include "mi_include_core.h"

mCError * mCError::s_pLastError = 0;

void mCError::ClearError( mCError const * a_pError )
{
    mCError ** pErrorPointer = &s_pLastError;
    mCError * pError;
    while( ( pError = *pErrorPointer ) != 0 )
    {
        if ( pError == a_pError )
        {
            *pErrorPointer = pError->m_pPrevious;
            delete pError;
            return;
        }
        else
        {
            pErrorPointer = &pError->m_pPrevious;
        }
    }
}

MILPCChar mCError::GetText( void ) const
{
    return m_pcText;
}

MILPCChar mCError::GetFile( void ) const
{
    return m_pcFile;
}

MIUInt mCError::GetLine( void ) const
{
    return m_uLine;
}

mCError::~mCError( void )
{
    delete [] m_pcText;
    delete [] m_pcFile;
}

mCError::mCError( void ) :
    m_iErrorCode( 0 ),
    m_pcText( 0 ),
    m_pcFile( 0 ),
    m_uLine( 0 ),
    m_pPrevious( 0 )
{
}

mCError::mCError( mCError const & )  // Not used.
{
}

void mCError::Init( MIInt a_iErrorCode, MILPCChar a_pcText, MILPCChar a_pcFile, MIUInt a_uLine, mCError * a_pPrevious )
{
    MIUInt uLength;

    uLength = static_cast< MIUInt >( g_strlen( a_pcText ) );
    m_pcText = new MIChar [ uLength + 1 ];
    g_memcpy( m_pcText, a_pcText, ( uLength + 1 ) );

    uLength = static_cast< MIUInt >( g_strlen( a_pcFile ) );
    m_pcFile = new MIChar [ uLength + 1 ];
    g_memcpy( m_pcFile, a_pcFile, ( uLength + 1 ) );

    m_iErrorCode = a_iErrorCode;
    m_uLine = a_uLine;
    m_pPrevious = a_pPrevious;
}