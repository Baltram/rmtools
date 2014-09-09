#include <cstdio>
#include <ctime>
#include "..\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

MIUInt HandleUnicodeString( void * a_pDest )
{
    MIUInt uResult = 0;
    MIByte * pDest = static_cast< MIByte * >( a_pDest );
    while ( *pDest++ )
    {
        if ( *pDest == 0x00 )
            *pDest = 0xAA;
        else if ( *pDest == 0xAA )
            *pDest = 0x00;
        ++pDest;
        ++uResult;
    }
    return uResult;
}

void ReadTabString( mCIOStreamBinary & a_streamSource, mCString & a_strDest, MIU16 a_u16IsUnicode )
{
    MIUInt uSize = a_streamSource.ReadU16() << a_u16IsUnicode;
    a_strDest.SetText( ( MILPCChar ) 0, uSize );
    a_streamSource.Read( a_strDest.AccessText(), uSize );
    if ( a_u16IsUnicode )
    {
        HandleUnicodeString( a_strDest.AccessText() );
        a_strDest.Replace( "\t\xAA", "\\\xAAt\xAA" );
    }
    else
        a_strDest.Replace( "\t", "\\t" );
}

void WriteTabString( mCIOStreamBinary & a_streamDest, mCString a_strSource, MIU16 a_u16IsUnicode )
{
    if ( a_u16IsUnicode )
        a_strSource.Replace( "\\\xAAt\xAA", "\t\xAA" );
    else
        a_strSource.Replace( "\\t", "\t" );
    MIUInt uSize = a_u16IsUnicode ? HandleUnicodeString( a_strSource.AccessText() ) : a_strSource.GetLength();
    a_streamDest << static_cast< MIU16 >( uSize );
    a_streamDest.Write( a_strSource.GetText(), uSize << a_u16IsUnicode );
}

void ReadCsvString( mCIOStreamBinary & a_streamSource, mCString & a_strDest, MIU16 a_u16IsUnicode, MIBool & a_bIsLastColumn )
{
    a_bIsLastColumn = MIFalse;
    MILPChar pcBuffer = mCString::AccessStaticBuffer();
    MIUInt uSize = 0;
    MIUInt uEnd = a_streamSource.GetSize();
    if ( a_u16IsUnicode )
    {
        MIU16 * pIt = reinterpret_cast< MIU16 * >( pcBuffer );
        --uEnd;
        while ( a_streamSource.Tell() < uEnd )
        {
            a_streamSource >> *pIt;
            if ( ( a_bIsLastColumn = ( *pIt == '\t' ) ) || ( *pIt == '\n' ) )
                break;
            ++pIt;
        }
        *pIt = 0;
        uSize = HandleUnicodeString( pcBuffer ) * 2;
    }
    else
    {
        MILPChar pcIt = pcBuffer;
        while ( a_streamSource.Tell() < uEnd )
        {
            a_streamSource >> *pcIt;
            if ( ( a_bIsLastColumn = ( *pcIt == '\t' ) ) || ( *pcIt == '\n' ) )
                break;
            ++pcIt;
        }
        *pcIt = 0;
        uSize = static_cast< MIUInt >( pcIt - pcBuffer );
    }
    a_bIsLastColumn = !a_bIsLastColumn;
    a_strDest.SetText( pcBuffer, uSize );
    a_strDest.Replace( a_u16IsUnicode ? "\r\xAA" : "\r", "" );
}

void WriteCsvString( mCIOStreamBinary & a_streamDest, mCString a_strSource, MIU16 a_u16IsUnicode, MIBool a_bIsLastColumn )
{
    MIUInt uSize = a_u16IsUnicode ? HandleUnicodeString( a_strSource.AccessText() ) * 2 : a_strSource.GetLength();
    a_streamDest.Write( a_strSource.GetText(), uSize );
    if ( a_u16IsUnicode )
        a_streamDest.Write( a_bIsLastColumn ? L"\r\n" : L"\t", a_bIsLastColumn ? 4 : 2 );
    else
        a_streamDest << ( a_bIsLastColumn ? "\r\n" : "\t" );
}

mCString ConvertCsvToRisen2Tab( mCFileStream & a_streamSource )
{
    a_streamSource.Seek( 0 );
    MIU16 u16IsUnicode = ( a_streamSource.ReadU16() == 0xFEFF ) ? 1 : 0;
    a_streamSource.Seek( u16IsUnicode ? 2 : 0 );
    mTArray< mTArray< mCString > > arrColumns;
    MIBool bIsLastColumn = MIFalse;
    while( !bIsLastColumn )
        ReadCsvString( a_streamSource, arrColumns.AddNew().AddNew(), u16IsUnicode, bIsLastColumn );
    for ( MIUInt uEnd = a_streamSource.GetSize(), uLine = 2; a_streamSource.Tell() != uEnd; ++uLine )
    {
        bIsLastColumn = MIFalse;
        for ( MIUInt u = 0, ue = arrColumns.GetCount(); u != ue; ++u )
        {
            if ( bIsLastColumn )
                return mCString().Format( "\"%s\": Missing record(s) at line %u.", a_streamSource.GetFileName().GetText(), uLine );
            ReadCsvString( a_streamSource, arrColumns[ u ].AddNew(), u16IsUnicode, bIsLastColumn );
        }
        if ( !bIsLastColumn )
            return mCString().Format( "\"%s\": Abundant record(s) at line %u.", a_streamSource.GetFileName().GetText(), uLine );
    }
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".tab0";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    MIU64 u64FileTime = static_cast< MIU64 >( time( 0 ) ) * 10000000 + 116444736000000000;
    streamOut.Write( "TAB1", 4 );
    streamOut << ( MIU16 ) 2;
    streamOut << u16IsUnicode;
    streamOut << u64FileTime;
    streamOut << g_32( arrColumns.GetCount() );
    for ( MIUInt u = 0, ue = arrColumns.GetCount(); u != ue; ++u )
    {
        streamOut << ( MIU8 ) 1;
        streamOut << ( MIU16 ) 1;
        WriteTabString( streamOut, arrColumns[ u ][ 0 ], u16IsUnicode );
        streamOut << g_32( arrColumns[ u ].GetCount() - 1 );
        for ( MIUInt v = 1, ve = arrColumns[ u ].GetCount(); v != ve; ++v )
            WriteTabString( streamOut, arrColumns[ u ][ v ], u16IsUnicode );
    }
    streamOut.Close();
    return "";
}

mCString ConvertRisen2TabToCsv( mCFileStream & a_streamSource )
{
    mTArray< mTArray< mCString > > arrColumns;
    a_streamSource.Seek( 4 );
    if ( a_streamSource.ReadU16() != 2 )
        return "Unknown .tab file version.";
    MIU16 u16IsUnicode = g_max< MIU16 >( a_streamSource.ReadU16(), 1 );
    a_streamSource.Skip( 8 );
    for ( MIUInt u = a_streamSource.ReadU32(); u--; )
    {
        mTArray< mCString > & arrStrings = arrColumns.AddNew();
        if ( !a_streamSource.ReadU8() )
            continue;
        a_streamSource.Skip( 2 );
        ReadTabString( a_streamSource, arrStrings.AddNew(), u16IsUnicode );
        for ( MIUInt u = a_streamSource.ReadU32(); u--; )
            ReadTabString( a_streamSource, arrStrings.AddNew(), u16IsUnicode );
    }
    mCString strOutFilePath = g_GetDirectoryPath( a_streamSource.GetFileName() ) + 
                              "\\" +
                              g_GetFileNameNoExt( a_streamSource.GetFileName() ) +
                              ".csv";
    mCFileStream streamOut;
    if ( streamOut.Open( strOutFilePath, mEFileOpenMode_Write ) == mEResult_False )
        return "Cannot open/create the file \"" + strOutFilePath + "\".";
    MIUInt uColumnSize = 0;
    streamOut << ( MIU16 ) 0xFEFF;
    for ( MIUInt u = arrColumns.GetCount(); u--; uColumnSize = g_max( uColumnSize, arrColumns[ u ].GetCount() ) );
    for ( MIUInt u = arrColumns.GetCount(); u--; arrColumns[ u ].Resize( uColumnSize ) );
    for ( MIUInt u = 0; u != uColumnSize; ++u )
        for ( MIUInt v = 0, ve = arrColumns.GetCount(); v != ve; ++v )
            WriteCsvString( streamOut, arrColumns[ v ][ u ], u16IsUnicode, ( v + 1 == ve ) );
    streamOut.Close();
    return "";
}

MIBool InitCsvFile( mCFileStream & a_streamCsv )
{
    a_streamCsv.Seek( 0 );
    MIU16 u16BOM = a_streamCsv.ReadU16();
    if ( u16BOM == 0xFFFE )
        a_streamCsv.SetInvertEndianness( MITrue );
    //else if ( u16BOM != 0xFEFF )
    //    return MIFalse;
    return MITrue;
}

int r2tab( int argc, char* argv[] )
{
    mCString strError;
    for ( ; ; )
    {
        strError = "Please start the conversion by dragging a .tab or .csv file onto the executable.";
        if ( argc != 2 )
            break;
        mCFileStream streamIn, streamOut;
        strError = "Cannot open the file \"" + mCString( argv[ 1 ] ) + "\".";
        if ( streamIn.Open( argv[ 1 ], mEFileOpenMode_Read ) == mEResult_False )
            break;
        strError = "The input file is neither a compatible Unicode .csv file (tab stop as separator) nor a valid Risen 2 .tab file.";
        if ( streamIn.ReadU32() == 0x31424154 )
            strError = ConvertRisen2TabToCsv( streamIn );
        else if ( InitCsvFile( streamIn ) )
            strError = ConvertCsvToRisen2Tab( streamIn );
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