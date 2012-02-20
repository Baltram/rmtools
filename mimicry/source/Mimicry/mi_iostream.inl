template< mEStreamType M >
mTIOStream< M >::~mTIOStream( void )
{
}

template< mEStreamType M >
void mTIOStream< M >::SetInvertEndianness( MIBool a_bMode )
{
    mTIStream< M >::SetInvertEndianness( a_bMode );
    mTOStream< M >::SetInvertEndianness( a_bMode );
}

template< mEStreamType M >
mEResult mTIOStream< M >::Skip( MIInt a_iCount )
{
    return Seek( ( Tell() + a_iCount ), mEStreamSeekMode_Begin );
}

template< mEStreamType M, mEStreamType N >
mTIStream< M > & operator >> ( mTIStream< M > & a_streamSource, mTOStream< N > & a_streamDest )
{
    mTIOStream< M > * pStreamSource = dynamic_cast< mTIOStream< M > * >( &a_streamSource );
    if ( pStreamSource )
    {
        MIUInt const uOffset = pStreamSource->Tell();
        pStreamSource->Seek( 0 );
        MIUInt uSize = pStreamSource->GetSize();
        MIByte * pBuffer = new MIByte [ uSize ];
        a_streamSource.Read( pBuffer, uSize );
        pStreamSource->Seek( uOffset );
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