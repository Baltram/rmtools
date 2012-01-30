inline
mEResult mCStreamSeekable::Skip( MIInt a_iCount )
{
    return Seek( ( Tell() + a_iCount ), mEStreamSeekMode_Begin );
}

inline
mCStreamSeekable::~mCStreamSeekable( void )
{
}

template< mEStreamType M, mEStreamType N >
mTIStream< M > & operator >> ( mTIStream< M > & a_streamSource, mTOStream< N > & a_streamDest )
{
    mCStreamSeekable * pStreamSource = dynamic_cast< mCStreamSeekable * >( &a_streamSource );
    if ( pStreamSource )
    {
        MIUInt uSize = pStreamSource->GetSize() - pStreamSource->Tell();
        MIByte * pBuffer = new MIByte [ uSize ];
        a_streamSource.Read( pBuffer, uSize );
        a_streamDest.Write( pBuffer, uSize );
        delete [] pBuffer;
    }
    return a_streamSource;
}

template< mEStreamType M, mEStreamType N >
mTOStream< M > & operator << ( mTOStream< M > & a_streamDest, mTIStream< N > & a_streamSource )
{
    a_streamSource >> a_streamDest;
    return a_streamDest;
}