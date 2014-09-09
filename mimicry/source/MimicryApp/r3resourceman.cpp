#include <cstdio>
#include "..\Mimicry.h"
#include "Windows.h"
#include "..\..\..\zlib\zlib.h"

namespace
{
    MIBool s_bDebug = MIFalse;
}

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MIFalse )
{
	if ( a_bPrintDefaultMessage )
		printf( "Press ENTER to continue... " );
	for ( MIChar c = 0; c != '\n' && c != EOF; c = getchar() );
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
        if ( s_uCreatedFileCount * 10 / s_uTotalFileCount != ( s_uCreatedFileCount - 1 ) * 10 / s_uTotalFileCount )
            printf( "%u%%\n", s_uCreatedFileCount * 10 / s_uTotalFileCount * 10 );
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

MIBool ReadImageFile( mCIOStreamBinary & streamIn, mCString const & a_strFilePath )
{
    streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    streamIn >> u32Data1Offset >> u32Data1Size;
    streamIn.Seek( u32Data1Offset );
    mCString const strDDSPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".dds";
    mCFileStream streamDDS( strDDSPath, mEFileOpenMode_Write );
    if ( !streamDDS.IsOpen() )
    {
        printf( "Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strDDSPath.GetText() );
        WaitForEnterKey( MITrue );
        return MITrue;
    }
    mCByteArray arrBuffer;
    arrBuffer.Resize( u32Data1Size );
    streamIn.Read( arrBuffer.AccessBuffer(), u32Data1Size );
    streamDDS.Write( arrBuffer.GetBuffer(), u32Data1Size );
    return MITrue;
}

MIBool ReadSoundFile( mCIOStreamBinary & streamIn, mCString const & a_strFilePath )
{
    streamIn.Skip( -44 );
    MIU32 u32Data1Offset, u32Data1Size;
    streamIn >> u32Data1Offset >> u32Data1Size;
    streamIn.Seek( u32Data1Offset );
    mCString const strWAVPath = g_GetDirectoryPath( a_strFilePath ) + "\\" + g_GetFileNameNoExt( a_strFilePath ) + ".wav";
    mCFileStream streamWAV( strWAVPath, mEFileOpenMode_Write );
    if ( !streamWAV.IsOpen() )
    {
        printf( "Could not create %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", strWAVPath.GetText() );
        WaitForEnterKey( MITrue );
        return MITrue;
    }
    mCByteArray arrBuffer;
    arrBuffer.Resize( u32Data1Size );
    streamIn.Read( arrBuffer.AccessBuffer(), u32Data1Size );
    streamWAV.Write( arrBuffer.GetBuffer(), u32Data1Size );
    return MITrue;
}

MIBool ReadResourceFile( mCIOStreamBinary & streamIn, mCString const & a_strFilePath )
{
    if ( streamIn.ReadString( 4 ) != "R3RF" )
        return MIFalse;
    streamIn.Seek( streamIn.ReadU32() + 4 );
    mCString const strResourceRevision = streamIn.ReadString( 4 );
    if ( strResourceRevision == "IM06" )
        return ReadImageFile( streamIn, a_strFilePath );
    if ( strResourceRevision == "SN09" || strResourceRevision == "DI14" )
        return ReadSoundFile( streamIn, a_strFilePath );
    printf( "Error: Unsupported resource type.\n" );
    WaitForEnterKey( MITrue );
    return MITrue;
}

int main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        printf( "Please drag a Risen 3 PAK file (.pak), a Risen 3 image (.r3img) or a Risen sound (.r3snd .r3dlg) onto the r3resourceman.exe\n" );
        WaitForEnterKey( MITrue );
        return 0;
    }
    mCFileStream streamIn( argv[ 1 ], mEFileOpenMode_Read );
    if ( ReadResourceFile( streamIn, argv[ 1 ] ) )
        return 0;
    streamIn.Close();
    mCGenomeVolume::RegisterZlibUncompressFunction( &uncompress );
    mCGenomeVolume Pak( argv[ 1 ], MITrue );
    if ( !Pak.IsOpen() )
    {
        printf( "Error: %s is neither a Risen 3 resource file nor a PAK file.\n", argv[ 1 ] );
        WaitForEnterKey( MITrue );
        return 0;
    }
    mCGenomeVolume::CDir const & Root = Pak.GetRoot();
    mCString strResourceType = Pak.GetRisen3ResourceType();
    MIBool bResult = CreateFiles( g_GetDirectoryPath( argv[ 1 ] ) + "\\" + ( strResourceType == "" ? g_GetFileNameNoExt( argv[ 1 ] ) : strResourceType ), Root );
    if ( bResult )
    {
        printf( "Unpacking successful.\n" );
        return 0;
    }
    else
        printf( "Error when unpacking %s\nThe problem might be missing access rights. Try starting the program with admin rights.\n", argv[ 1 ] );
    if ( s_bDebug && mCError::GetLastError< mCError >() )
    {
        printf( mCError::GetLastError< mCError >()->GetText() );
        printf( "\n" );
    }
    WaitForEnterKey( MITrue );
    return 0;
}