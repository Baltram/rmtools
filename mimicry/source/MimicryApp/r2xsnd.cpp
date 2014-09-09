#include <cstdio>
#include <ctime>
#include "..\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

mCString ConvertWavToRisen2Xsnd( mCFileStream & a_streamSource )
{
    MIUInt uSize = a_streamSource.GetSize();
    MIUInt uBytesPerSecond, uDataChunkSize;
    MIUInt uNextChunk = 12;
    for ( ; ; )
    {
        a_streamSource.Seek( uNextChunk );
        MIU32 u32Id = a_streamSource.ReadU32();
        MIUInt uChunkSize = a_streamSource.ReadU32();
        uNextChunk += uChunkSize + 8;
        if ( u32Id == 0x20746D66 )  // "fmt "
        {
            a_streamSource.Skip( 8 );
            a_streamSource >> g_32( uBytesPerSecond );
        }
        else if ( u32Id == 0x61746164 )  // "data"
        {
            uDataChunkSize = uChunkSize;
            break;
        }
    }
    MIUInt uDuration = static_cast< MIUInt >( ( static_cast< MIU64 >( uDataChunkSize ) * 1000 ) / uBytesPerSecond );
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              "._xsnd";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    MIU64 u64FileTime = static_cast< MIU64 >( time( 0 ) ) * 10000000 + 116444736000000000;

    streamOut.Write( "GAR3\0\0\0\0", 8 );
    streamOut.Write( "GR01", 4 );
    streamOut.Write( "SN08", 4 );
    streamOut << ( MIU32 ) 40;       // PropOffset
    streamOut << ( MIU32 ) 83;       // PropLangth
    streamOut << ( MIU32 ) 123;      // DataOffset
    streamOut << g_32( uSize + 8 );  // DataLength
    streamOut << u64FileTime;
    streamOut.Write( ".wav\0\0\0\0", 8 );

    streamOut.Write( "GEC0", 4 );
    streamOut << ( MIU32 ) 0x88E0CA06;  // Class name hash/type_info (->bCRuntimeType)?
    streamOut << ( MIU16 ) 1;
    streamOut << ( MIU32 ) 35;
    streamOut << ( MIU32 ) 30;
    streamOut << ( MIU32 ) 31;
    streamOut << ( MIU16 ) 2;  // Property count

    streamOut << ( MIU32 ) 0xB23C93CD;  // long
    streamOut << ( MIU32 ) 0x7C9663EB;
    streamOut << ( MIU32 ) 4;
    streamOut << g_32( uDuration );

    streamOut << ( MIU32 ) 0xF9CBA7A0;
    streamOut << ( MIU32 ) 0x0D2E96B2;
    streamOut << ( MIU32 ) 1;
    streamOut << ( MIU8 ) 0;

    // ClassData
    streamOut << ( MIU32 ) 0x88E0CA06 << ( MIU32 ) 1 << ( MIU16 ) 0;
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

mCString ConvertRisen2XsndToWav( mCFileStream & a_streamSource )
{
    a_streamSource.Seek( 12 );
    if ( a_streamSource.ReadU32() != 0x38304E53 )
        return "Invalid ._xsnd file";
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".wav";
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

int r2xsnd( int argc, char* argv[] )
{
	mCString strError;
    for ( ; ; )
    {
        strError = "Please start the conversion by dragging a .wav or ._xsnd file onto the executable.";
        if ( argc != 2 )
            break;
        mCFileStream streamIn, streamOut;
        strError = "Cannot open the file \"" + mCString( argv[ 1 ] ) + "\".";
        if ( streamIn.Open( argv[ 1 ], mEFileOpenMode_Read ) == mEResult_False )
            break;
        MIU32 u32Magic = 0;
        streamIn >> u32Magic;
        strError = "The input file is neither a valid .wav file nor a valid Risen 2 ._xsnd file.";
        if ( u32Magic == 0x33524147 )
            strError = ConvertRisen2XsndToWav( streamIn );
        else if ( u32Magic == 0x46464952 )
            strError = ConvertWavToRisen2Xsnd( streamIn );
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