inline
void mCString::AssureStaticBufferSize( MIUInt a_uSize )
{
    if ( s_uBufferSize < a_uSize )
    {
        delete [] s_pcBuffer;
        s_pcBuffer = new MIChar [ s_uBufferSize = a_uSize * 5 / 4 + 255 ];
    }
}

inline
MILPChar mCString::AccessStaticBuffer( void )
{
    return s_pcBuffer;
}

inline
MIUInt mCString::GetStaticBufferSize( void )
{
    return s_uBufferSize;
}

inline
MILPChar mCString::Stringize( MIChar a_cChar )
{
    static MIChar pcResult[ 2 ] = "\0";
    pcResult[ 0 ] = a_cChar;
    return pcResult;
}

inline
MILPCChar mCString::NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MIChar a_cChar )
{
    a_pcText = reinterpret_cast< MILPCChar >( g_memchr( a_pcText, a_cChar, ( a_pcEndText - a_pcText ) ) );
    return a_pcText;
}

inline
MILPCChar mCString::NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MILPCChar a_pcString, MIUInt a_uStringLength )
{
    if ( !a_uStringLength-- )
        return a_pcText = 0;
    MIChar cFirstChar = *a_pcString++;
    while ( ( static_cast< MIUInt >( a_pcEndText - a_pcText ) > a_uStringLength ) && NextOf( a_pcText, a_pcEndText, cFirstChar ) && ( static_cast< MIUInt >( a_pcEndText - a_pcText ) > a_uStringLength ) )
        if ( !g_strncmp( ++a_pcText, a_pcString, a_uStringLength ) )
            return --a_pcText;
    return a_pcText = 0;
}