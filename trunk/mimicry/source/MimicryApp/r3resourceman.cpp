#include <cstdio>
#include "..\Mimicry.h"
#include "..\Mimicry\mi_include_risen.h"
#include "Windows.h"
#include "..\..\..\zlib\zlib.h"

namespace
{
    MIBool s_bDebug = MIFalse;
    MIBool s_bNonInteractive = MIFalse;
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
    if ( a_uCurrent * 10 / a_uTotal != ( a_uCurrent - 1 ) * 10 / a_uTotal )
        printf( "%u%%\n", a_uCurrent * 10 / a_uTotal * 10 );
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
        s_uTotalFileCount = a_Dir.GetFileCount();
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
        mCFileStream streamOut( a_strRootDirectory + File.GetFilePath(), mEFileOpenMode_Write );
        if ( !streamOut.IsOpen() )
            return MIFalse;
        File.GetFileData( streamOut );
        streamOut.Close();
        ++s_uCreatedFileCount;
        ShowProgress( s_uCreatedFileCount, s_uTotalFileCount );
        HANDLE hFile = CreateFileA( ( a_strRootDirectory + File.GetFilePath() ).GetText(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
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
    for ( ; uResult > 9; )
    {
        printf( "Please enter archive generation (0-9)\n" );
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
    mCString const strPakPath = a_strDirectoryPath + ".pak";
    mCFileStream streamPAK( strPakPath, mEFileOpenMode_Write );
    if ( !streamPAK.IsOpen() )
    {
        printf( "Error: Could not create %s\n", strPakPath.GetText() );
        return MIFalse;
    }
    mCString strVolumeName;
    if ( !mCGenomeVolume::CreateRisen3Archive( a_strDirectoryPath, arrFilePaths, arrFileTimes, streamPAK, &RequestGeneration, &strVolumeName, &ShowProgress ) )
    {
        if ( mCError::GetLastError< mCError >() )
            printf( "Error: %s\n", mCError::GetLastError< mCError >()->GetText() );
        streamPAK.Close();
        DeleteFileA( strPakPath.GetText() );
        return MIFalse;
    }
    streamPAK.Close();
    mCString strPakPathNew = g_GetDirectoryPath( strPakPath ) + "\\" + strVolumeName;
    if ( strPakPathNew == strPakPath || MoveFileA( strPakPath.GetText(), strPakPathNew.GetText() ) )
        return printf( "Successfully created %s.\n", strPakPathNew.GetText() ), MITrue;
    else
        printf( "Successfully created %s but could not rename to %s\n", strPakPath.GetText(), g_GetFileName( strPakPathNew ).GetText() );
    return MIFalse;
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

MIBool SndDlgToWav( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    a_streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size, u32Data2Offset, u32Data2Size;
    a_streamIn >> u32Data1Offset >> u32Data1Size;
    a_streamIn.Skip( 1 );
    a_streamIn >> u32Data2Offset >> u32Data2Size;
    a_streamIn.Seek( u32Data1Offset );
    mCString const strWAVPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".wav";
    mCFileStream streamWAV( strWAVPath, mEFileOpenMode_Write );
    if ( !streamWAV.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strWAVPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCByteArray arrBuffer;
    arrBuffer.Resize( u32Data1Size );
    a_streamIn.Read( arrBuffer.AccessBuffer(), u32Data1Size );
    streamWAV.Write( arrBuffer.GetBuffer(), u32Data1Size );
    if ( !u32Data2Size )
        return MITrue;
    // dlg
    a_streamIn.Seek( u32Data2Offset );
    mCMemoryStream streamDoc;
    mCRisenDoc Doc( a_streamIn, streamDoc );
    if ( !Doc.DocumentRisen3DlgData2() )
    {
        printf( "Error: Unknown dialog file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".r3dlgdoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
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
    if ( !Doc.DocumentRisen3Template() )
    {
        printf( "Error: Unknown template file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".r3tpldoc";
    if ( !streamDoc.ToFile( strDocPath ) )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDocPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    return MITrue;
}

MIBool ReadResourceFile( mCIOStreamBinary & streamIn, mCString const & a_strFilePath, MIBool & a_bIsResourceFile )
{
    if ( !( a_bIsResourceFile = ( streamIn.ReadString( 4 ) == "R3RF" ) ) )
        return streamIn.Seek( 0 ), MIFalse;
    streamIn.Seek( streamIn.ReadU32() + 4 );
    mCString const strResourceRevision = streamIn.ReadString( 4 );
    if ( strResourceRevision == "IM06" )
        return ImgToDds( streamIn, a_strFilePath );
    if ( strResourceRevision == "SN09" || strResourceRevision == "DI14" )
        return SndDlgToWav( streamIn, a_strFilePath );
    if ( strResourceRevision == "TP02" )
        return TplToTplDoc( streamIn, a_strFilePath );
    printf( "Error: Unsupported resource type.\n" );
    WaitForEnterKey( MITrue );
    return MIFalse;
}

int ResolveRisen3Hashes( void )
{
    mCRisenName::InitializeRisen3Strings();
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

MIBool WavToSnd( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath, MIBool & a_bIsWav )
{
    if ( a_streamIn.ReadString( 4 ) != "RIFF" )
        return a_streamIn.Seek( 0 ), a_bIsWav = MIFalse;
    a_bIsWav = MITrue;
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    MIBool bIs2D = ! mCString( strResourceName ).ToLower().Contains( ".3d" ) &&
                   ( mCString( strResourceName ).ToLower().Contains( ".2d" ) ||
                     mCString( strResourceName ).ToLower().Contains( "_2d" ) || 
                     0 == strResourceName.FirstOf( "sfx_ui_" ) ||
                     0 == strResourceName.FirstOf( "mus_" ) ||
                     0 == strResourceName.FirstOf( "cs_" ) ||
                     0 == strResourceName.FirstOf( "csi_" ) );
    strResourceName.Replace( ".2d", "" ), strResourceName.Replace( ".2D", "" ), strResourceName.Replace( ".3d", "" ), strResourceName.Replace( ".3D", "" );
    mCString const strR3SNDPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".r3snd";
    mCFileStream streamDest( strR3SNDPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3SNDPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    MIUInt uBytesPerSecond, uDataChunkSize;
    MIUInt uNextChunk = 12;
    for ( ; ; )
    {
        a_streamIn.Seek( uNextChunk );
        mCString strChunkId = a_streamIn.ReadString( 4 );
        MIUInt uChunkSize = a_streamIn.ReadU32();
        uNextChunk += uChunkSize + 8;
        if ( strChunkId == "fmt " )
        {
            a_streamIn.Skip( 8 );
            a_streamIn >> g_32( uBytesPerSecond );
        }
        else if ( strChunkId == "data" )
        {
            uDataChunkSize = uChunkSize;
            break;
        }
    }
    MIUInt uDuration = static_cast< MIUInt >( ( static_cast< MIU64 >( uDataChunkSize ) * 1000 ) / uBytesPerSecond );
    streamDest << "R3RF" << ( MIU32 )( a_streamIn.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << a_streamIn;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( a_streamIn.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeRisen3Strings();
    streamDest << mCRisenName( "class eCSoundResource2" ) << "SN09";
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
    streamDest << mCRisenName( "unsigned char" ) << mCRisenName( "Flags" ) << ( MIU32 ) 1 << ( MIU8 )( bIs2D ? 3 : 0 );
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

MIBool DlgDocToDlg( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath )
{
    mCString strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3DlgPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".r3dlg";
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
    mCRisenDocParser Parser( streamDoc, streamDlgData2 );
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
    streamDest << "R3RF" << ( MIU32 )( streamWav.GetSize() + streamDlgData2.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamWav;
    streamDest << streamDlgData2;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamWav.GetSize() ) << ( MIU8 ) 0;
    streamOffsetTable << g_32( 8 + 36 + streamWav.GetSize() ) << g_32( streamDlgData2.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 2; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeRisen3Strings();
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

MIBool DdsToImg( mCIOStreamBinary & a_streamIn, mCString const & a_strFilePath, MIBool & a_bIsDds )
{
    if ( a_streamIn.ReadString( 4 ) != "DDS " || ( a_streamIn.Seek( 76 ), a_streamIn.ReadU32() ) != 32 )
        return a_streamIn.Seek( 0 ), a_bIsDds = MIFalse;
    a_bIsDds = MITrue;
    mCString const strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCString const strR3IMGPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".r3img";
    mCFileStream streamDest( strR3IMGPath, mEFileOpenMode_Write );
    if ( !streamDest.IsOpen() )
    {
        printf( "Error: Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strR3IMGPath.GetText() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    enum eEColorFormat
    {
        eEColorFormat_Unknown =               0, eEColorFormat_UnknownHash =       0xA09E2086,
        eEColorFormat_A8R8G8B8 =             21, eEColorFormat_A8R8G8B8Hash =      0xC43B9B72,  // (default)
        eEColorFormat_X8R8G8B8 =             22, eEColorFormat_X8R8G8B8Hash =      0xFE25BBA9,
        eEColorFormat_X1R5G5B5 =             24, eEColorFormat_X1R5G5B5Hash =      0xE317BC19,
        eEColorFormat_A1R5G5B5 =             25, eEColorFormat_A1R5G5B5Hash =      0xA92D9BE2,
        eEColorFormat_A4R4G4B4 =             26, eEColorFormat_A4R4G4B4Hash =      0x900A3562,
        eEColorFormat_A8 =                   28, eEColorFormat_A8Hash =            0x5AFAE68F,
        eEColorFormat_R16F =                111, eEColorFormat_R16FHash =          0x05580235,
        eEColorFormat_G16R16F =             112, eEColorFormat_G16R16FHash =       0xD8D4A203,
        eEColorFormat_A16B16G16R16F =       113, eEColorFormat_A16B16G16R16FHash = 0x0F978AB4,
        eEColorFormat_R32F =                114, eEColorFormat_R32FHash =          0x05580A33,
        eEColorFormat_G32R32F =             115, eEColorFormat_G32R32FHash =       0xDD36993F,
        eEColorFormat_A32B32G32R32F =       116, eEColorFormat_A32B32G32R32FHash = 0x985EF62C,
        eEColorFormat_DXT1 =          827611204, eEColorFormat_DXT1Hash =          0x0550FE97,
        eEColorFormat_DXT2 =          844388420, eEColorFormat_DXT2Hash =          0x0550FE98,
        eEColorFormat_DXT3 =          861165636, eEColorFormat_DXT3Hash =          0x0550FE99,
        eEColorFormat_DXT4 =          877942852, eEColorFormat_DXT4Hash =          0x0550FE9A,
        eEColorFormat_DXT5 =          894720068, eEColorFormat_DXT5Hash =          0x0550FE9B
    };
    eEColorFormat enuColorFormat = eEColorFormat_Unknown, enuColorFormatHash = eEColorFormat_UnknownHash;
    MIU32 u32Flags, u32FourCC, u32RGBBitCount, u32RBitMask, u32GBitMask, u32BBitMask, u32ABitMask;
    a_streamIn >> u32Flags >> u32FourCC >> u32RGBBitCount >> u32RBitMask >> u32GBitMask >> u32BBitMask >> u32ABitMask;
    if ( u32Flags & 0x4 /*DDPF_FOURCC*/ )
    {
        switch ( u32FourCC )
        {
        case eEColorFormat_DXT1:
            enuColorFormat = eEColorFormat_DXT1, enuColorFormatHash = eEColorFormat_DXT1Hash;
            break;
        case eEColorFormat_DXT2:
            enuColorFormat = eEColorFormat_DXT2, enuColorFormatHash = eEColorFormat_DXT2Hash;
            break;
        case eEColorFormat_DXT3:
            enuColorFormat = eEColorFormat_DXT3, enuColorFormatHash = eEColorFormat_DXT3Hash;
            break;
        case eEColorFormat_DXT4:
            enuColorFormat = eEColorFormat_DXT4, enuColorFormatHash = eEColorFormat_DXT4Hash;
            break;
        case eEColorFormat_DXT5:
            enuColorFormat = eEColorFormat_DXT5, enuColorFormatHash = eEColorFormat_DXT5Hash;
            break;
        case eEColorFormat_R16F:
            enuColorFormat = eEColorFormat_R16F, enuColorFormatHash = eEColorFormat_R16FHash;
            break;
        case eEColorFormat_G16R16F:
            enuColorFormat = eEColorFormat_G16R16F, enuColorFormatHash = eEColorFormat_G16R16FHash;
            break;
        case eEColorFormat_A16B16G16R16F:
            enuColorFormat = eEColorFormat_A16B16G16R16F, enuColorFormatHash = eEColorFormat_A16B16G16R16FHash;
            break;
        case eEColorFormat_R32F:
            enuColorFormat = eEColorFormat_R32F, enuColorFormatHash = eEColorFormat_R32FHash;
            break;
        case eEColorFormat_G32R32F:
            enuColorFormat = eEColorFormat_G32R32F, enuColorFormatHash = eEColorFormat_G32R32FHash;
            break;
        case eEColorFormat_A32B32G32R32F:
            enuColorFormat = eEColorFormat_R16F, enuColorFormatHash = eEColorFormat_A32B32G32R32FHash;
            break;
        }
    }
    else if ( u32Flags & 0x40 /*DDPF_RGB*/ )
    {
        if ( u32Flags & 0x1 /*DDPF_ALPHAPIXELS*/ )
        {
            if ( u32RGBBitCount == 32 && u32RBitMask == 0xFF0000 && u32GBitMask == 0xFF00 && u32BBitMask == 0xFF && u32ABitMask == 0xFF000000 )
                enuColorFormat = eEColorFormat_A8R8G8B8, enuColorFormatHash = eEColorFormat_A8R8G8B8Hash;
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0x7C00 && u32GBitMask == 0x3E0 && u32BBitMask == 0x1F && u32BBitMask == 0x8000 )
                enuColorFormat = eEColorFormat_A1R5G5B5, enuColorFormatHash = eEColorFormat_A1R5G5B5Hash;
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0xFF00 && u32GBitMask == 0xFF0 && u32BBitMask == 0xFF && u32ABitMask == 0xFF000 )
                enuColorFormat = eEColorFormat_A4R4G4B4, enuColorFormatHash = eEColorFormat_A4R4G4B4Hash;
        }
        else
        {
            if ( u32RGBBitCount == 32 && u32RBitMask == 0xFF0000 && u32GBitMask == 0xFF00 && u32BBitMask == 0xFF )
                enuColorFormat = eEColorFormat_X8R8G8B8, enuColorFormatHash = eEColorFormat_X8R8G8B8Hash;
            else if ( u32RGBBitCount == 16 && u32RBitMask == 0x7C00 && u32GBitMask == 0x3E0 && u32BBitMask == 0x1F )
                enuColorFormat = eEColorFormat_X1R5G5B5, enuColorFormatHash = eEColorFormat_X1R5G5B5Hash;
        }
    }
    else if ( u32Flags & 0x1 /*DDPF_ALPHAPIXELS*/ && u32ABitMask == 0xFF )
        enuColorFormat = eEColorFormat_A8, enuColorFormatHash = eEColorFormat_A8Hash;
    MIUInt uMipMapCount, uHeight, uWidth;
    a_streamIn.Seek( 8 );
    if ( a_streamIn.ReadU32() & 0x20000 )
        uMipMapCount = 1;
    a_streamIn >> g_32( uHeight ) >> g_32( uWidth );
    a_streamIn.Skip( 8 );
    if ( uMipMapCount )
        a_streamIn >> g_32( uMipMapCount );
    MIU32 u32SkipMips = ( uMipMapCount && uWidth >= 128 ) ? ( uWidth == 128 ? 257 : 258 ) : 0;
    streamDest << "R3RF" << ( MIU32 )( a_streamIn.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << a_streamIn;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( a_streamIn.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeRisen3Strings();
    streamDest << mCRisenName( "class eCImageResource2" ) << "IM06";
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
    streamDest << mCRisenName( "unsigned int" ) << mCRisenName( "SkipMips" ) << ( MIU32 ) 4 << g_32( u32SkipMips );
    streamDest << mCRisenName( "enum eEColorFormat" ) << mCRisenName( "PixelFormat" ) << ( MIU32 ) 8 << g_32( enuColorFormatHash ) << g_32( enuColorFormat );
    streamDest << ( MIU32 ) 1 << mCRisenName( "class eCImageResource2" ) << ( MIU16 ) 1 << ( MIU32 ) 32 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0;
    uOffset4 = streamDest.Tell();
    streamDest.Seek( uOffset );
    streamDest << g_32( uOffset4 - uOffset - 4 );
    streamDest.Seek( uOffset2 );
    streamDest << g_32( uOffset4 - uOffset2 - 4 );
    streamDest.Seek( uOffset3 );
    streamDest << g_32( uOffset4 - uOffset3 - 4 );
    return MITrue;
}

int main( int argc, char* argv[] )
{
    if ( argc < 2 )
    {
        printf( "Risen 3 Resource Manager v1.0 by Baltram\n"
                "Start by dragging a file or folder onto the r3resman.exe file.\n\n"
                "Supported file/folder types and actions:\n"
                "  <folder>                            : Create .pak volume\n"
                "  Risen 3 PAK volume (.pak)           : Unpack\n"
                "  Risen 3 image (.r3img)              : Convert to .dds\n"
                "  Risen 3 sound (.r3snd )             : Convert to .wav\n"
                "  Risen 3 dialog (.r3dlg)             : Convert to .wav and .r3dlgdoc\n"
                "  Risen 3 template (.r3tpl)           : Convert to .r3tpldoc\n"
                "  DDS image (.dds)                    : Convert to .r3img\n"
                "  WAV sound (.wav)                    : Convert to .r3snd\n"
                "  Risen 3 dialog document (.r3dlgdoc) : Convert to .r3dlg\n\n" );
        WaitForEnterKey( MITrue );
        return 1;
    }
    for ( MIInt i = 2; i != argc; ++i )
    {
        if ( mCString( argv[ i ] ) == "--non-interactive" )
            s_bNonInteractive = MITrue;
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
    mCGenomeVolume::RegisterZlibCompressFunction( &compress );
    mCGenomeVolume::RegisterZlibUncompressFunction( &uncompress );
    GetFullPathNameA( argv[ 1 ], mCString::GetStaticBufferSize(), mCString::AccessStaticBuffer(), NULL );
    mCString strPath( mCString::AccessStaticBuffer() );
    DWORD dwAttrib = GetFileAttributesA( strPath.GetText() );
    if ( dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
    {
        if ( CreateRisen3Volume( strPath ) )
            return 0;
        WaitForEnterKey( MITrue );
        return 1;
    }
    mCFileStream streamIn( strPath, mEFileOpenMode_Read );
    if ( g_GetFileExt( strPath ).ToLower() == "r3dlgdoc" )
        return DlgDocToDlg( streamIn, strPath ) ? 0 : 1;
    MIBool bIsResourceFile = MIFalse, bIsDdsFile = MIFalse, bIsWavFile = MIFalse;
    MIBool bSuccess = ReadResourceFile( streamIn, strPath, bIsResourceFile );
    if ( bIsResourceFile )
        return bSuccess ? 0 : 1;
    bSuccess = DdsToImg( streamIn, strPath, bIsDdsFile );
    if ( bIsDdsFile )
        return bSuccess ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "dds" )
    {
        printf( "Error: Invalid .dds file: %s\n", strPath.GetText() );
        WaitForEnterKey( MITrue );
        return 1;
    }
    bSuccess = WavToSnd( streamIn, strPath, bIsWavFile );
    if ( bIsWavFile )
        return bSuccess ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "wav" )
    {
        printf( "Error: Invalid .wav file: %s\n", strPath.GetText() );
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
    mCGenomeVolume::CDir const & Root = Pak.GetRoot();
    mCString strResourceType = Pak.GetRisen3ResourceType();
    mCString strDir = g_GetDirectoryPath( strPath );
    if ( strDir == "" )
        strDir = ".";
    MIBool bResult = CreateFiles( strDir + "\\" + ( strResourceType == "" ? g_GetFileNameNoExt( strPath ) : strResourceType ), Root );
    if ( bResult )
    {
        printf( "Unpacking successful.\n" );
        return 0;
    }
    else
        printf( "Error when unpacking %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strPath.GetText() );
    if ( s_bDebug && mCError::GetLastError< mCError >() )
        printf( "%s\n", mCError::GetLastError< mCError >()->GetText() );
    WaitForEnterKey( MITrue );
    return 1;
}