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
    if ( 0 == Doc.DocumentRisen3Class() || ( a_streamIn.Seek( u32Data1Offset ), Doc.WriteLine(), !Doc.DocumentRisen3Sector() ) )
    {
        printf( "Error: Unknown sector file version.\n" );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    mCString const strDocPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".r3secdoc";
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
    if ( strResourceRevision == "SC02" )
        return SecToSecDoc( streamIn, a_strFilePath );
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
    mCRisenName::InitializeRisen3Strings();
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
        if ( strExt == "r3tpldoc" )
            arrTemplateDocs.Add( arrFiles[ u ] );
        else if ( strExt == "r3secdoc" )
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
    mCString const strR3SecPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".r3sec";
    mCMemoryStream streamSec;
    mCMemoryStream streamResourceClass;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamSec );
    mCRisenDocParser ResourceClassParser( streamDoc, streamResourceClass );
    if ( !ResourceClassParser.ParseRisen3Class( MITrue ) )
    {
        printf( "Error: Parse error at line %u.\n", ResourceClassParser.GetLastErrorLine() );
        WaitForEnterKey( MITrue );
        return MIFalse;
    }
    if ( !Parser.ParseRisen3Sector( MITrue ) )
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
    streamDest << "R3RF" << ( MIU32 )( streamSec.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamSec;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamSec.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeRisen3Strings();
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
    mCString const strR3TplPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + strResourceName + ".r3tpl";
    mCMemoryStream streamTpl;
    mCStringStream streamDoc;
    a_streamIn >> streamDoc;
    streamDoc.Seek( 0 );
    mCRisenDocParser Parser( streamDoc, streamTpl );
    if ( !Parser.ParseRisen3Template( strResourceName, MITrue ) )
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
    streamDest << "R3RF" << ( MIU32 )( streamTpl.GetSize() + 8 + 36 + 36 ) << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU64 ) 0 << ( MIU32 ) 0;
    streamDest << streamTpl;
    mCMemoryStream streamOffsetTable;
    MIUInt uOffset = streamDest.Tell(), uOffset2, uOffset3, uOffset4;
    streamOffsetTable << ( MIU32 )( 8 + 36 ) << g_32( streamTpl.GetSize() ) << ( MIU8 ) 0;
    for ( MIUInt u = 3; u--; )
        streamOffsetTable << g_32( uOffset ) << ( MIU32 ) 0 << ( MIU8 ) 0;
    streamDest << streamOffsetTable;
    mCRisenName::InitializeRisen3Strings();
    streamDest << mCRisenName( "class gCTemplateResource" ) << "TP02";
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
                "  <folder>                              : Create .pak volume\n"
                "  Risen 3 PAK volume (.pak)             : Unpack\n"
                "  Risen 3 PAK volume, renamed (.pak.x)  : Update archive generation to x\n"
                "  Risen 3 image (.r3img)                : Convert to .dds\n"
                "  Risen 3 sound (.r3snd )               : Convert to .wav\n"
                "  Risen 3 dialog (.r3dlg)               : Convert to .wav and .r3dlgdoc\n"
                "  Risen 3 template (.r3tpl)             : Convert to .r3tpldoc\n"
                "  Risen 3 sector (.r3sec)               : Convert to .r3secdoc\n"
                "  DDS image (.dds)                      : Convert to .r3img\n"
                "  WAV sound (.wav)                      : Convert to .r3snd\n"
                "  Risen 3 dialog document (.r3dlgdoc)   : Convert to .r3dlg\n"
                "  Risen 3 template document (.r3tpldoc) : Convert to .r3tpl\n"
                "  Risen 3 sector document (.r3secdoc)   : Convert to .r3sec\n"
                "  Sector modification file (.r3secmod)  : Process\n\n" );
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
    if ( dwAttrib != INVALID_FILE_ATTRIBUTES && dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
    {
        if ( CreateRisen3Volume( strPath ) )
            return 0;
        WaitForEnterKey( MITrue );
        return 1;
    }
    mCFileStream streamIn( strPath, mEFileOpenMode_Read );
    if ( g_GetFileExt( strPath ).ToLower() == "r3dlgdoc" )
        return DlgDocToDlg( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "r3tpldoc" )
        return TplDocToTpl( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "r3secdoc" )
        return SecDocToSec( streamIn, strPath ) ? 0 : 1;
    else if ( g_GetFileExt( strPath ).ToLower() == "r3secmod" )
        return ProcessSecMod( streamIn, strPath ) ? 0 : 1;
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
    MIUInt uNumber = MI_DW_INVALID;
    if ( g_GetFileExt( strPath ).Scan( "%u", &uNumber ) == 1 && strPath.Lower().EndsWith( mCString().Format( ".pak.%u", uNumber ) ) )
    {
        if ( UpdateArchiveGeneration( strPath ) )
            return 0;
        WaitForEnterKey( MITrue );
        return 1;
    }
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