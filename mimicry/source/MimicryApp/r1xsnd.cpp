#include <cstdio>
#include <ctime>
#include "..\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

namespace
{
    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mCString ConvertMp3ToRisen1Xsnd( mCFileStream & a_streamSource )
{
    MIUInt uSize = a_streamSource.GetSize();
	mCString strMp3FileName = a_streamSource.GetFileName();
    mCString strOutFilePath = g_GetDirectoryPath( strMp3FileName ) + 
                              "\\" +
                              g_GetFileNameNoExt( strMp3FileName ) +
                              "._xsnd";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
	
    MIU16 const u16PropertySystemVersion = 201, u16PropertyVersion = 30;
	streamOut << "GR01SN04" << ( MIU32 ) 40 << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 0;
    streamOut << g_time() << ".wav" << ( MIU32 ) 0;
    streamOut << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( streamOut, "eCSoundResource2" );
    streamOut << ( MIU8 )( 1 ) << ( MIU16 )( 0 );
    streamOut << u16PropertySystemVersion << u16PropertySystemVersion;
    streamOut << ( MIU32 ) 0;  // @71
    streamOut << u16PropertySystemVersion << ( MIU32 ) 3;
    WriteString( streamOut, "Duration" );
    WriteString( streamOut, "long" );
    streamOut << u16PropertyVersion << ( MIU32 )( sizeof( MIU32 ) ) << ( MIU32 )( 0 );  // ToDo: Proper Duration
	MIU8 u8Flags = strMp3FileName.Contains( "%_2d" ) ? 3 : ( strMp3FileName.Contains( "%_sw" ) ? 2 : 0 );
    WriteString( streamOut, "Flags" );
    WriteString( streamOut, "char" );
    streamOut << u16PropertyVersion << ( MIU32 )( sizeof( MIU8 ) ) << u8Flags;
    WriteString( streamOut, "Links" );
    WriteString( streamOut, "bTObjArray<struct eCSoundResource2::SLink>" );
    streamOut << u16PropertyVersion << ( MIU32 ) 5;
    streamOut << ( MIU8 ) 1 << ( MIU32 ) 0;  // ToDo: Support links?
    streamOut << u16PropertySystemVersion;

    MIUInt const uDataOffset = streamOut.Tell();
    mCByteArray arrData( 0, uSize );
    a_streamSource.Seek( 0 );
    a_streamSource.Read( arrData.AccessBuffer(), uSize );
    streamOut.Write( arrData.GetBuffer(), uSize );
    MIUInt const uOffsetEnd = streamOut.Tell();

    streamOut.Seek( 12 );
    streamOut << g_32( uDataOffset - 40 ) << g_32( uDataOffset ) << g_32( uOffsetEnd - uDataOffset );
    streamOut.Seek( 71 );
    streamOut << g_32( uDataOffset - 75 );
    streamOut.Seek( uOffsetEnd );

    streamOut.Close();
    return "";
}

mCString ConvertRisen1XsndToMp3( mCFileStream & a_streamSource )
{
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".mp3";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    a_streamSource.Seek( 16 );
    MIUInt uDataOffset = a_streamSource.ReadU32();
    MIUInt uDataSize = a_streamSource.ReadU32();
    mCByteArray arrData( 0, uDataSize );
    a_streamSource.Seek( uDataOffset );
    a_streamSource.Read( arrData.AccessBuffer(), uDataSize );
    streamOut.Write( arrData.GetBuffer(), uDataSize );
    streamOut.Close();
    return "";
}

int r1xsnd( int argc, char* argv[] )
{
	mCString strError;
    for ( ; ; )
    {
        strError = "Please start the conversion by dragging a .mp3 or ._xsnd file onto the executable.";
        if ( argc != 2 )
            break;
        mCFileStream streamIn, streamOut;
        strError = "Cannot open the file \"" + mCString( argv[ 1 ] ) + "\".";
        if ( streamIn.Open( argv[ 1 ], mEFileOpenMode_Read ) == mEResult_False )
            break;
        MIU64 u64Magic = 0;
        streamIn >> u64Magic;
        strError = "The input file is neither a valid .mp3 file nor a valid Risen 1 ._xsnd file.";
        if ( u64Magic == 0x34304E5331305247 )
            strError = ConvertRisen1XsndToMp3( streamIn );
		else if ( mCString( argv[ 1 ] ).Lower().EndsWith( ".mp3" ) )
            strError = ConvertMp3ToRisen1Xsnd( streamIn );
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