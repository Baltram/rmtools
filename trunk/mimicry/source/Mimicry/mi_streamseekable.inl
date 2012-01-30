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
mCIStream< M > & operator >> ( mCIStream< M > & a_streamSource, mCOStream< N > & a_streamDest )
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
mCOStream< M > & operator << ( mCOStream< M > & a_streamDest, mCIStream< N > & a_streamSource )
{
    a_streamSource >> a_streamDest;
    return a_streamDest;
}