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
mEResult mTIOStream< M >::Skip( MII64 a_i64Count )
{
    return Seek( ( Tell64() + a_i64Count ), mEStreamSeekMode_Begin );
}

template< mEStreamType M, mEStreamType N >
mTIStream< M > & operator >> ( mTIStream< M > & a_streamSource, mTOStream< N > & a_streamDest )
{
    mTIOStream< M > * pStreamSource = dynamic_cast< mTIOStream< M > * >( &a_streamSource );
    if ( pStreamSource )
    {
        MIU64 const u64Offset = pStreamSource->Tell64();
        pStreamSource->Seek( 0 );
        MIU64 u64Size = pStreamSource->GetSize64();
        while ( u64Size )
        {
            MIUInt const uCopySize = static_cast< MIUInt >( g_min< MIU64 >( u64Size, 1 << 20 ) );  // 1 MB max
            MIByte * pBuffer = new MIByte [ uCopySize ];
            a_streamSource.Read( pBuffer, uCopySize );
            a_streamDest.Write( pBuffer, uCopySize );
            delete [] pBuffer;
            u64Size -= uCopySize;
        }
        pStreamSource->Seek( u64Offset );
    }
    return a_streamSource;
}

template< mEStreamType M, mEStreamType N >
mTOStream< M > & operator << ( mTOStream< M > & a_streamDest, mTIStream< N > & a_streamSource )
{
    a_streamSource >> a_streamDest;
    return a_streamDest;
}

template< mEStreamType M >
MIUInt mTIOStream< M >::GetSize( void ) const
{
    return static_cast< MIUInt >( g_min< MIU64 >( GetSize64(), UINT_MAX ) );
}

template< mEStreamType M >
MIUInt mTIOStream< M >::Tell( void ) const
{
    return static_cast< MIUInt >( g_min< MIU64 >( Tell64(), UINT_MAX ) );
}