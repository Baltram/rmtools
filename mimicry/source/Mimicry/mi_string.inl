inline MILPChar mCString::AccessStaticBuffer( void )
{
    return s_pcBuffer;
}

inline MIUInt mCString::GetStaticBufferSize( void )
{
    return s_uBufferSize;
}

inline MILPChar mCString::Stringize( MIChar a_cChar )
{
    static MIChar pcResult[ 2 ] = "\0";
    pcResult[ 0 ] = a_cChar;
    return pcResult;
}

inline MILPCChar mCString::NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MIChar a_cChar )
{
    a_pcText = reinterpret_cast< MILPCChar >( g_memchr( a_pcText, a_cChar, ( a_pcEndText - a_pcText ) ) );
    return a_pcText;
}

inline MILPCChar mCString::NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MILPCChar a_pcString, MIUInt a_uStringLength )
{
    MIChar cFirstChar = *a_pcString++;
    --a_uStringLength;
    while ( NextOf( a_pcText, a_pcEndText, cFirstChar ) )
    {
        if ( g_strncmp( ++a_pcText, a_pcString, a_uStringLength ) )
            continue;
        --a_pcText;
        break;
    }
    return a_pcText;
}