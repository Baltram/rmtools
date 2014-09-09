#include <cstdio>
#include <ctime>
#include "..\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

mCString ConvertDdsToRisen2Ximg( mCFileStream & a_streamSource )
{
    MIUInt uSize = a_streamSource.GetSize();
    MIUInt uWidth, uHeight, uMipMapCount = 0;
    MIU32 u32Compression = 21;
    a_streamSource.Seek( 76 );
    if ( a_streamSource.ReadU32() != 32 )
        return "Invalid .dds file.";
    if ( a_streamSource.ReadU32() & 0x4 )
        a_streamSource >> u32Compression;
    a_streamSource.Seek( 8 );
    if ( a_streamSource.ReadU32() & 0x20000 )
        uMipMapCount = 1;
    a_streamSource >> g_32( uHeight ) >> g_32( uWidth );
    a_streamSource.Skip( 8 );
    if ( uMipMapCount )
        a_streamSource >> g_32( uMipMapCount );
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              "._ximg";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    MIU64 u64FileTime = static_cast< MIU64 >( time( 0 ) ) * 10000000 + 116444736000000000;

    streamOut.Write( "GAR3\0\0\0\0", 8 );
    streamOut.Write( "GR01", 4 );
    streamOut.Write( "IM04", 4 );
    streamOut << ( MIU32 ) 40;       // PropOffset
    streamOut << ( MIU32 ) 154;      // PropLangth
    streamOut << ( MIU32 ) 194;      // DataOffset
    streamOut << g_32( uSize + 8 );  // DataLength
    streamOut << u64FileTime;
    streamOut.Write( ".dds\0\0\0\0", 8 );

    streamOut.Write( "GEC0", 4 );
    streamOut << ( MIU32 ) 0x251EC780;  // Class name hash/type_info (->bCRuntimeType)?
    streamOut << ( MIU16 ) 1;
    streamOut << ( MIU32 ) 74;
    streamOut << ( MIU32 ) 62;
    streamOut << ( MIU32 ) 70;
    streamOut << ( MIU16 ) 4;           // Property count.

    streamOut << ( MIU32 ) 0x0B888030;  // int
    streamOut << ( MIU32 ) 0x0E60A085;  // Width
    streamOut << ( MIU32 ) 4;
    streamOut << g_32( uWidth );

    streamOut << ( MIU32 ) 0x0B888030;  // int
    streamOut << ( MIU32 ) 0xB73174BE;  // Height
    streamOut << ( MIU32 ) 4;
    streamOut << g_32( uHeight );

    streamOut << ( MIU32 ) 0xB23C93CD;  // SkipMips
    streamOut << ( MIU32 ) 0x3EA71CD5;  // long
    streamOut << ( MIU32 ) 4;
    streamOut << ( MIU32 ) 0;           // 1 <= uMipMapCount <= 6 (ToDo: uMipMapCount > 6).

    MIU32 u32Format = 0xC43B9B72;       // 32bpp, no compression (ToDo: 24bpp -> 0x0).
    if ( u32Compression != 21 )
        u32Format = 0x0550FE66 + ( u32Compression >> 24 );
    streamOut << ( MIU32 ) 0x9A8892EC;
    streamOut << ( MIU32 ) 0x35F9A530;
    streamOut << ( MIU32 ) 8;
    streamOut << u32Format << u32Compression;

    // ClassData
    streamOut << ( MIU32 ) 0x251EC780;
    streamOut << ( MIU16 ) 1;
    streamOut << ( MIU32 ) 32;
    streamOut << ( MIU64 ) 0 << ( MIU64 ) 0;                    // SkipMips == 0 (ToDo: SkipMips != 0).
    streamOut << ( MIU32 ) 0xFFFF << ( MIU32 ) 0x0;             // Experimental.
    streamOut << ( MIU32 ) 0x316483CD << ( MIU32 ) 0x557341E5;  // Experimental.
    streamOut << ( MIU32 ) 0x7BF929DD << ( MIU32 ) 1 << ( MIU16 ) 0;
    streamOut << ( MIU32 ) 0x84ADA072 << ( MIU32 ) 1 << ( MIU16 ) 0;

    mCByteArray arrData( 0, uSize );
    a_streamSource.Seek( 0 );
    a_streamSource.Read( arrData.AccessBuffer(), uSize );
    streamOut.Write( "GAR3\0\0\0\0", 8 );
    streamOut.Write( arrData.GetBuffer(), uSize );

    streamOut.Close();
    return "";
}

mCString ConvertRisen2XimgToDds( mCFileStream & a_streamSource )
{
	a_streamSource.Seek( 12 );
    if ( a_streamSource.ReadU32() != 0x34304D49 )
        return "Invalid ._xsnd file";
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".dds";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    a_streamSource.Seek( 24 );
    MIUInt uDataOffset = a_streamSource.ReadU32() + 16;
    MIUInt uDataSize = a_streamSource.ReadU32() - 8;
    mCByteArray arrData( 0, uDataSize );
    a_streamSource.Seek( uDataOffset );
    a_streamSource.Read( arrData.AccessBuffer(), uDataSize );
    streamOut.Write( arrData.GetBuffer(), uDataSize );
    streamOut.Close();
    return "";
}

int r2ximg( int argc, char* argv[] )
{
    mCString strError;
    for ( ; ; )
    {
        strError = "Please start the conversion by dragging a .dds or ._ximg file onto the executable.";
        if ( argc != 2 )
            break;
        mCFileStream streamIn, streamOut;
        strError = "Cannot open the file \"" + mCString( argv[ 1 ] ) + "\".";
        if ( streamIn.Open( argv[ 1 ], mEFileOpenMode_Read ) == mEResult_False )
            break;
        MIU32 u32Magic = 0;
        streamIn >> u32Magic;
        strError = "The input file is neither a valid .dds file nor a valid Risen 2 ._ximg file.";
        if ( u32Magic == 0x33524147 )
            strError = ConvertRisen2XimgToDds( streamIn );
        else if ( u32Magic == 0x20534444 )
            strError = ConvertDdsToRisen2Ximg( streamIn );
        streamIn.Close();
        break;
    }
    if ( strError != "" )
    {
        printf( ( "Error: " + strError + "\n" ).GetText() );
        WaitForEnterKey( MITrue );
    }
    return 0;
}