#include <cstdio>
#include "..\Mimicry.h"
#include "..\Mimicry\mi_include_risen.h"
#include "Windows.h"
#include "..\..\..\zlib\zlib.h"

namespace
{
    MIBool s_bDebug = MIFalse;
    MIBool s_bNonInteractive = MIFalse;
    MIBool s_bFsb = MIFalse;
    MIUInt s_uGeneration = MI_DW_INVALID;
}

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse, MIBool a_bExit = MITrue )
{
    if ( s_bNonInteractive )
        return;
    if ( a_bPrintDefaultMessage )
        printf( a_bExit ? "Press ENTER to exit... " : "Press ENTER to continue... " );
    for ( MIChar c = 0; c != '\n' && c != EOF; c = getchar() );
}

void ShowProgress( MIUInt a_uCurrent, MIUInt a_uTotal )
{
    static MIU64 u64Time = 0;
    if ( g_time() - u64Time >= 100000000 || a_uCurrent * 10 / a_uTotal != ( a_uCurrent - 1 ) * 10 / a_uTotal )
        u64Time = g_time(), printf( "%u%%\n", a_uCurrent * 100 / a_uTotal );
}

MIU64 SwapHighLow( MIU64 a_u64Time )
{
    return ( a_u64Time << 32 ) | ( a_u64Time >> 32 );
}

MIBool CreateFiles( mCString const & a_strDirectory, mCGenomeVolume::CDir const & a_Dir, mCString a_strRootDirectory = "" )
{
    // a_strDirectory ends not with '/' or '\'
    if ( a_strRootDirectory == "" )
        a_strRootDirectory = a_strDirectory;
    static MIUInt s_uCreatedFileCount = 0, s_uTotalFileCount = 0;
    if ( a_strRootDirectory == a_strDirectory )
    {
        s_uCreatedFileCount = 0;
        s_uTotalFileCount = a_Dir.GetTotalFileCount();
    }
    DWORD dwAttrib = GetFileAttributesA( a_strDirectory.GetText() );
    if ( dwAttrib == INVALID_FILE_ATTRIBUTES )
    {
        if ( !CreateDirectoryA( a_strDirectory.GetText(), NULL ) )
            return MIFalse;
    }
    for ( MIUInt u = 0, ue = a_Dir.GetFiles().GetCount(); u != ue; ++u )
    {
        mCGenomeVolume::CFileHandle const & File = a_Dir.GetFiles()[ u ];
        mCString const strFilePath = a_strDirectory + "\\" + g_GetFileName( File.GetFilePath() );
        mCFileStream streamOut( strFilePath, mEFileOpenMode_Write );
        if ( !streamOut.IsOpen() )
            return MIFalse;
        File.GetFileData( streamOut );
        streamOut.Close();
        ++s_uCreatedFileCount;
        ShowProgress( s_uCreatedFileCount, s_uTotalFileCount );
        HANDLE hFile = CreateFileA( ( strFilePath ).GetText(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( hFile == INVALID_HANDLE_VALUE )
            return MIFalse;
        FILETIME CreationTime, LastAccessTime, LastWriteTime;
        *reinterpret_cast< MIU64 * >( &CreationTime ) = File.GetCreationTime();
        *reinterpret_cast< MIU64 * >( &LastAccessTime ) = File.GetLastAccessTime();
        *reinterpret_cast< MIU64 * >( &LastWriteTime ) = File.GetLastWriteTime();
        SetFileTime( hFile, &CreationTime, &LastAccessTime, &LastWriteTime );
        CloseHandle( hFile );
    }
    for ( MIUInt u = 0, ue = a_Dir.GetSubDirs().GetCount(); u != ue; ++u )
        if ( !CreateFiles( a_strDirectory + "\\" + a_Dir.GetSubDirs()[ u ].GetName(), a_Dir.GetSubDirs()[ u ], a_strRootDirectory ) )
            return MIFalse;
    HANDLE hDir = CreateFileA( a_strDirectory.GetText(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
    if ( hDir != INVALID_HANDLE_VALUE )
    {
        FILETIME CreationTime, LastAccessTime, LastWriteTime;
        *reinterpret_cast< MIU64 * >( &CreationTime ) = a_Dir.GetCreationTime();
        *reinterpret_cast< MIU64 * >( &LastAccessTime ) = a_Dir.GetLastAccessTime();
        *reinterpret_cast< MIU64 * >( &LastWriteTime ) = a_Dir.GetLastWriteTime();
        if ( CreationTime.dwHighDateTime != 0 )
            SetFileTime( hDir, &CreationTime, &LastAccessTime, &LastWriteTime );
    }
    CloseHandle( hDir );
    return MITrue;
}

MIBool GetFileTime( mCString const & a_strFilePath, mCGenomeVolume::SFileTime & a_TimeDest )
{
    a_TimeDest.m_u64Created = a_TimeDest.m_u64Accessed = a_TimeDest.m_u64Modified = g_time();
    HANDLE hFile = CreateFileA( a_strFilePath.GetText(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if ( hFile != INVALID_HANDLE_VALUE )
        return GetFileTime( hFile, reinterpret_cast< LPFILETIME >( &a_TimeDest.m_u64Created ), reinterpret_cast< LPFILETIME >( &a_TimeDest.m_u64Accessed ), reinterpret_cast< LPFILETIME >( &a_TimeDest.m_u64Modified ) ) ? MITrue : MIFalse;
    return MIFalse;
}

void AddFiles( mCString const & a_strDirectoryPath, mTArray< mCString > & a_arrFilePaths, mTArray< mCGenomeVolume::SFileTime > & a_arrFileTimes )
{
    WIN32_FIND_DATAA FindInfo;
    HANDLE hFind = FindFirstFileA( ( a_strDirectoryPath + "\\*" ).GetText(), &FindInfo );
    if ( hFind == INVALID_HANDLE_VALUE )
        return;
    do
    {
        if ( FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            if ( mCString( FindInfo.cFileName ).TrimLeft( '.' ) != "" )
                AddFiles( a_strDirectoryPath + "\\" + g_GetFileName( FindInfo.cFileName ), a_arrFilePaths, a_arrFileTimes );
        }
        else
        {
            a_arrFilePaths.Add( a_strDirectoryPath + "\\" + FindInfo.cFileName );
            mCGenomeVolume::SFileTime & Time = a_arrFileTimes.AddNew();
            *reinterpret_cast< FILETIME * >( &Time.m_u64Created ) = FindInfo.ftCreationTime;
            *reinterpret_cast< FILETIME * >( &Time.m_u64Accessed ) = FindInfo.ftLastAccessTime;
            *reinterpret_cast< FILETIME * >( &Time.m_u64Modified ) = FindInfo.ftLastWriteTime;
        }
    } while ( FindNextFileA( hFind, &FindInfo ) );
    FindClose( hFind );
}

MIUInt RequestGeneration( void )
{
    if ( s_uGeneration != MI_DW_INVALID )
        return s_uGeneration;
    MIUInt uResult = 10;
    for ( ; uResult > 9 || uResult == 0; )
    {
        printf( "Please enter archive generation (1-9)\n" );
        MI_CRT_NO_WARNINGS( scanf( "%u", &uResult ); )
        for ( MIChar c = 0; c != '\n' && c != EOF; c = getchar() );
    }
    return uResult;
}

MIBool CreateRisen3Volume( mCString a_strDirectoryPath )
{
    mTArray< mCString > arrFilePaths;
    mTArray< mCGenomeVolume::SFileTime > arrFileTimes;
    a_strDirectoryPath.Replace( '/', '\\' );
    a_strDirectoryPath.TrimRight( "\\" );
    AddFiles( a_strDirectoryPath, arrFilePaths, arrFileTimes );
    mCString strVolumePath;
    if ( !mCGenomeVolume::CreateRisen3Archive( a_strDirectoryPath, arrFilePaths, arrFileTimes, &RequestGeneration, &strVolumePath, &ShowProgress ) )
    {
        if ( mCError::GetLastError< mCError >() )
            printf( "Error: %s\n", mCError::GetLastError< mCError >()->GetText() );
        return MIFalse;
    }
    return printf( "Successfully created %s.\n", strVolumePath.GetText() ), MITrue;
}

namespace
{
    MIBool ProcessDir( mCFileStream & a_streamArchive, MIUInt uNewGeneration )
    {
        MIUInt uNameLength = a_streamArchive.ReadU32();
        a_streamArchive.Skip( 8 * 3 + 4 + uNameLength + ( uNameLength ? 1 : 0 ) );
        for ( MIUInt u = a_streamArchive.ReadU32(); u--; )
        {
            if ( 0x10 & a_streamArchive.ReadU32() )
            {
                if ( ProcessDir( a_streamArchive, uNewGeneration ) )
                    continue;
                return MIFalse;
            }
            uNameLength = a_streamArchive.ReadU32();
            mCString strName = a_streamArchive.ReadString( uNameLength );
            if ( strName.Scan( "w_%*3c_%*u_%c", mCString::AccessStaticBuffer() ) != 1 )
                return MIFalse;
            a_streamArchive.Skip( -static_cast< MIInt >( uNameLength ) + 6 );
            a_streamArchive << ( MIChar )( uNewGeneration + '0' );
            a_streamArchive.Skip( uNameLength - 7 + 1 + 4 * 8 + 5 * 4 );
        }
        return MITrue;
    }
}

MIBool UpdateArchiveGeneration( mCString const & a_strPath )
{
    MIUInt uNewGeneration = 0;
    if ( g_GetFileName( a_strPath ).Scan( "%*u_%*2c_%*3c.pak.%u", &uNewGeneration ) != 1 || uNewGeneration > 9 )
        return printf( "Error: Invalid archive file name.\n" ), MIFalse;
    mCString strPathNew = g_GetDirectoryPath( a_strPath ) + mCString().Format( "\\%u", uNewGeneration ) + g_GetFileName( a_strPath ).TrimLeft( "0123456789" ).TrimRight( ".0123456789" );
    if ( strPathNew.Lower() != a_strPath.Lower() && !MoveFileA( a_strPath.GetText(), strPathNew.GetText() ) )
        return printf( "Error: Could not rename %s to %s\n", g_GetFileName( a_strPath ).GetText(), g_GetFileName( strPathNew ).GetText() ), MIFalse;
    mCFileStream streamArchive( strPathNew, mEFileOpenMode_ReadWrite );
    if ( !streamArchive.IsOpen() )
        return printf( "Error: Could not write to %s.\n", g_GetFileName( strPathNew ).GetText() ), MIFalse;
    if ( streamArchive.ReadU32() != 1 || streamArchive.ReadString( 4 ) != "G3V0" )
        return printf( "Error: Invalid archive file structure.\n" ), MIFalse;
    streamArchive.Seek( 32 );
    streamArchive.Seek( streamArchive.ReadU32() );
    if ( !ProcessDir( streamArchive, uNewGeneration ) )
    {
        streamArchive.Close();
        MoveFileA( strPathNew.GetText(), a_strPath.GetText() );
        return printf( "Error: Invalid archive file structure.\n" ), MIFalse;
    }
    return printf( "Successfully updated archive generation to %u.\n", uNewGeneration ), MITrue;
}

MIBool WrlToWrlDoc( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCMemoryStream streamDoc;
    mCRisenDoc Doc( a_streamIn, streamDoc );
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset;
    a_streamIn >> u32Data1Offset;
    a_streamIn.Seek( u32Data1Offset );
    if ( !Doc.DocumentElexWorld() )
    {
        printf( "Error: Unknown world file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".elexwrldoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    return MITrue;
}

MIBool WrlDocToWrl( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strWrlPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elexwrl";
    mCMemoryStream streamWrl;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamWrl, MITrue );
    if ( !Parser.ParseElexWorld( MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", Parser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCFileStream streamDest( strWrlPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strWrlPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    streamDest << "E1RF" << ( MIU32 )( streamWrl.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamWrl;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell();
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamWrl.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class gCWorldResource" ) << "WR02";
    streamDest << g_32( 104 + strResourceName.GetLength() );
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    streamDest << ( MIU32 ) 20 << "GEC2" << mCRisenName( "class gCWorldResource" ) << ( MIU16 ) 1 << ( MIU32 ) 6 << ( MIU16 ) 0 << ( MIU32 ) 0;
    return MITrue;
}

MIBool ImgToDds( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    a_streamIn >> u32Data1Offset >> u32Data1Size;
    a_streamIn.Seek( u32Data1Offset );
    mCString const strDDSPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".dds";
    mCFileStream streamDDS( strDDSPath, mEFileOpenMode_Write );
    if ( !streamDDS.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDDSPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCByteArray arrBuffer;
    arrBuffer.Resize( u32Data1Size );
    a_streamIn.Read( arrBuffer.AccessBuffer(), u32Data1Size );
    streamDDS.Write( arrBuffer.GetBuffer(), u32Data1Size );
    return MITrue;
}

MIBool SndDlgToFsb( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut )
{
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    a_streamIn >> u32Data1Offset >> u32Data1Size;
    a_streamIn.Seek( u32Data1Offset );
    mCByteArray arrBuffer;
    arrBuffer.Resize( u32Data1Size );
    a_streamIn.Read( arrBuffer.AccessBuffer(), u32Data1Size );
    a_streamOut.Write( arrBuffer.GetBuffer(), u32Data1Size );
    return MITrue;
}

MIBool TplToTplDoc( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    a_streamIn >> u32Data1Offset >> u32Data1Size;
    a_streamIn.Seek( u32Data1Offset );
    mCMemoryStream streamDoc;
    mCRisenDoc Doc( a_streamIn, streamDoc );
    if ( !Doc.DocumentRisen3Template( MITrue ) )
    {
        printf( "Error: Unknown template file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".elextpldoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    return MITrue;
}

MIBool SecToSecDoc( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCMemoryStream streamDoc;
    mCRisenDoc Doc( a_streamIn, streamDoc );
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    a_streamIn >> u32Data1Offset >> u32Data1Size;
    a_streamIn.Skip( 1 + 3 * ( 4 + 4 + 1 ) + 3 * 4 + 4 * 8 );
    a_streamIn.ReadString( a_streamIn.ReadU32() );
    a_streamIn.Skip( 4 * ( 4 + 4 + 1 ) );
    MIUInt uReferencedFileCount = a_streamIn.ReadU32();
    a_streamIn.Skip( uReferencedFileCount * 8 );
    for ( ; uReferencedFileCount--; a_streamIn.ReadString( a_streamIn.ReadU32() ) );
    a_streamIn.Skip( 2 * 4 );
    if ( 0 == Doc.DocumentRisen3Class() || ( a_streamIn.Seek( u32Data1Offset ), Doc.WriteLine(), !Doc.DocumentRisen3Sector( MITrue ) ) )
    {
        printf( "Error: Unknown sector file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".elexsecdoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    return MITrue;
}

MIBool HdrToHdrDoc( mCIOStreamBinary & streamIn, mCString const & a_strFilePath )
{
    if ( streamIn.ReadString( 4 ) != "GAR5" )
        return streamIn.Seek( 0 ), MIFalse;
    streamIn.ReadU32();  // 0x00000020
    mCMemoryStream streamDoc;
    mCRisenDoc Doc( streamIn, streamDoc );
    if ( !Doc.DocumentRisen3Hdr() )
    {
        printf( "Error: Unknown hdr file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".hdrdoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    return MITrue;
}

MIBool HdrDocToHdr( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3HdrPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".hdr";
    mCMemoryStream streamHdr;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamHdr, MITrue );
    if ( !Parser.ParseRisen3Hdr( MITrue, MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", Parser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCFileStream streamDest( strR3HdrPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3HdrPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    streamDest << "GAR5" << g_32( 0x00000020 );
    streamDest << streamHdr;
    return MITrue;
}

int ResolveRisen3Hashes( void )
{
    mCRisenName::InitializeGameStrings();
    while ( MITrue )
    {
        MIU32 u32Hash = 0;
        printf( "Enter hash.\n" );
        MI_CRT_NO_WARNINGS( scanf( "%x", &u32Hash ); )
        for ( MIChar c = 0; c != '\n' && c != EOF; c = getchar() );
        if ( !u32Hash )
            continue;
        mCRisenName Name( u32Hash );
        if ( !Name.IsValid() )
            printf( "Unknown hash\n" );
        else
            printf( "%s\n", Name.GetString().GetText() );
    }
    return 0;
}

namespace
{
    struct SFrame
    {
        MIUInt m_uPosition;
        MIUInt m_uSize;
        MIUInt m_uSamplingRate;
        MIUInt m_uBitRate;
        MIBool m_bMono;
    };

    MIBool ReadMp3FrameHeader( mCIOStreamBinary & a_streamIn, SFrame & a_frameDest )  // Assumes a_streamIn.SetInvertEndianness( MITrue ) has been called
    {
        MIUInt const uPosition = a_streamIn.Tell();
        MIU32 u32Header = a_streamIn.ReadU32();
        MIUInt uFrameSync    = ( u32Header & ( ( ( 1 << 11 ) - 1 ) << 21 ) ) >> 21;
        MIUInt uMpegId       = ( u32Header & ( ( ( 1 <<  2 ) - 1 ) << 19 ) ) >> 19;
        MIUInt uLayerDesc    = ( u32Header & ( ( ( 1 <<  2 ) - 1 ) << 17 ) ) >> 17;
        MIUInt uBitrateId    = ( u32Header & ( ( ( 1 <<  4 ) - 1 ) << 12 ) ) >> 12;
        MIUInt uFreqId       = ( u32Header & ( ( ( 1 <<  2 ) - 1 ) << 10 ) ) >> 10;
        MIUInt uPaddingFlag  = ( u32Header & ( ( ( 1 <<  1 ) - 1 ) <<  9 ) ) >>  9;
        MIUInt uChannelMode  = ( u32Header & ( ( ( 1 <<  2 ) - 1 ) <<  6 ) ) >>  6;
        if ( uFrameSync != 0x7FF || uMpegId < 2 || uLayerDesc != 1 )  // Only Layer III MPEG v1 or v2 supported
             return MIFalse;
        MIUInt arrSamplingRates[] = { 44100, 48000, 32000 };
        MIUInt arrKBitRates1[] = { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 };  // 0 means invalid
        MIUInt arrKBitRates2[] = { 0,  8, 16, 24, 32, 40, 48, 56,  64,  80,  96, 112, 128, 144, 160, 0 };  // 0 means invalid
        a_frameDest.m_uPosition = uPosition;
        a_frameDest.m_uSamplingRate = arrSamplingRates[ uFreqId ] / ( uMpegId == 3 ? 1 : 2 );
        a_frameDest.m_uBitRate = 1000 * ( uMpegId == 3 ? arrKBitRates1[ uBitrateId ] : arrKBitRates2[ uBitrateId ] );
        a_frameDest.m_bMono = uChannelMode == 3;
        a_frameDest.m_uSize = ( 144 * a_frameDest.m_uBitRate ) / a_frameDest.m_uSamplingRate + uPaddingFlag;
        return MITrue;
    }
}

MIBool Mp3ToFsb( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut )
{
    a_streamIn.SetInvertEndianness( MITrue );
    // Skip ID3v2-Header
    while ( a_streamIn.ReadString( 3 ) == "ID3" )
    {
        a_streamIn.Skip( 3 );
        MIU32 u32SizeSynchSafe = a_streamIn.ReadU32();
        MIUInt uHeaderSize = ( ( u32SizeSynchSafe & 0x0000007F ) >> 0 ) +
                             ( ( u32SizeSynchSafe & 0x00007F00 ) >> 1 ) +
                             ( ( u32SizeSynchSafe & 0x007F0000 ) >> 2 ) +
                             ( ( u32SizeSynchSafe & 0x7F000000 ) >> 3 );
        a_streamIn.Skip( uHeaderSize );
    }
    a_streamIn.Skip( -3 );
    // Extract frames but skip first 'info' frame if present
    mTArray< SFrame > arrFrames;
    for ( MIUInt uPosition = a_streamIn.Tell(), uSize = a_streamIn.GetSize(); uPosition + 4 <= uSize; uPosition = a_streamIn.Tell() )
    {
        SFrame Frame;
        if ( !ReadMp3FrameHeader( a_streamIn, Frame ) )
            break;
        arrFrames.Add( Frame );
        // Skip initial info frame(s) if present
        if ( arrFrames.GetCount() == 1 && Frame.m_uSize >= 40 )
        {
            a_streamIn.Skip( Frame.m_bMono ? 17 : 32 );
            mCString strMagic = a_streamIn.ReadString( 4 );
            if ( strMagic == "Info" || 
                 strMagic == "LAME" || 
                 strMagic == "Xing" || 
                 strMagic == "VBRI" )
                arrFrames.RemoveAt( 0 );
        }
        if ( Frame.m_uSize == 0 )
            break;
        a_streamIn.Seek( uPosition + Frame.m_uSize );
    }
    if ( arrFrames.GetCount() == 0 )
    {
        printf( "Error: Unsupported .mp3 file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    MIUInt uFrequencyId;
    switch ( arrFrames.Front().m_uSamplingRate )
    {
    case 48000:
        uFrequencyId = 9; break;
    case 44100:
        uFrequencyId = 8; break;
    case 32000:
        uFrequencyId = 7; break;
    case 24000:
        uFrequencyId = 6; break;
    case 22050:
        uFrequencyId = 5; break;
    case 16000:
        uFrequencyId = 4; break;
    }
    a_streamOut << "FSB5" << ( MIU32 ) 1 << ( MIU32 ) 1 << ( MIU32 ) 36 << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 11 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0;  // FSB5 header
    a_streamOut << g_32( ( uFrequencyId << 1 ) + ( ( arrFrames.Front().m_bMono ? 0 : 1 ) << 5 ) );
    a_streamOut << g_32( ( 1152 * arrFrames.GetCount() ) << 2 );
    a_streamOut << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    mCByteArray arrBuffer;
    MIU32 const u32Zero = 0;
    for ( MIUInt u = 0, ue = arrFrames.GetCount(); u != ue; ++u )
    {
        MIUInt const uFrameSize = arrFrames[ u ].m_uSize;
        a_streamIn.Seek( arrFrames[ u ].m_uPosition );
        arrBuffer.Resize( uFrameSize );
        a_streamIn.Read( arrBuffer.AccessBuffer(), uFrameSize );
        a_streamOut.Write( arrBuffer.GetBuffer(), uFrameSize );
        a_streamOut.Write( &u32Zero, ( 0 - a_streamOut.Tell() ) % 4 );
    }
    a_streamOut.Seek( 20 );
    a_streamOut << g_32( a_streamOut.GetSize() - 96 );
    return MITrue;
}

MIBool FsbToSndDlg( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    if ( a_streamIn.ReadString( 4 ) != "FSB5" || a_streamIn.ReadU32() != 1 || a_streamIn.ReadU32() != 1 || a_streamIn.ReadU32() != 36 || a_streamIn.ReadU32() != 0 )
    {
        printf( "Error: Unsupported .fsb file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3SndPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elexsnd";
    mCString const strR3DlgPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elexdlg";
    mCFileStream streamDlg( strR3DlgPath, mEFileOpenMode_Read );
    if ( streamDlg.IsOpen() )
    {
        streamDlg.Open( strR3DlgPath, mEFileOpenMode_ReadWrite );
        streamDlg.Seek( 4 );
        MIUInt const uResourceInfoOffset = streamDlg.ReadU32();
        streamDlg.Seek( uResourceInfoOffset - 36 );
        MIUInt const uData1Offset = streamDlg.ReadU32();
        MIUInt const uOldFsbSize = streamDlg.ReadU32();
        MIUInt const uNewFsbSize =  a_streamIn.GetSize();
        MIUInt const uNewResourceInfoOffset = uResourceInfoOffset - uOldFsbSize + uNewFsbSize;
        mCMemoryStream streamOffsetTable;
        streamOffsetTable << uData1Offset << g_32( uNewFsbSize ) << streamDlg.ReadU8();
        for ( MIUInt u = 0; u != 3; ++u )
        {
            streamOffsetTable << g_32( streamDlg.ReadU32() - uOldFsbSize + uNewFsbSize );
            streamOffsetTable << streamDlg.ReadU32();
            streamOffsetTable << streamDlg.ReadU8();
        }
        streamDlg.Seek( uData1Offset + uOldFsbSize );
        MIUInt uRemainingSize = streamDlg.GetSize() - streamDlg.Tell();
        mCByteArray arrRemainder( 0, uRemainingSize );
        streamDlg.Read( arrRemainder.AccessBuffer(), uRemainingSize );
        streamDlg.Seek( uData1Offset );
        streamDlg << a_streamIn;
        streamDlg.Write( arrRemainder.GetBuffer(), uRemainingSize );
        streamDlg.Seek( 4 );
        streamDlg << g_32( uNewResourceInfoOffset );
        streamDlg.Seek( uNewResourceInfoOffset - 36 );
        streamDlg << streamOffsetTable;
        streamDlg.Skip( 44 );
        streamDlg.Skip( streamDlg.ReadU32() );
        streamDlg << streamOffsetTable;
        return MITrue;
    }
    // This following classification is incorrect for only 38 of the 3510 original sound files
    MIBool bIs2D = ! mCString( strResourceName ).ToLower().Contains( ".3d" ) &&
                   ( mCString( strResourceName ).ToLower().Contains( ".2d" ) ||
                     mCString( strResourceName ).ToLower().Contains( "_2d" ) ||
                     0 == strResourceName.FirstOf( "sfx_ui_" ) ||
                     0 == strResourceName.FirstOf( "mus_" ) );
    strResourceName.Replace( ".2d", "" ), strResourceName.Replace( ".2D", "" ), strResourceName.Replace( ".3d", "" ), strResourceName.Replace( ".3D", "" );
    mCFileStream streamDest( strR3SndPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3SndPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    a_streamIn.Seek( 60 );
    MIUInt uFrequencyId = ( a_streamIn.ReadU32() & 0x1E ) >> 1;
    MIUInt arrSamplingRates[] = { 0, 8000, 11000, 11025, 16000, 22050, 24000, 32000, 44100, 48000 };
    MIUInt uSamplingRate = uFrequencyId > 9 ? 0 : arrSamplingRates[ uFrequencyId ];
    MIU64  u64SampleCount = a_streamIn.ReadU32() >> 2;
    MIUInt uDuration = static_cast< MIUInt >( ( 1000 * u64SampleCount ) / uSamplingRate );
    streamDest << "E1RF" << ( MIU32 )( a_streamIn.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << a_streamIn;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( a_streamIn.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class eCSoundResource2" ) << "SN11";
    uOffset = streamDest.Tell();
    streamDest << ( MIU32 ) 0;
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    uOffset2 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << "GEC2" << mCRisenName( "class eCSoundResource2" ) << ( MIU16 ) 1;
    uOffset3 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << ( MIU16 ) 2;
    streamDest << mCRisenName( "unsigned int" ) << mCRisenName( "Duration" ) << ( MIU32 ) 4 << g_32( uDuration );
    streamDest << mCRisenName( "unsigned char" ) << mCRisenName( "Flags" ) << ( MIU32 ) 1 << ( MIU8 )( bIs2D ? 1 : 0 );
    streamDest << ( MIU32 ) 0;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    streamDest.Seek( uOffset3 );
    streamDest << g_32( uOffset4 - uOffset3 - 4 );
    return MITrue;
}

MIBool FsbToMp3( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut )
{
    MIUInt uEndDataOffset = 96;
    if ( a_streamIn.ReadString( 4 ) != "FSB5" || a_streamIn.ReadU32() != 1 || a_streamIn.ReadU32() != 1 || a_streamIn.ReadU32() != 36 || a_streamIn.ReadU32() != 0 || ( uEndDataOffset += a_streamIn.ReadU32(), a_streamIn.ReadU32() != 11 ) )
        return MIFalse;  // Not a .fsb file created with elexresman
    a_streamIn.Seek( 96 );
    a_streamIn.SetInvertEndianness( MITrue );
    mCByteArray arrBuffer;
    while ( a_streamIn.Tell() < uEndDataOffset )
    {
        SFrame Frame;
        if ( !ReadMp3FrameHeader( a_streamIn, Frame ) )
            return MIFalse;
        a_streamIn.Skip( -4 );
        arrBuffer.Resize( Frame.m_uSize );
        a_streamIn.Read( arrBuffer.AccessBuffer(), Frame.m_uSize );
        a_streamOut.Write( arrBuffer.GetBuffer(), Frame.m_uSize );
        a_streamIn.Skip( ( 0 - a_streamIn.Tell() ) % 4 );
    }
    return MITrue;
}

MIBool FsbToWav( mCMemoryStream & streamFsb, mCString const & a_strFilePath )
{
    // Credits: id-daemon at zenhax.com
    MIBool const b64Bit = sizeof( MILPVoid ) == sizeof( MIU64 );
    mCString strDll = b64Bit ? "fmod64.dll" : "fmodL.dll";
    static HINSTANCE s_hinstLib = LoadLibraryA( strDll.GetText() );
    if ( s_hinstLib == NULL && b64Bit )
    {
        mCString strElexSystemDir;
        DWORD dwSize = mCString::GetStaticBufferSize();
        if ( ERROR_SUCCESS == RegGetValueA( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 411300", "InstallLocation", RRF_RT_REG_SZ, NULL, reinterpret_cast< LPBYTE >( mCString::AccessStaticBuffer() ), &dwSize ) )
            strElexSystemDir = mCString( mCString::AccessStaticBuffer() ).TrimRight( "/\\" ) + "\\system";
        dwSize = mCString::GetStaticBufferSize();
        if ( ERROR_SUCCESS == RegGetValueA( HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\GOG.com\\Games\\1885888793", "EXE", RRF_RT_REG_SZ, NULL, reinterpret_cast< LPBYTE >( mCString::AccessStaticBuffer() ), &dwSize ) )
            strElexSystemDir = g_GetDirectoryPath( mCString::AccessStaticBuffer() );
        if ( strElexSystemDir != "" )
            s_hinstLib = LoadLibraryA( ( strElexSystemDir + "\\" + strDll ).GetText() );
    }
    if ( s_hinstLib == NULL )
    {
        printf( ( "Error: " + strDll + " missing or incompatible.\n" ).GetText() );
        if ( b64Bit )
            printf( "Please copy it from ELEX\\system to this application's directory.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    MII32 ( MI_STDCALL * pFMOD_System_Create )( MILPVoid * );
    *( FARPROC * ) ( &pFMOD_System_Create ) = GetProcAddress( s_hinstLib, "FMOD_System_Create" );
    MII32 ( MI_STDCALL * pFMOD_System_Init )( MILPVoid, MII32, MIU32, MILPVoid );
    *( FARPROC * ) ( &pFMOD_System_Init ) = GetProcAddress( s_hinstLib, "FMOD_System_Init" );
    MII32 ( MI_STDCALL * pFMOD_System_CreateSound )( MILPVoid, MILPCChar, MIU32, MILPVoid, MILPVoid * );
    *( FARPROC * ) ( &pFMOD_System_CreateSound ) = GetProcAddress( s_hinstLib, "FMOD_System_CreateSound" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetNumSubSounds )( MILPVoid, MII32 * );
    *( FARPROC * ) ( &pFMOD_Sound_GetNumSubSounds ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetNumSubSounds" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetSubSound )( MILPVoid, MII32, MILPVoid * );
    *( FARPROC * ) ( &pFMOD_Sound_GetSubSound ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetSubSound" );
    MII32 ( MI_STDCALL * pFMOD_Sound_SeekData )( MILPVoid, MIU32 );
    *( FARPROC * ) ( &pFMOD_Sound_SeekData ) = GetProcAddress( s_hinstLib, "FMOD_Sound_SeekData" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetDefaults )( MILPVoid, MIFloat *, MII32 * );
    *( FARPROC * ) ( &pFMOD_Sound_GetDefaults ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetDefaults" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetFormat )( MILPVoid, MILPVoid, MILPVoid, MII32 *, MII32 * );
    *( FARPROC * ) ( &pFMOD_Sound_GetFormat ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetFormat" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetLength )( MILPVoid, MIU32 *, MIU32 );
    *( FARPROC * ) ( &pFMOD_Sound_GetLength ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetLength" );
    MII32 ( MI_STDCALL * pFMOD_Sound_GetName )( MILPVoid, MILPChar, MII32 );
    *( FARPROC * ) ( &pFMOD_Sound_GetName ) = GetProcAddress( s_hinstLib, "FMOD_Sound_GetName" );
    MII32 ( MI_STDCALL * pFMOD_Sound_ReadData )( MILPVoid, MILPVoid, MIU32, MIU32 * );
    *( FARPROC * ) ( &pFMOD_Sound_ReadData ) = GetProcAddress( s_hinstLib, "FMOD_Sound_ReadData" );
    MII32 ( MI_STDCALL * pFMOD_Sound_Release )( MILPVoid );
    *( FARPROC * ) ( &pFMOD_Sound_Release ) = GetProcAddress( s_hinstLib, "FMOD_Sound_Release" );
    if ( !pFMOD_System_Create ||
         !pFMOD_System_Init ||
         !pFMOD_System_CreateSound ||
         !pFMOD_Sound_GetNumSubSounds ||
         !pFMOD_Sound_GetSubSound ||
         !pFMOD_Sound_SeekData ||
         !pFMOD_Sound_GetDefaults ||
         !pFMOD_Sound_GetFormat ||
         !pFMOD_Sound_GetLength ||
         !pFMOD_Sound_GetName ||
         !pFMOD_Sound_ReadData || 
         !pFMOD_Sound_Release )
    {
        printf( "Error: Unsupported %s version.", strDll.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    static MILPVoid pSystem = 0;
    if ( pSystem == 0 &&
         ( pFMOD_System_Create( &pSystem ) != 0 ||
           pFMOD_System_Init( pSystem, 32, 0, 0 ) != 0 ) )
    {
        printf( "Error: FMOD system initialization failed." );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    MILPVoid pSound = 0;
    MIU32 arrFMOD_CREATESOUNDEXINFO[ 2 ] = { 8, streamFsb.GetSize() };
    if ( pFMOD_System_CreateSound( pSystem, static_cast< MILPCChar >( streamFsb.GetBuffer() ), 0x4880, &arrFMOD_CREATESOUNDEXINFO, &pSound ) != 0 )
    {
        printf( "Error: Unsupported sound file." );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    MII32 i32Result = 0;
    MII32 i32NumSubSounds = 0;
    i32Result |= pFMOD_Sound_GetNumSubSounds( pSound, &i32NumSubSounds );
    for ( MII32 i = 0; i < i32NumSubSounds; ++i )
    {
        MILPVoid pSubSound = 0;
        i32Result |= pFMOD_Sound_GetSubSound( pSound, i, &pSubSound );
        i32Result |= pFMOD_Sound_SeekData( pSubSound, 0 );
        MIFloat fFrequency = 0;
        i32Result |= pFMOD_Sound_GetDefaults( pSubSound, &fFrequency, 0 );
        MII32 i32NumChannels = 0;
        MII32 iNumBitsPerSample = 0;
        i32Result |= pFMOD_Sound_GetFormat( pSubSound, 0, 0, &i32NumChannels, &iNumBitsPerSample );
        MIU32 u32Length = 0;
        i32Result |= pFMOD_Sound_GetLength( pSubSound, &u32Length, 2 );
        i32Result |= pFMOD_Sound_GetName( pSubSound, mCString::AccessStaticBuffer(), static_cast< MII32 >( mCString::GetStaticBufferSize() ) );
        mCString strSoundName( i32Result ? "" : mCString::AccessStaticBuffer() );
        MIU32 u32NumBytesPerSample = ( iNumBitsPerSample + 7 ) / 8;
        MIU32 u32FrameSize = i32NumChannels * u32NumBytesPerSample;
        MIU32 u32DataSize = u32Length * u32FrameSize;
        MIU32 u32ReadSize = 0;
        mCByteArray arrBuffer( 0, u32DataSize );
        i32Result |= pFMOD_Sound_ReadData( pSubSound, arrBuffer.AccessBuffer(), u32DataSize, &u32ReadSize );
        if ( i32Result )
        {
            printf( "Error: Failed to retrieve sound format and data." );
            WaitForEnterKey( MITrue );
            return MIFalse;
        }
        if ( strSoundName == "" )
            strSoundName = g_GetFileNameNoExt( a_strFilePath ) + ( i32NumSubSounds > 1 ? mCString().Format( "_%u" ) : "" );
        mCString strWavFilePath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strSoundName + ".wav";
        mCFileStream streamWav( strWavFilePath, mEFileOpenMode_Write );
        if ( !streamWav.IsOpen() )
        {
            printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strWavFilePath.GetText() );
            WaitForEnterKey( MITrue );
            return MIFalse;
        }
        MIU32 u32Frequency = static_cast< MIU32 >( fFrequency );
        streamWav << "RIFF" << u32DataSize + 36 << "WAVEfmt " << ( MIU32 ) 16 << ( MIU16 ) 1;
        streamWav << static_cast< MIU16 >( i32NumChannels ) << u32Frequency << u32Frequency * u32FrameSize << static_cast< MIU16 >( u32FrameSize ) << static_cast< MIU16 >( iNumBitsPerSample );
        streamWav << "data" << u32DataSize;
        streamWav.Write( arrBuffer.GetBuffer(), u32ReadSize );
    }
    pFMOD_Sound_Release( pSound );
    return MITrue;
}

MIBool DlgDocToDlg( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3DlgPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elexdlg";
    mCString const strWavPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".wav";
    mCFileStream streamWav( strWavPath, mEFileOpenMode_Read );
    if ( !streamWav.IsOpen() )
    {
        printf( "Error: Could not open %s\n", strWavPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    if ( streamWav.ReadString( 4 ) != "RIFF" )
    {
        printf( "Error: Invalid .wav file.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCMemoryStream streamDlgData2;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamDlgData2, MITrue );
    if ( !Parser.ParseRisen3DlgData2( MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", Parser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCFileStream streamDest( strR3DlgPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3DlgPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    streamDest << "E1RF" << ( MIU32 )( streamWav.GetSize() + streamDlgData2.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamWav;
    streamDest << streamDlgData2;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamWav.GetSize() ) << ( MIU8 ) 0;
    streamOffsetTable << g_32( 8 + 36 + streamWav.GetSize() ) << g_32( streamDlgData2.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 2; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class eCDialogueResource2" ) << "DI14";
    uOffset = streamDest.Tell();
    streamDest << ( MIU32 ) 0;
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    uOffset2 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << "GEC2" << mCRisenName( "class eCDialogueResource2" ) << ( MIU16 ) 1;
    uOffset3 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << ( MIU16 ) 0;
    streamDest << ( MIU32 ) 0;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    streamDest.Seek( uOffset3 );
    streamDest << g_32( uOffset4 - uOffset3 - 4 );
    return MITrue;
}

namespace
{
    MIInt CompareReplace( MILPCVoid a_pReplace1, MILPCVoid a_pReplace2 )
    {
        return *static_cast< MIInt const * >( a_pReplace1 ) - *static_cast< MIInt const * >( a_pReplace2 );
    }
}

MIBool ProcessSecMod( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCError::CProbe Probe;
    mCRisenName::InitializeGameStrings();
    mCStringStream streamIn;
    streamIn << a_streamIn;
    streamIn.Seek( a_streamIn.Tell() );
    mCMemoryStream streamTemp;
    mCDocParser Parser( streamIn, streamTemp );
    mTArray< mCString > arrTemplateEntities;
    mTArray< mTArray< mCString > > arrPropertySetNames;
    while ( !streamIn.IsAtEnd() )
    {
        if ( Parser.MatchImmediate( "\"", MIFalse ) )
        {
            streamIn.Skip( -1 );
            if ( mEResult_False == streamIn.ReadStringInQuotes( arrTemplateEntities.AddNew() ) )
                return printf( "Error: Missing enclosing quotes at line %u.\n", streamIn.GetLineNumber() ), WaitForEnterKey( MITrue ), MIFalse;
            arrPropertySetNames.AddNew();
        }
        else if ( Parser.MatchImmediate( "{", MIFalse ) )
        {
            arrTemplateEntities.Add( "{" + streamIn.ReadString( 36 ) + "}" );
            if ( !Parser.MatchImmediate( "}", MIFalse ) )
                return printf( "Error: Invalid GUID format at line %u.\n", streamIn.GetLineNumber() ), WaitForEnterKey( MITrue ), MIFalse;
            arrPropertySetNames.AddNew();
        }
        else
        {
            if ( 0 == arrTemplateEntities.GetCount() )
                return printf( "Error: Sector modification files must start with template entity GUID or name (in quotes).\n" ), WaitForEnterKey( MITrue ), MIFalse;
            mCString strPropertySet = streamIn.ReadString();
            if ( !strPropertySet.EndsWith( "_PS" ) || 0 == mCRisenName( "class " + strPropertySet ).GetRisenID() )
                return printf( "Error: Unknown property set : \"%s\".\n", strPropertySet.GetText() ), WaitForEnterKey( MITrue ), MIFalse;
            arrPropertySetNames.Back().Add( strPropertySet );
        }
    }
    mTStringMap< MIBool > mapUniquenessCheck;
    for ( MIUInt u = arrTemplateEntities.GetCount(); u--; mapUniquenessCheck[ arrTemplateEntities[ u ] ] = MITrue )
        if ( mapUniquenessCheck[ arrTemplateEntities[ u ] ] )
            return printf( "Error: Template entity listed multiple times: %s.\n", arrTemplateEntities[ u ].GetText() ), WaitForEnterKey( MITrue ), MIFalse;
    mTArray< mCString > arrFiles;
    mTArray< mCGenomeVolume::SFileTime > arrFileTimes;
    AddFiles( g_GetDirectoryPath( a_strFilePath ), arrFiles, arrFileTimes );
    mTArray< mCString > arrTemplateDocs;
    mTArray< mCString > arrSectorDocs;
    for ( MIUInt u = arrFiles.GetCount(); u--; )
    {
        mCString strExt = g_GetFileExt( arrFiles[ u ] ).ToLower();
        if ( strExt == "elextpldoc" )
            arrTemplateDocs.Add( arrFiles[ u ] );
        else if ( strExt == "elexsecdoc" )
            arrSectorDocs.Add( arrFiles[ u ] );
    }
    mTArray< mCString > arrPendingTemplateEntities( arrTemplateEntities );
    mTArray< mTArray< mCString > > arrPropertySets( arrPropertySetNames );
    for ( MIUInt u = arrTemplateDocs.GetCount(); u--; )
    {
        mCStringStream streamTplDoc;
        if ( mEResult_False == streamTplDoc.FromFile( arrTemplateDocs[ u ] ) )
        {
            printf( "Warning: Could not open %s\n", arrTemplateDocs[ u ].GetText() );
            continue;
        }
        for ( MIUInt v = arrPendingTemplateEntities.GetCount(); v--; )
        {
            if ( arrPendingTemplateEntities[ v ] == "" )
                continue;
            streamTplDoc.Seek( 0 );
            mCString strMarker = "\"" + arrPendingTemplateEntities[ v ] + "\" {";
            if ( arrPendingTemplateEntities[ v ].StartsWith( "{" ) )
                strMarker = "GUID = " + arrPendingTemplateEntities[ v ];
            if ( !streamTplDoc.SkipTo( strMarker.GetText(), strMarker.GetLength() ) )
                continue;
            streamTplDoc.Skip( strMarker.GetLength() );
            MIUInt uMarkerOffset = streamTplDoc.Tell();
            MIUInt uEndOffset = streamTplDoc.GetSize();
            if ( streamTplDoc.SkipTo( "\" {", mCString( "\" {" ).GetLength() ) )
                uEndOffset = streamTplDoc.Tell();
            for ( MIUInt w = arrPropertySetNames[ v ].GetCount(); w--; )
            {
                streamTplDoc.Seek( uMarkerOffset );
                if ( !streamTplDoc.SkipTo( ( arrPropertySetNames[ v ][ w ] + " {" ).GetText(), ( arrPropertySetNames[ v ][ w ] + " {" ).GetLength() ) ||
                     streamTplDoc.Tell() >= uEndOffset )
                    return printf( "Error: Could not find property set %s of %s in %s.\n", arrPropertySetNames[ v ][ w ].GetText(), arrPendingTemplateEntities[ v ].GetText(), g_GetFileName( arrTemplateDocs[ u ] ).GetText() ), WaitForEnterKey( MITrue ), MIFalse;
                MIUInt uOffsetPSStart = streamTplDoc.Tell();
                if ( !streamTplDoc.SkipTo( "ClassData {", mCString( "ClassData {" ).GetLength() ) )
                    return printf( "Error: Unsupported template document formatting in %s.\n", g_GetFileName( arrTemplateDocs[ u ] ).GetText() ), WaitForEnterKey( MITrue ), MIFalse;
                MIUInt uOffsetPSEnd = streamTplDoc.Tell();
                streamTplDoc.Seek( uOffsetPSStart );
                arrPropertySets[ v ][ w ] = streamTplDoc.ReadString( uOffsetPSEnd - uOffsetPSStart );
            }
            arrPendingTemplateEntities[ v ] = "";
        }
    }
    for ( MIUInt u = arrPendingTemplateEntities.GetCount(); u--; )
        if ( arrPendingTemplateEntities[ u ] != "" )
            return printf( "Error: Could not find .tpldoc file for %s.\n", arrPendingTemplateEntities[ u ].GetText() ), WaitForEnterKey( MITrue ), MIFalse;
    for ( MIUInt u = 0, ue = arrSectorDocs.GetCount(); u != ue; ++u )
    {
        mCStringStream streamSecDoc;
        if ( mEResult_False == streamSecDoc.FromFile( arrSectorDocs[ u ] ) )
        {
            printf( "Warning: Could not open %s\n", arrSectorDocs[ u ].GetText() );
            continue;
        }
        printf( "%s...\n", g_GetFileName( arrSectorDocs[ u ] ).GetText() );
        struct SReplace
        {
            MIUInt   m_uOffset;
            MIUInt   m_uSize;
            MIUInt   m_uTemplateEntity;
            MIUInt   m_uPropertySet;
            mCString m_strIndent;
        };
        mTArray< SReplace > arrReplace;
        for ( MIUInt v = 0, ve = arrTemplateEntities.GetCount(); v != ve; ++v )
        {
            streamSecDoc.Seek( 0 );
            mCString strMarker = "\"" + arrTemplateEntities[ v ] + "\" {";
            if ( arrTemplateEntities[ v ].StartsWith( "{" ) )
                strMarker = "Creator = " + arrTemplateEntities[ v ];
            MIUInt uCount = 0;
            for ( ; streamSecDoc.SkipTo( strMarker.GetText(), strMarker.GetLength() ); ++uCount )
            {
                streamSecDoc.Skip( strMarker.GetLength() );
                MIUInt uMarkerOffset = streamSecDoc.Tell();
                MIUInt uEndOffset = streamSecDoc.GetSize();
                if ( streamSecDoc.SkipTo( "\" {", mCString( "\" {" ).GetLength() ) )
                    uEndOffset = streamSecDoc.Tell();
                for ( MIUInt w = arrPropertySetNames[ v ].GetCount(); w--; )
                {
                    streamSecDoc.Seek( uMarkerOffset );
                    if ( !streamSecDoc.SkipTo( ( arrPropertySetNames[ v ][ w ] + " {" ).GetText(), ( arrPropertySetNames[ v ][ w ] + " {" ).GetLength() ) ||
                         streamSecDoc.Tell() >= uEndOffset )
                    {
                        printf( "Warning: Could not find property set %s of %s in %s.\n", arrPropertySetNames[ v ][ w ].GetText(), arrTemplateEntities[ v ].GetText(), g_GetFileName( arrSectorDocs[ u ] ).GetText() );
                        continue;
                    }
                    SReplace & Replace = arrReplace.AddNew();
                    Replace.m_uOffset = streamSecDoc.Tell();
                    streamSecDoc.SkipTo( "\n", 1 );
                    for ( MILPCChar pcIt = static_cast< MILPCChar >( streamSecDoc.GetBuffer() ) + streamSecDoc.Tell() + 1; *pcIt++ == ' '; Replace.m_strIndent += ' ' );
                    if ( !streamSecDoc.SkipTo( "ClassData {", mCString( "ClassData {" ).GetLength() ) )
                        return printf( "Error: Unsupported sector document formatting in %s.\n", g_GetFileName( arrSectorDocs[ u ].GetText() ) ), WaitForEnterKey( MITrue ), MIFalse;
                    Replace.m_uSize = streamSecDoc.Tell() - Replace.m_uOffset;
                    Replace.m_uTemplateEntity = v;
                    Replace.m_uPropertySet = w;
                }
                streamSecDoc.Seek( uMarkerOffset );
            }
            printf( "  %ux %s\n", uCount, arrTemplateEntities[ v ].GetText() );
        }
        qsort( arrReplace.AccessBuffer(), arrReplace.GetCount(), sizeof( arrReplace[ 0 ] ), &CompareReplace );
        if ( arrReplace.GetCount() == 0 )
            continue;
        mCStringStream streamSecDocNew;
        streamSecDoc.Seek( 0 );
        for ( MIUInt v = 0, ve = arrReplace.GetCount(); v != ve; ++v )
        {
            streamSecDocNew.Write( static_cast< MILPCChar >( streamSecDoc.GetBuffer() ) + streamSecDoc.Tell(), arrReplace[ v ].m_uOffset - streamSecDoc.Tell() );
            streamSecDoc.Seek( arrReplace[ v ].m_uOffset + arrReplace[ v ].m_uSize );
            mCString strPS = arrPropertySets[ arrReplace[ v ].m_uTemplateEntity ][ arrReplace[ v ].m_uPropertySet ];
            mCString strIndentRaw = "";
            if ( strPS.Contains( "\n " ) )
                for ( MIUInt w = strPS.FirstOf( "\n " ) + 1; strPS[ w++ ] == ' '; strIndentRaw += " " );
            strPS.Replace( ( "\n" + strIndentRaw ).GetText(), ( "\n" + arrReplace[ v ].m_strIndent ).GetText() );
            streamSecDocNew.Write( strPS.GetText(), strPS.GetLength() );
        }
        streamSecDocNew.Write( static_cast< MILPCChar >( streamSecDoc.GetBuffer() ) + streamSecDoc.Tell(), streamSecDoc.GetSize() - streamSecDoc.Tell() );
        if ( mEResult_False == streamSecDocNew.ToFile( arrSectorDocs[ u ] ) )
            printf( "Warning: Could not write %s\n", arrSectorDocs[ u ].GetText() );
        printf( "\n" );
    }
    WaitForEnterKey( MITrue );
    return MITrue;
}

MIBool SecDocToSec( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3SecPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elexsec";
    mCMemoryStream streamSec;
    mCMemoryStream streamResourceClass;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamSec, MITrue );
    mCRisenDocParser ResourceClassParser( streamDoc, streamResourceClass );
    if ( !ResourceClassParser.ParseRisen3Class( MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", ResourceClassParser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    if ( !Parser.ParseRisen3Sector( MITrue, MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", Parser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCFileStream streamDest( strR3SecPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3SecPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    streamDest << "E1RF" << ( MIU32 )( streamSec.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamSec;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamSec.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class gCSectorResource" ) << "SC02";
    uOffset = streamDest.Tell();
    streamDest << ( MIU32 ) 0;
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    uOffset2 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << streamResourceClass;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    return MITrue;
}

MIBool TplDocToTpl( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3TplPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".elextpl";
    mCMemoryStream streamTpl;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamTpl, MITrue );
    if ( !Parser.ParseRisen3Template( strResourceName + ".entity", MITrue, MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", Parser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCFileStream streamDest( strR3TplPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3TplPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    streamDest << "E1RF" << ( MIU32 )( streamTpl.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamTpl;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamTpl.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class gCTemplateResource" ) << "TP03";
    uOffset = streamDest.Tell();
    streamDest << ( MIU32 ) 0;
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    uOffset2 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << "GEC2" << mCRisenName( "class gCTemplateResource" ) << ( MIU16 ) 1;
    uOffset3 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << ( MIU16 ) 0;
    streamDest << ( MIU32 ) 0;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    streamDest.Seek( uOffset3 );
    streamDest << g_32( uOffset4 - uOffset3 - 4 );
    return MITrue;
}

MIBool DdsToImg( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath, MIBool & a_bIsDds )
{
    if ( a_streamIn.ReadString( 4 ) != "DDS " || ( a_streamIn.Seek( 76 ), a_streamIn.ReadU32() ) != 32 )
        return a_streamIn.Seek( 0 ), a_bIsDds = MIFalse;
    a_bIsDds = MITrue;
    mCString const strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3IMGPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".eleximg";
    mCFileStream streamDest( strR3IMGPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3IMGPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString strColorFormat = "eEGfxFormat2_Unknown";
    MIUInt const uDXGICount = 133;
    mCStringArray arrPerDXGI( "eEGfxFormat2_Unknown", uDXGICount );
    arrPerDXGI[ 2 ]  = "eEGfxFormat2_R32G32B32A32_Float";
    arrPerDXGI[ 3 ]  = "eEGfxFormat2_R32G32B32A32_UInt";
    arrPerDXGI[ 4 ]  = "eEGfxFormat2_R32G32B32A32_SInt";
    arrPerDXGI[ 6 ]  = "eEGfxFormat2_R32G32B32_Float";
    arrPerDXGI[ 7 ]  = "eEGfxFormat2_R32G32B32_UInt";
    arrPerDXGI[ 8 ]  = "eEGfxFormat2_R32G32B32_SInt";
    arrPerDXGI[ 10 ] = "eEGfxFormat2_R16G16B16A16_Float";
    arrPerDXGI[ 11 ] = "eEGfxFormat2_R16G16B16A16_UNorm";
    arrPerDXGI[ 12 ] = "eEGfxFormat2_R16G16B16A16_UInt";
    arrPerDXGI[ 13 ] = "eEGfxFormat2_R16G16B16A16_SNorm";
    arrPerDXGI[ 14 ] = "eEGfxFormat2_R16G16B16A16_SInt";
    arrPerDXGI[ 16 ] = "eEGfxFormat2_R32G32_Float";
    arrPerDXGI[ 17 ] = "eEGfxFormat2_R32G32_UInt";
    arrPerDXGI[ 18 ] = "eEGfxFormat2_R32G32_SInt";
    arrPerDXGI[ 24 ] = "eEGfxFormat2_R10G10B10A2_UNorm";
    arrPerDXGI[ 25 ] = "eEGfxFormat2_R10G10B10A2_UInt";
    arrPerDXGI[ 26 ] = "eEGfxFormat2_R11G11B10_Float";
    arrPerDXGI[ 28 ] = "eEGfxFormat2_R8G8B8A8_UNorm";
    arrPerDXGI[ 29 ] = "eEGfxFormat2_R8G8B8A8_UNorm_SRGB";
    arrPerDXGI[ 30 ] = "eEGfxFormat2_R8G8B8A8_UInt";
    arrPerDXGI[ 31 ] = "eEGfxFormat2_R8G8B8A8_SNorm";
    arrPerDXGI[ 32 ] = "eEGfxFormat2_R8G8B8A8_SInt";
    arrPerDXGI[ 34 ] = "eEGfxFormat2_R16G16_Float";
    arrPerDXGI[ 35 ] = "eEGfxFormat2_R16G16_UNorm";
    arrPerDXGI[ 36 ] = "eEGfxFormat2_R16G16_UInt";
    arrPerDXGI[ 37 ] = "eEGfxFormat2_R16G16_SNorm";
    arrPerDXGI[ 38 ] = "eEGfxFormat2_R16G16_SInt";
    arrPerDXGI[ 40 ] = "eEGfxFormat2_D32_Float";
    arrPerDXGI[ 41 ] = "eEGfxFormat2_R32_Float";
    arrPerDXGI[ 42 ] = "eEGfxFormat2_R32_UInt";
    arrPerDXGI[ 43 ] = "eEGfxFormat2_R32_SInt";
    arrPerDXGI[ 45 ] = "eEGfxFormat2_D24_UNorm_S8_UInt";
    arrPerDXGI[ 49 ] = "eEGfxFormat2_R8G8_UNorm";
    arrPerDXGI[ 50 ] = "eEGfxFormat2_R8G8_UInt";
    arrPerDXGI[ 51 ] = "eEGfxFormat2_R8G8_SNorm";
    arrPerDXGI[ 52 ] = "eEGfxFormat2_R8G8_SInt";
    arrPerDXGI[ 54 ] = "eEGfxFormat2_R16_Float";
    arrPerDXGI[ 55 ] = "eEGfxFormat2_D16_UNorm";
    arrPerDXGI[ 56 ] = "eEGfxFormat2_R16_UNorm";
    arrPerDXGI[ 57 ] = "eEGfxFormat2_R16_UInt";
    arrPerDXGI[ 58 ] = "eEGfxFormat2_R16_SNorm";
    arrPerDXGI[ 59 ] = "eEGfxFormat2_R16_SInt";
    arrPerDXGI[ 61 ] = "eEGfxFormat2_R8_UNorm";
    arrPerDXGI[ 62 ] = "eEGfxFormat2_R8_UInt";
    arrPerDXGI[ 63 ] = "eEGfxFormat2_R8_SNorm";
    arrPerDXGI[ 64 ] = "eEGfxFormat2_R8_SInt";
    arrPerDXGI[ 65 ] = "eEGfxFormat2_A8_UNorm";
    arrPerDXGI[ 71 ] = "eEGfxFormat2_BC1_UNorm";
    arrPerDXGI[ 72 ] = "eEGfxFormat2_BC1_UNorm_SRGB";
    arrPerDXGI[ 74 ] = "eEGfxFormat2_BC2_UNorm";
    arrPerDXGI[ 75 ] = "eEGfxFormat2_BC2_UNorm_SRGB";
    arrPerDXGI[ 77 ] = "eEGfxFormat2_BC3_UNorm";
    arrPerDXGI[ 78 ] = "eEGfxFormat2_BC3_UNorm_SRGB";
    arrPerDXGI[ 80 ] = "eEGfxFormat2_BC4_UNorm";
    arrPerDXGI[ 81 ] = "eEGfxFormat2_BC4_SNorm";
    arrPerDXGI[ 83 ] = "eEGfxFormat2_BC5_UNorm";
    arrPerDXGI[ 84 ] = "eEGfxFormat2_BC5_SNorm";
    arrPerDXGI[ 87 ] = "eEGfxFormat2_B8G8R8A8_UNorm";
    arrPerDXGI[ 88 ] = "eEGfxFormat2_B8G8R8X8_UNorm";
    arrPerDXGI[ 91 ] = "eEGfxFormat2_B8G8R8A8_UNorm_SRGB";
    arrPerDXGI[ 93 ] = "eEGfxFormat2_B8G8R8X8_UNorm_SRGB";
    arrPerDXGI[ 95 ] = "eEGfxFormat2_BC6H_UF16";
    arrPerDXGI[ 96 ] = "eEGfxFormat2_BC6H_SF16";
    arrPerDXGI[ 98 ] = "eEGfxFormat2_BC7_UNorm";
    arrPerDXGI[ 99 ] = "eEGfxFormat2_BC7_UNorm_SRGB";
    MIU32 u32Flags, u32FourCC, u32RGBBitCount, u32RBitMask, u32GBitMask, u32BBitMask, u32ABitMask;
    a_streamIn >> u32Flags >> u32FourCC >> u32RGBBitCount >> u32RBitMask >> u32GBitMask >> u32BBitMask >> u32ABitMask;
    if ( u32FourCC == 808540228 /*DX10*/ )
    {
        a_streamIn.Skip( 20 );
        MIU32 u32DXGIFormat = a_streamIn.ReadU32();
        if ( u32DXGIFormat < uDXGICount )
            strColorFormat = arrPerDXGI[ u32DXGIFormat ];
    }
    else if ( u32Flags & 0x4 /*DDPF_FOURCC*/ )
    {
        switch ( u32FourCC )
        {
            case 113        : strColorFormat = "eEGfxFormat2_R16G16B16A16_Float"; break;
            case 116        : strColorFormat = "eEGfxFormat2_R32G32B32A32_Float"; break;
            case 112        : strColorFormat = "eEGfxFormat2_R16G16_Float";       break;
            case 115        : strColorFormat = "eEGfxFormat2_R32G32_Float";       break;
            case 111        : strColorFormat = "eEGfxFormat2_R16_Float";          break;
            case 114        : strColorFormat = "eEGfxFormat2_R32_Float";          break;
            case 827611204  : strColorFormat = "eEGfxFormat2_BC1_UNorm";          break;
            case 844388420  : strColorFormat = "eEGfxFormat2_BC2_UNorm";          break;
            case 861165636  : strColorFormat = "eEGfxFormat2_BC2_UNorm";          break;
            case 877942852  : strColorFormat = "eEGfxFormat2_BC3_UNorm";          break;
            case 894720068  : strColorFormat = "eEGfxFormat2_BC3_UNorm";          break;
            case 826889281  : strColorFormat = "eEGfxFormat2_BC4_UNorm";          break;
            case 1429488450 : strColorFormat = "eEGfxFormat2_BC4_UNorm";          break;
            case 1395934018 : strColorFormat = "eEGfxFormat2_BC4_SNorm";          break;
            case 843666497  : strColorFormat = "eEGfxFormat2_BC5_UNorm";          break;
            case 1429553986 : strColorFormat = "eEGfxFormat2_BC5_UNorm";          break;
            case 1395999554 : strColorFormat = "eEGfxFormat2_BC5_SNorm";          break;
        }
    }
    else if ( u32Flags & 0x40 /*DDPF_RGB*/ || u32Flags & 0x20000 /*DDPF_LUMINANCE*/ )
    {
        if ( u32Flags & 0x1 /*DDPF_ALPHAPIXELS*/ )
        {
            if ( u32RGBBitCount == 32 && u32RBitMask == 0xFF0000 && u32GBitMask == 0xFF00 && u32BBitMask == 0xFF && u32ABitMask == 0xFF000000 )
                strColorFormat = "eEGfxFormat2_B8G8R8A8_UNorm";
            else if ( u32RGBBitCount == 32 && u32RBitMask == 0xFFFF && u32ABitMask == 0xFFFF0000 )
                strColorFormat = "eEGfxFormat2_R16G16_UNorm";
            else if ( u32RGBBitCount == 32 && u32RBitMask == 0xFF && u32GBitMask == 0xFF00 && u32BBitMask == 0xFF0000 && u32ABitMask == 0xFF000000 )
                strColorFormat = "eEGfxFormat2_R8G8B8A8_UNorm";
            else if ( u32RGBBitCount == 32 && u32RBitMask == 0x3FF && u32GBitMask == 0xFFC00 && u32BBitMask == 0x3FF00000 && u32ABitMask == 0xC0000000 )
                strColorFormat = "eEGfxFormat2_R10G10B10A2_UNorm";
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0xFF && u32ABitMask == 0xFF00 )
                strColorFormat = "eEGfxFormat2_R8G8_UNorm";
        }
        else
        {
            if ( u32RGBBitCount == 32 && u32RBitMask == 0xFF0000 && u32GBitMask == 0xFF00 && u32BBitMask == 0xFF )
                strColorFormat = "eEGfxFormat2_B8G8R8X8_UNorm";
            else if ( u32RGBBitCount == 32 && u32RBitMask == 0xFFFF && u32GBitMask == 0xFFFF0000 )
                strColorFormat = "eEGfxFormat2_R16G16_UNorm";
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0xFF && u32GBitMask == 0xFF00 )
                strColorFormat = "eEGfxFormat2_R8G8_UNorm";
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0xFFFF )
                strColorFormat = "eEGfxFormat2_R16_UNorm";
            else if ( u32RGBBitCount == 8 && u32RBitMask == 0xFF )
                strColorFormat = "eEGfxFormat2_R8_UNorm";
        }
    }
    else if ( u32Flags & 0x1 /*DDPF_ALPHAPIXELS*/ && u32ABitMask == 0xFF )
        strColorFormat = "eEGfxFormat2_A8_UNorm";
    mCRisenName nameColorFormat( strColorFormat );
    MIInt uEnumColorFormat = 0;
    if ( !nameColorFormat.GetEnumValue( uEnumColorFormat ) )
        nameColorFormat = mCRisenName( "eEGfxFormat2_Unknown" );
    MIUInt uMipMapCount = 0, uHeight, uWidth;
    a_streamIn.Seek( 8 );
    if ( a_streamIn.ReadU32() & 0x20000 )
        uMipMapCount = 1;
    a_streamIn >> g_32( uHeight ) >> g_32( uWidth );
    a_streamIn.Skip( 8 );
    if ( uMipMapCount )
        a_streamIn >> g_32( uMipMapCount );
    MIU32 u32SkipMips = ( uMipMapCount && uWidth >= 128 ) ? ( uWidth == 128 ? 257 : 258 ) : 0;
    streamDest << "E1RF" << ( MIU32 )( a_streamIn.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << a_streamIn;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( a_streamIn.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeGameStrings();
    streamDest << mCRisenName( "class eCImageResource2" ) << "IM08";
    uOffset = streamDest.Tell();
    streamDest << ( MIU32 ) 0;
    mCGenomeVolume::SFileTime Time;
    GetFileTime( a_strFilePath, Time );
    streamDest << SwapHighLow( Time.m_u64Modified ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( Time.m_u64Created ) << SwapHighLow( g_time() );
    streamDest << g_32( strResourceName.GetLength() ) << strResourceName;
    streamDest << streamOffsetTable;
    streamDest << ( MIU32 ) 0 << ( MIU32 ) 0;
    uOffset2 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << "GEC2" << mCRisenName( "class eCImageResource2" ) << ( MIU16 ) 1;
    uOffset3 = streamDest.Tell();
    streamDest << ( MIU32 ) 0 << ( MIU16 ) 4;
    streamDest << mCRisenName( "int" ) << mCRisenName( "Width" ) << ( MIU32 ) 4 << g_32( uWidth );
    streamDest << mCRisenName( "int" ) << mCRisenName( "Height" ) << ( MIU32 ) 4 << g_32( uHeight );
    streamDest << mCRisenName( "int" ) << mCRisenName( "Depth" ) << ( MIU32 ) 4 << ( MIU32 ) 0;
    streamDest << mCRisenName( "enum eEGfxFormat2" ) << mCRisenName( "PixelFormat" ) << ( MIU32 ) 8 << nameColorFormat.GetID() << g_32( uEnumColorFormat );
    streamDest << ( MIU32 ) 1 << mCRisenName( "class eCImageResource2" ) << ( MIU16 ) 1 << ( MIU32 ) 36 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    streamDest.Seek( uOffset3 );
    streamDest << g_32( uOffset4 - uOffset3 - 4 );
    return MITrue;
}

struct SEnumData
{
    mCString strName, strNumber, strHash, strType;
};

MIInt uncompressHeaderlessZlib( MILPByte a_pDest, MIU32 * a_pDestLen, MILPCByte a_pSource, MIU32 a_u32SourceLen )
{
    z_stream stream;
    MIInt err;
    stream.next_in = const_cast< MILPByte >( a_pSource );
    stream.avail_in = static_cast< MIUInt >( a_u32SourceLen );
    if ( static_cast< MIU32 >( stream.avail_in ) != a_u32SourceLen )
        return Z_BUF_ERROR;
    stream.next_out = a_pDest;
    stream.avail_out = static_cast< MIUInt >( *a_pDestLen );
    if ( static_cast< MIU32 >( stream.avail_out ) != *a_pDestLen )
        return Z_BUF_ERROR;
    stream.zalloc = 0;
    stream.zfree = 0;
    err = inflateInit2( &stream, -15 );
    if ( err != Z_OK )
        return err;
    err = inflate( &stream, Z_FINISH );
    if ( err != Z_STREAM_END ) {
        inflateEnd( &stream );
        if ( err == Z_NEED_DICT || ( err == Z_BUF_ERROR && stream.avail_in == 0 ) )
            return Z_DATA_ERROR;
        return err;
    }
    *a_pDestLen = stream.total_out;
    err = inflateEnd( &stream );
    return err;
}

MIInt compressHeaderlessZlib( MILPByte a_pDest, MIU32 * a_pDestLen, MILPCByte a_pSource, MIU32 a_u32SourceLen)
{
    MIInt level = Z_DEFAULT_COMPRESSION;
    z_stream stream;
    MIInt err;
    stream.next_in = const_cast< MILPByte >( a_pSource );
    stream.avail_in = static_cast< MIUInt >( a_u32SourceLen );
    stream.next_out = a_pDest;
    stream.avail_out = static_cast< MIUInt >( *a_pDestLen );
    if ( static_cast< MIU32 >( stream.avail_out ) != *a_pDestLen )
        return Z_BUF_ERROR;
    stream.zalloc = 0;
    stream.zfree = 0;
    stream.opaque = 0;
    err = deflateInit2( &stream, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY );
    if ( err != Z_OK )
        return err;
    err = deflate( &stream, Z_FINISH );
    if ( err != Z_STREAM_END )
    {
        deflateEnd( &stream );
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *a_pDestLen = stream.total_out;
    err = deflateEnd( &stream );
    return err;
}

mCString FormatTable( mTArray< mCStringArray > a_arrEntries, mCString a_strAlign )
{
    MIUInt const uWidth = a_strAlign.GetLength(), uHeight = a_arrEntries.GetCount();
    mCString strTable, strSpace( ' ', 255 );
    mTArray< MIUInt > arrMaxLength( 0, uWidth );
    for ( MIUInt u = 0; u != uHeight; ++u )
        for ( MIUInt v = 0, ve = g_min( a_arrEntries[ u ].GetCount(), uWidth ); v != ve; ++v )
            if ( arrMaxLength[ v ] < a_arrEntries[ u ][ v ].GetLength() )
                arrMaxLength[ v ] = a_arrEntries[ u ][ v ].GetLength();
    for ( MIUInt u = 0; u != uHeight; ++u, strTable.TrimRight( " |" ), strTable += "\n" )
        for ( MIUInt v = 0, ve = g_min( a_arrEntries[ u ].GetCount(), uWidth ); v != ve; ++v )
            strTable += ( a_strAlign[ v ] == 'l' ? ( a_arrEntries[ u ][ v ] + strSpace ).Left ( arrMaxLength[ v ] ) :
                                                   ( strSpace + a_arrEntries[ u ][ v ] ).Right( arrMaxLength[ v ] ) ) + " | ";
    return strTable.TrimRight( "\n" );
}

MIBool ReadResourceFile( mCIOStreamBinary & streamIn, mCString const & a_strFilePath, MIBool & a_bIsResourceFile )
{
    if ( !( a_bIsResourceFile = ( streamIn.ReadString( 4 ) == "E1RF" ) ) )
        return streamIn.Seek( 0 ), MIFalse;
    streamIn.Seek( streamIn.ReadU32() + 4 );
    mCString const strResourceRevision = streamIn.ReadString( 4 );
    if ( strResourceRevision == "IM08" )
        return ImgToDds( streamIn, a_strFilePath );
    if ( strResourceRevision == "SN11" || strResourceRevision == "DI22" )
    {
        mCMemoryStream streamFsb, streamMp3;
        mCString strFilePath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath );
        SndDlgToFsb( streamIn, streamFsb );
        streamFsb.Seek( 0 );
        if ( FsbToMp3( streamFsb, streamMp3 ) )
            strFilePath += ".mp3";
        else if ( s_bFsb )
            strFilePath += ".fsb";
        else
            return FsbToWav( streamFsb, strFilePath );
        if ( !( strFilePath.EndsWith( "mp3" ) ? streamMp3 : streamFsb ).ToFile( strFilePath ) )
        {
            printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strFilePath.GetText() );
            WaitForEnterKey( MITrue );
            return MIFalse;
        }
        return MITrue;
    }
    if ( strResourceRevision == "TP03" )
        return TplToTplDoc( streamIn, a_strFilePath );
    if ( strResourceRevision == "SC02" )
        return SecToSecDoc( streamIn, a_strFilePath );
    if ( strResourceRevision == "WR02" )
        return WrlToWrlDoc( streamIn, a_strFilePath );
    printf( "Error: Unsupported resource type.\n" );
    WaitForEnterKey( MITrue );
    return MIFalse;
}

mCString GetMemorySizeString( MIU64 u64Size )
{
    MILPCChar arrUnits[] = { "B ", "KB", "MB", "GB" };
    MIUInt u = 0, ue = sizeof( arrUnits ) / sizeof( arrUnits[ 0 ] );
    for ( ; u + 1 < ue && u64Size >> ( 10 * ( u + 1 ) ); ++u );
    return mCString().Format( "%.3g ", static_cast< MIDouble >( u64Size ) / ( 1 << ( 10 * u ) ) ) + arrUnits[ u ];
}

MIBool Unpack( mCGenomeVolume & a_Pak, mCString const & a_strPath )
{
    mCGenomeVolume::CDir Root = a_Pak.GetRoot();
    mCString strSubDirsToUnpack;
    if ( Root.GetSubDirs().GetCount() == 0 && Root.GetTotalFileCount() == 0 )
    {
        printf( "The archive is empty.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    else if ( s_bNonInteractive )
    {
        strSubDirsToUnpack = "*";
    }
    else if ( Root.GetSubDirs().GetCount() > 0 )
    {
        printf( "Packed directories:\n\n" );
        mTArray< mCStringArray > arrTableLines;
        for ( MIUInt u = 0; u < Root.GetSubDirs().GetCount(); ++u )
        {
            mCGenomeVolume::CDir const & Dir = Root.GetSubDirs()[ u ];
            mCString const strName = Dir.GetName();
            mCString const strResourceType = Dir.GetRisen3ResourceType();
            if ( Dir.GetRisen3ResourceType() == "" && strName.GetLength() == 8 && strName[ 1 ] == '_' && strName[ 4 ] == '_' && isdigit( strName[ 0 ] ) )
            {
                Root.AccessSubDirs().RemoveAt( u-- );
                continue;
            }
            arrTableLines.AddNew();
            arrTableLines.Back().AddNew().Format( "%u", u + 1 );
            arrTableLines.Back().AddNew() = strName + ( strResourceType == "" ? "" : " (" + strResourceType + ")" );
            arrTableLines.Back().AddNew().Format( "%u files", Dir.GetTotalFileCount() );
            arrTableLines.Back().AddNew() = GetMemorySizeString( Dir.GetTotalSize() );
        }
        printf( FormatTable( arrTableLines, "rlrr" ).GetText() );
        printf( "\n\nPlease specify the numbers of the directories to be extracted, or * for 'all'.\n" );
        for ( MIChar c = 0; c != '\n' && c != EOF; c = getchar(), strSubDirsToUnpack += c );
    }
    MIBool const bAllDirs = strSubDirsToUnpack.Contains( '*' );
    for ( MIUInt u = 0, ue = strSubDirsToUnpack.GetLength(); u != ue; ++u )
        if ( strSubDirsToUnpack[ u ] < '0' || strSubDirsToUnpack[ u ] > '9' )
            strSubDirsToUnpack[ u ] = ' ';
    strSubDirsToUnpack = " " + strSubDirsToUnpack + " ";
    for ( MIUInt uReplaceCount = 1; uReplaceCount; )
        strSubDirsToUnpack.Replace( " 0", "  ", uReplaceCount );
    for ( MIUInt u = Root.GetSubDirs().GetCount(); u--; )
    {
        mCString strResourceType = Root.GetSubDirs()[ u ].GetRisen3ResourceType();
        if ( !bAllDirs && !strSubDirsToUnpack.Contains( mCString().Format( " %u ", u + 1 ).GetText() ) )
            Root.AccessSubDirs().RemoveAt( u );
        else if ( strResourceType != "" )
            Root.AccessSubDirs()[ u ].AccessName() = strResourceType;
    }
    if ( Root.GetSubDirs().GetCount() == 0 && Root.GetTotalFileCount() == 0 )
        return MITrue;
    mCString strDir = g_GetDirectoryPath( a_strPath );
    if ( strDir == "" )
        strDir = ".";
    MIBool bResult = CreateFiles( strDir + "\\" + g_GetFileNameNoExt( a_strPath ), Root );
    if ( bResult )
        return printf( "Unpacking successful.\n" ), MITrue;
    else
        printf( "Error when unpacking %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", a_strPath.GetText() );
    if ( s_bDebug && mCError::GetLastError< mCError >() )
        printf( "%s\n", mCError::GetLastError< mCError >()->GetText() );
    WaitForEnterKey( MITrue );
    return MIFalse;
}

int main( int argc, char* argv[] )
{
    mCRisenName::SetGame( mEGame_Elex );

    /*mCFileStream streamIn2("E:\\Steam\\steamapps\\common\\ELEX\\data\\packed\\c_1_na\\SkinnedMesh\\Mesh_Male_Hair_Short_01_L1.elexskn", mEFileOpenMode_Read);
    mCFileStream streamOut("C:\\Users\\Val\\Desktop\\Mesh_Male_Hair_Short_01_L1.elexskndoc", mEFileOpenMode_Write);
    streamIn2.Seek(523603);
    mCString strIndent;
    mCRisenDoc doc( streamIn2, streamOut, strIndent );
    doc.DocumentRisen3Class();
    return 0;*///$

    if ( argc < 2 )
    {
        printf( "Elex Resource Manager v0.6 by Baltram\n"
                "Start by dragging a file or folder onto elexresman.exe.\n\n"
                "Supported file types and actions:\n\n"
                "  Elex PAK volume : (un)pack .pak     <--> [folder]\n"
                "  Elex document   :  convert .hdr     <--> .hdrdoc\n"
                "  Elex image      :  convert .eleximg <--> .dds\n"
                "  Elex sound      :  convert .elexsnd <--> .mp3 or --> .wav\n"
                "  Elex dialog     :  convert .elexdlg <--> .mp3 or --> .wav\n"
                "  Elex template   :  convert .elextpl <--> .elextpldoc\n"
                "  Elex world      :  convert .elexwrl <--> .elexwrldoc\n"
                "  Elex sector     :  convert .elexsec <--> .elexsecdoc\n"
                "  Sector modifi-\n"
                "  cation file     :  process .elexsecmod\n\n" );
        WaitForEnterKey( MITrue );
        return 1;
    }
    for ( MIInt i = 2; i < argc; ++i )
    {
        if ( mCString( argv[ i ] ) == "--non-interactive" )
            s_bNonInteractive = MITrue;
        else if ( mCString( argv[ i ] ) == "--fsb" )
            s_bFsb = MITrue;
        else if ( mCString( argv[ i ] ) == "--generation" )
        {
            if ( i + 1 >= argc )
            {
                printf( "Error: --generation option used but no generation (0-9) specified.\n" );
                WaitForEnterKey( MITrue );
                return 1;
            }
            if ( mCString( argv[ ++i ] ).Scan( "%u", &s_uGeneration ) != 1 || s_uGeneration > 9 )
            {
                printf( "Error: Invalid generation specified: \"%s\".\n", argv[ i ] );
                WaitForEnterKey( MITrue );
                return 1;
            }
        }
        else
        {
            printf( "Error: Unknown option: \"%s\".\n", argv[ i ] );
            WaitForEnterKey( MITrue );
            return 1;
        }
    }
    mCGenomeVolume::RegisterZlibCompressFunction( &compressHeaderlessZlib );
    mCGenomeVolume::RegisterZlibUncompressFunction( &uncompressHeaderlessZlib );
    GetFullPathNameA( argv[ 1 ], mCString::GetStaticBufferSize(), mCString::AccessStaticBuffer(), NULL );
    mCString strPath( mCString::AccessStaticBuffer() );
    DWORD dwAttrib = GetFileAttributesA( strPath.GetText() );
    if ( dwAttrib != INVALID_FILE_ATTRIBUTES && dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
    {
        if ( CreateRisen3Volume( strPath ) )
            return 0;
        WaitForEnterKey( MITrue );
        return 1;
    }
    mCFileStream streamIn( strPath, mEFileOpenMode_Read );
    if ( g_GetFileExt( strPath ).ToLower() == "elextpldoc" )
        return TplDocToTpl( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "elexsecdoc" )
        return SecDocToSec( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "elexsecmod" )
        return ProcessSecMod( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "hdr" )
        return HdrToHdrDoc( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "hdrdoc" )
        return HdrDocToHdr( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "elexwrldoc" )
        return WrlDocToWrl( streamIn, strPath ) ? 0 : 1;
    MIBool bIsResourceFile = MIFalse, bIsDdsFile = MIFalse, bIsWavFile = MIFalse;
    MIBool bSuccess = ReadResourceFile( streamIn, strPath, bIsResourceFile );
    if ( bIsResourceFile )
        return bSuccess ? 0 : 1;
    if ( g_GetFileExt( strPath ).ToLower() == "mp3" )
    {
        mCMemoryStream streamFsb;
        if ( !Mp3ToFsb( streamIn, streamFsb ) )
            return 1;
        streamFsb.Seek( 0 );
        return FsbToSndDlg( streamFsb, strPath );
    }
    if ( g_GetFileExt( strPath ).ToLower() == "fsb" )
        return FsbToSndDlg( streamIn, strPath );
    bSuccess = DdsToImg( streamIn, strPath, bIsDdsFile );
    if ( bIsDdsFile )
        return bSuccess ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "dds" )
    {
        printf( "Error: Invalid .dds file: %s\n", strPath.GetText() );
        WaitForEnterKey( MITrue );
        return 1;
    }
    streamIn.Close();
    mCGenomeVolume Pak( strPath, MITrue );
    if ( !Pak.IsOpen() )
    {
        printf( "Error: Unsupported file type: %s\n", strPath.GetText() );
        WaitForEnterKey( MITrue );
        return 1;
    }
    return Unpack( Pak, strPath ) ? 0 : 1;
}