template< typename T >
void mCError::ClearAllErrors()
{
    mCError ** pErrorPointer = &s_pLastError;
    mCError * pError;
    while( pError = *pErrorPointer )
    {
        if ( dynamic_cast< T * >( pError ) )
        {
            *pErrorPointer = pError->m_pPrevious;
            delete pError;
        }
        else
        {
            pErrorPointer = &pError->m_pPrevious;
        }
    }
}

template< typename T >
void mCError::Error( MIInt a_iErrorCode, MILPCChar a_pcText, MILPCChar a_pcFile, MIUInt a_uLine )
{
    mCError * pError = new T;
    pError->Init( a_iErrorCode, a_pcText, a_pcFile, a_uLine, s_pLastError );
    s_pLastError = pError;
}

template< typename T >
mCError const * mCError::GetLastError( void )
{
    mCError * pError = s_pLastError;
    while ( pError )
    {
        if ( dynamic_cast< T * >( pError ) )
            break;
        pError = pError->m_pPrevious;
    }
    return pError;
}

template< typename T >
MIInt mCError::GetErrorCode( void ) const
{
    if ( typeid( T const * ) == typeid( this ) )
        return m_iErrorCode;
    return MI_DW_INVALID;
}
