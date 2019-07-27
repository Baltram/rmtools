#include <cstdio>
#include <ctime>
#include "..\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

mCString ConvertWavToRisen2Xdlg( mCFileStream & a_streamSource )
{
    MIUInt const uSize = a_streamSource.GetSize();
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
    MIUInt const uDuration = static_cast< MIUInt >( ( static_cast< MIU64 >( uDataChunkSize ) * 1000 ) / uBytesPerSecond );
    mCString strDlgFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              "._xdlg";
    mCMemoryStream streamDlg;
    if ( streamDlg.FromFile( strDlgFilePath ) == mEResult_False )
        return "Cannot open the file \"" + strDlgFilePath + "\".";
    mCMemoryStream streamDlgNew;
    MIU64 const u64FileTime = static_cast< MIU64 >( time( 0 ) ) * 10000000 + 116444736000000000;
    streamDlg.Seek( 24 );
    MIUInt const uDataOffset = streamDlg.ReadU32() + 8;
    MIUInt const uDataSizeOld = streamDlg.ReadU32();
    streamDlg.Seek( uDataOffset + 8 );
    MIUInt const uWavSizeOld = streamDlg.ReadU32();
    streamDlgNew.Write( streamDlg.GetBuffer(), uDataOffset + 8 );
    streamDlgNew << g_32( uSize );
    streamDlgNew << a_streamSource;
    streamDlgNew.Write( static_cast< MILPCByte >( streamDlg.GetBuffer() ) + uDataOffset + 12 + uWavSizeOld, streamDlg.GetSize() - ( uDataOffset + 12 + uWavSizeOld ) );
    streamDlgNew.Seek( 28 );
    streamDlgNew << g_32( uDataSizeOld + uSize - uWavSizeOld );
    streamDlgNew << u64FileTime;
    if ( streamDlgNew.ToFile( strDlgFilePath ) == mEResult_False )
        return "Cannot write to file \"" + strDlgFilePath + "\".";
    return "";
}

mCString ConvertRisen2XdlgToWav( mCFileStream & a_streamSource )
{
    a_streamSource.Seek( 12 );
    if ( a_streamSource.ReadString( 4 ) != "DI08" )
        return "Invalid ._xdlg file";
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".wav";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    a_streamSource.Seek( 24 );
    MIUInt uDataOffset = a_streamSource.ReadU32() + 8;
    a_streamSource.Seek( uDataOffset + 8 );
    MIUInt uSize = a_streamSource.ReadU32();
    mCByteArray arrData( 0, uSize );
    a_streamSource.Read( arrData.AccessBuffer(), uSize );
    streamOut.Write( arrData.GetBuffer(), uSize );
    streamOut.Close();
    return "";
}

int r2xdlg( int argc, char* argv[] )
{
	mCString strError;
    for ( ; ; )
    {
        strError = "Please start the conversion by dragging a .wav or ._xdlg file onto the executable.";
        if ( argc != 2 )
            break;
        mCFileStream streamIn, streamOut;
        strError = "Cannot open the file \"" + mCString( argv[ 1 ] ) + "\".";
        if ( streamIn.Open( argv[ 1 ], mEFileOpenMode_Read ) == mEResult_False )
            break;
        MIU32 u32Magic = 0;
        streamIn >> u32Magic;
        strError = "The input file is neither a valid .wav file nor a valid Risen 2 ._xdlg file.";
        if ( u32Magic == 0x33524147 )
            strError = ConvertRisen2XdlgToWav( streamIn );
        else if ( u32Magic == 0x46464952 )
            strError = ConvertWavToRisen2Xdlg( streamIn );
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