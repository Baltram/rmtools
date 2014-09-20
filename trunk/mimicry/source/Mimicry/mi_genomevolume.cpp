#include "mi_include_converters.h"
#include "mi_include_standard.h"

namespace
{
    MILPCChar const s_arrResourceTokens[] = {
        "img",
        "shd",
        "mat",
        "msh",
        "fnt",
        "snd",
        "spt",
        "col",
        "grp",
        "net",
        "skn",
        "bln",
        "bla",
        "dlg",
        "eff",
        "cut",
        "clo",
        "ocm",
        "dst",
        "nam",
        "thm",
        "lpb",
        // Speculation
        "prj",
        "sec",
        "tpl",
        "wrl"
    };

    MILPCChar const s_arrResourceTypes[] = {
        "Image",
        "Shader",
        "Material",
        "Mesh",
        "Font",
        "Sound",
        "SpeedTree",
        "CollisionMesh",
        "AnimationGroup",
        "AnimationNetwork",
        "SkinnedMesh",
        "BlendShape",
        "BlendAnimation",
        "Dialogue",
        "Effect",
        "Cutscene",
        "ClothMesh",
        "OcclusionMesh",
        "Destructible",
        "NavMesh",
        "TerrainHeightMap",
        "LightProbe",
        // Speculation
        "Project",
        "Sector",
        "Template",
        "World"
    };
}

MIInt ( *mCGenomeVolume::s_pfuncCompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
MIInt ( *mCGenomeVolume::s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );

namespace
{
    MIBool                s_bResourceDatabase = MIFalse;
    MIBool                s_bFirstDatabaseEntry = MIFalse;
    MIUInt                s_uGeneration = 0;
    mCString              s_strLanguage = "na";
    mCString              s_strResourceToken = "img";
    mCString              s_strProblematicFilePath;
    mCIOStreamBinary    * s_pDataStream = 0;
    mCIOStreamBinary    * s_pEntryStream = 0;
    mCMemoryStream      * s_pDBStream = 0;
    mCMemoryStream      * s_pCSVStream = 0;
    MIUInt                s_uTotalFileCount = 0;
    MIUInt                s_uCurrentFileCount = 0;
    void              ( * s_pfuncShowProgress )( MIUInt, MIUInt ) = 0;

    MIU64 SwapHighLow( MIU64 a_u64Time )
    {
        return ( a_u64Time << 32 ) | ( a_u64Time >> 32 );
    }
}

mCGenomeVolume::CFileHandle::CFileHandle( mCGenomeVolume * a_pVolume, MIUInt a_uFileIndex, MIBool a_bResolveRisen3Entries ) :
    m_pVolume( a_pVolume ),
    m_uFileIndex( a_uFileIndex ),
    m_bResolveRisen3Entries( a_bResolveRisen3Entries )
{
}

mCGenomeVolume::CFileHandle::CFileHandle( void ) :
    m_pVolume( 0 ),
    m_uFileIndex( MI_DW_INVALID ),
    m_bResolveRisen3Entries( MIFalse )
{
}

MIU64 mCGenomeVolume::CFileHandle::GetCreationTime( void ) const
{
    return m_pVolume->m_arrFiles[ m_uFileIndex ].m_u64Created;
}

MIBool mCGenomeVolume::CFileHandle::GetFileData( mCIOStreamBinary & a_streamFileDataDest ) const
{
    SFile const & File = m_pVolume->m_arrFiles[ m_uFileIndex ];
    mCByteArray arrFileData;
    arrFileData.Resize( File.m_uDataSize );
    m_pVolume->m_streamArchive.Seek( File.m_uDataOffset );
    m_pVolume->m_streamArchive.Read( arrFileData.AccessBuffer(), File.m_uDataSize );
    if ( File.m_bCompressed )
    {
        MIU32 u32FileSize = File.m_uFileSize;
        mCByteArray arrUncompressedFileData;
        arrUncompressedFileData.Resize( u32FileSize );
        if ( ( *mCGenomeVolume::s_pfuncUncompress )( arrUncompressedFileData.AccessBuffer(), &u32FileSize, arrFileData.GetBuffer(), arrFileData.GetCount() ) != 0 )
            return MIFalse;
        arrUncompressedFileData.Swap( arrFileData );
    }
    a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
    if ( m_bResolveRisen3Entries )
    {
        MIU32 u32Offset = a_streamFileDataDest.Tell();
        arrFileData.Resize( File.m_uDatabaseEntrySize );
        m_pVolume->m_streamResourceDatabase.Seek( File.m_uDatabaseOffset );
        m_pVolume->m_streamResourceDatabase.Read( arrFileData.AccessBuffer(), File.m_uDatabaseEntrySize );
        a_streamFileDataDest << m_pVolume->m_u32ResourceClassHash << m_pVolume->m_u32ResourceClassRevision << ( MIU32 )( File.m_uDatabaseEntrySize );
        a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
        a_streamFileDataDest.Seek( 0 );
        a_streamFileDataDest << "R3RF" << u32Offset;
    }
    a_streamFileDataDest.Seek( 0 );
    return MITrue;
}

MIUInt mCGenomeVolume::CFileHandle::GetFileIndex( void ) const
{
    return m_uFileIndex;
}

mCString mCGenomeVolume::CFileHandle::GetFilePath( void ) const
{
    if ( m_bResolveRisen3Entries )
        return "\\" + m_pVolume->m_arrFiles[ m_uFileIndex ].m_strResourceName + ".r3" + m_pVolume->m_strResourceToken;
    return m_pVolume->m_arrFiles[ m_uFileIndex ].m_strFilePath;
}

MIU64 mCGenomeVolume::CFileHandle::GetLastAccessTime( void ) const
{
    return m_pVolume->m_arrFiles[ m_uFileIndex ].m_u64Accessed;
}

MIU64 mCGenomeVolume::CFileHandle::GetLastWriteTime( void ) const
{
    return m_pVolume->m_arrFiles[ m_uFileIndex ].m_u64Modified;
}

mCGenomeVolume::CDir::CDir( void )
{
    m_u64Accessed = m_u64Created = m_u64Modified = 0;
}

MIU64 & mCGenomeVolume::CDir::AccessCreationTime( void )
{
    return m_u64Created;
}

mTArray< mCGenomeVolume::CFileHandle > & mCGenomeVolume::CDir::AccessFiles( void )
{
    return m_arrFiles;
}

MIU64 & mCGenomeVolume::CDir::AccessLastAccessTime( void )
{
    return m_u64Accessed;
}

MIU64 & mCGenomeVolume::CDir::AccessLastWriteTime( void )
{
    return m_u64Modified;
}

mCString & mCGenomeVolume::CDir::AccessName( void )
{
    return m_strName;
}

mTArray< mCGenomeVolume::CDir >  & mCGenomeVolume::CDir::AccessSubDirs( void )
{
    return m_arrSubDirs;
}

MIU64 mCGenomeVolume::CDir::GetCreationTime( void ) const
{
    return m_u64Created;
}

MIUInt mCGenomeVolume::CDir::GetFileCount( void ) const
{
    MIUInt uResult = m_arrFiles.GetCount();
    for ( MIUInt u = m_arrSubDirs.GetCount(); u--; uResult += m_arrSubDirs[ u ].GetFileCount() );
    return uResult;
}

mTArray< mCGenomeVolume::CFileHandle > const & mCGenomeVolume::CDir::GetFiles( void ) const
{
    return m_arrFiles;
}

MIU64 mCGenomeVolume::CDir::GetLastAccessTime( void ) const
{
    return m_u64Accessed;
}

MIU64 mCGenomeVolume::CDir::GetLastWriteTime( void ) const
{
    return m_u64Modified;
}

mCString const & mCGenomeVolume::CDir::GetName( void ) const
{
    return m_strName;
}

mTArray< mCGenomeVolume::CDir > const & mCGenomeVolume::CDir::GetSubDirs( void ) const
{
    return m_arrSubDirs;
}

mCGenomeVolume::mCGenomeVolume( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries ) :
    m_pRoot( 0 ),
    m_u32ResourceClassRevision( 0 ),
    m_u32ResourceClassHash( 0 )
{
    Open( a_strFilePath, a_bResolveRisen3Entries );
}

mCGenomeVolume::mCGenomeVolume( void ) :
    m_pRoot( 0 ),
    m_u32ResourceClassRevision( 0 ),
    m_u32ResourceClassHash( 0 )
{
}

mCGenomeVolume::~mCGenomeVolume( void )
{
    delete m_pRoot;
}

namespace
{
    enum RisenPakAttributes
    {
        RisenPakAttribute_ReadOnly   = 0x00000001,  // FILE_ATTRIBUTE_READONLY
        RisenPakAttribute_Hidden     = 0x00000002,  // FILE_ATTRIBUTE_HIDDEN
        RisenPakAttribute_System     = 0x00000004,  // FILE_ATTRIBUTE_SYSTEM
        RisenPakAttribute_Directory  = 0x00000010,  // FILE_ATTRIBUTE_DIRECTORY
        RisenPakAttribute_Archive    = 0x00000020,  // FILE_ATTRIBUTE_ARCHIVE
        RisenPakAttribute_Temporary  = 0x00000100,  // FILE_ATTRIBUTE_TEMPORARY
        RisenPakAttribute_Compressed = 0x00000800,  // FILE_ATTRIBUTE_COMPRESSED
        RisenPakAttribute_NotIndexed = 0x00002000,  // FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
        RisenPakAttribute_Encrypted  = 0x00004000,  // FILE_ATTRIBUTE_ENCRYPTED
        RisenPakAttribute_Deleted    = 0x00008000,
        RisenPakAttribute_Virtual    = 0x00010000,  // FILE_ATTRIBUTE_VIRTUAL
        RisenPakAttribute_Packed     = 0x00020000,
        RisenPakAttribute_Stream     = 0x00040000,
        RisenPakAttribute_Stream2    = 0x00080000
    };  // Thanks NicoDE

    void UpdateDirTime( mCGenomeVolume::CDir & a_Dir, mCGenomeVolume::SFileTime const & a_Time )
    {
        if ( s_bResourceDatabase )
        {
            a_Dir.AccessCreationTime() = a_Dir.AccessLastAccessTime() = a_Dir.AccessLastWriteTime() = g_time();
            return;
        }
        if ( a_Dir.GetCreationTime() == 0 || a_Dir.GetCreationTime() > a_Time.m_u64Created )
            a_Dir.AccessCreationTime() = a_Time.m_u64Created;
        if ( a_Dir.GetLastAccessTime() < a_Time.m_u64Accessed )
            a_Dir.AccessLastAccessTime() = a_Time.m_u64Accessed;
        if ( a_Dir.GetLastWriteTime() < a_Time.m_u64Modified )
            a_Dir.AccessLastWriteTime() = a_Time.m_u64Modified;
    }

    mCGenomeVolume::CDir & ReviseDirTime( mCGenomeVolume::CDir & a_Dir )
    {
        for ( MIUInt u = a_Dir.GetSubDirs().GetCount(); u--; )
            UpdateDirTime( a_Dir, ReviseDirTime( a_Dir.AccessSubDirs()[ u ] ) );
        return a_Dir;
    }

    mCString GetArchiveDirPath( mCString const & a_strFilePath, mCString const & a_strRootPath )
    {
        if ( !s_bResourceDatabase )
            return g_GetDirectoryPath( a_strFilePath ).TrimLeft( a_strRootPath.GetLength() );
        MIU32 u32Hash = g_djb2( g_GetFileNameNoExt( a_strFilePath ).ToLower().GetText() );
        return "\\0\\" + mCString().Format( "%.8x", u32Hash ).Left( 1 );
    }
}

mEResult mCGenomeVolume::CreateRisen3Archive( mCString a_strRootPath, mTArray< mCString > const & a_arrFilePaths, mTArray< SFileTime > const & a_arrFileTimes, mCIOStreamBinary & a_streamDest, MIUInt ( *a_pfuncRequestGeneration )( void ), mCString * a_pVolumeName, void ( * a_pfuncShowProgress )( MIUInt, MIUInt ) )
{
    s_uTotalFileCount = a_arrFilePaths.GetCount();
    s_uCurrentFileCount = 0;
    s_pfuncShowProgress = a_pfuncShowProgress;
    MIBool bRisen3Resource = MIFalse, bMixedFileExt = MIFalse;
    mCString strFirstExt = a_arrFilePaths.GetCount() ? g_GetFileExt( a_arrFilePaths[ 0 ] ).ToLower() : "";
    CDir Root;
    a_strRootPath.TrimRight( "\\/" );
    for ( MIUInt u = a_arrFilePaths.GetCount(); u--; )
    {
        mCString strExt = g_GetFileExt( a_arrFilePaths[ u ] ).ToLower();
        if ( !bRisen3Resource && strExt.Left( 2 ) == "r3" )
            for ( MIUInt v = sizeof( s_arrResourceTokens ) / sizeof( *s_arrResourceTokens ); v--; )
                if ( strExt == mCString( "r3" ) + s_arrResourceTokens[ v ] )
                    bRisen3Resource = MITrue;
        if ( !bMixedFileExt && strExt != strFirstExt /*&& strExt != "deleted"*/ )
            bMixedFileExt = MITrue;
    }
    if ( bRisen3Resource && bMixedFileExt )
        return MI_ERROR( mCGenomeArchiveError, EMixedRisen3ResourceTypes, "Failed to create Risen3 PAK archive because input folder contains mixed resource types." ), mEResult_False;
    s_bResourceDatabase = s_bFirstDatabaseEntry = bRisen3Resource;
    for ( MIUInt u = a_arrFilePaths.GetCount(); u--; )
    {
        CDir & Dir = GetDir( Root, GetArchiveDirPath( a_arrFilePaths[ u ], a_strRootPath ) );
        Dir.AccessFiles().Add( CFileHandle( 0, u ) );
        UpdateDirTime( Dir, a_arrFileTimes[ u ] );
    }
    ReviseDirTime( Root );
    mCMemoryStream streamEntryTable, streamDB, streamCSV;
    s_pDataStream = &a_streamDest;
    s_pEntryStream = &streamEntryTable;
    s_pCSVStream = &streamCSV;
    s_pDBStream = &streamDB;
    if ( bRisen3Resource )
    {
        s_uTotalFileCount += 2;
        s_uGeneration = ( *a_pfuncRequestGeneration )();
        s_strLanguage = "na";
        if ( g_GetFileName( a_strRootPath ).Scan( "%*[^_]_%99s", mCString::AccessStaticBuffer() ) == 1 )
            s_strLanguage.SetText( mCString::AccessStaticBuffer() );
        if ( s_strLanguage.Scan( "%*[^_]_%99s", mCString::AccessStaticBuffer() ) == 1 )  // Backward compatibility
            s_strLanguage.SetText( mCString::AccessStaticBuffer() );
        s_strResourceToken = strFirstExt.Right( 3 ).ToLower();
        streamCSV << "Hash|Name|Raw File Time|Compiled File Time|\r\n";
        streamDB << "GAR5" << ( MIU32 ) 0x20 << "RT02" << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 )( a_arrFilePaths.GetCount() );
    }
    a_streamDest << ( MIU32 ) 1 << "G3V0" << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 1 << ( MIU32 ) 0xFEEDFACE << ( MIU64 ) 48 << ( MIU64 ) 0 << ( MIU64 ) 0;  // Header
    if ( !WritePakDir( Root, a_arrFilePaths, a_arrFileTimes ) )
        return MI_ERROR( mCGenomeArchiveError, ESourceFileOpenError, mCString().Format( "Failed to create Risen 3 volume. Error when processing %s", s_strProblematicFilePath.GetText() ).GetText() ), mEResult_False;
    if ( bRisen3Resource )
    {
        WritePakFile( "<db>", Root );
        WritePakFile( "<csv>", Root );
    }
    MIU64 const u64RootOffset = a_streamDest.Tell();
    a_streamDest << streamEntryTable;
    MIU64 const u64VolumeSize = a_streamDest.Tell();
    a_streamDest.Seek( 32 );
    a_streamDest << u64RootOffset << u64VolumeSize;
    a_streamDest.Seek( static_cast< MIUInt >( u64VolumeSize ) );
    if ( !a_pVolumeName )
        return mEResult_Ok;
    if ( bRisen3Resource )
        a_pVolumeName->Format( "%u_%s_%s.pak", s_uGeneration, s_strLanguage.GetText(), s_strResourceToken.GetText() );
    else
        *a_pVolumeName = g_GetFileName( a_strRootPath + ".pak" );
    return mEResult_Ok;
}

void mCGenomeVolume::RegisterZlibCompressFunction( MIInt ( *a_pfuncCompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) )
{
    s_pfuncCompress = a_pfuncCompress;
}

void mCGenomeVolume::RegisterZlibUncompressFunction( MIInt ( * a_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) )
{
    s_pfuncUncompress = a_pfuncUncompress;
}

void mCGenomeVolume::Close( void )
{
    m_arrFiles.Clear();
    m_mapFileIndices.Clear();
    m_mapDirTimes.Clear();
    m_streamArchive.Close();
    m_pRoot = 0;
    m_strRisen3Name = m_strResourceToken = m_strLanguage = "";
    m_u32ResourceClassRevision = m_u32ResourceClassHash = 0;
    m_streamResourceDatabase.Clear();
}

mCString mCGenomeVolume::GetRisen3ResourceType( void )
{
    for ( MIUInt u = sizeof( s_arrResourceTokens ) / sizeof( *s_arrResourceTokens ); u--; )
        if ( s_arrResourceTokens[ u ] == m_strResourceToken )
            return s_arrResourceTypes[ u ] + ( m_strLanguage != "na" ? "_" + m_strLanguage : "" );
    return "";
}

mCGenomeVolume::CDir const & mCGenomeVolume::GetRoot( void )
{
    if ( m_pRoot )
        return *m_pRoot;
    CDir & Root = *( m_pRoot = new CDir );
    if ( m_strRisen3Name != "" )
    {
        for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
            if ( m_arrFiles[ u ].m_strResourceName != "" )
                Root.AccessFiles().Add( CFileHandle( this, u, MITrue ) );
    }
    else
    {
        for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
            GetDir( Root, g_GetDirectoryPath( m_arrFiles[ u ].m_strFilePath ), &m_mapDirTimes ).AccessFiles().Add( CFileHandle( this, u ) );
    }
    Root.AccessCreationTime() = Root.AccessLastAccessTime() = Root.AccessLastWriteTime() = 0;
    return Root;
}

MIBool mCGenomeVolume::FindFile( mCString a_strFileBaseName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest )
{
    mCByteArray arrFileData;
    MIUInt uFile;
    if ( !m_mapFileIndices.GetAt( a_strFileBaseName.ToLower(), uFile ) || ( !s_pfuncUncompress && m_arrFiles[ uFile ].m_bCompressed ) )
        return MIFalse;
    SFile const & File = m_arrFiles[ uFile ];
    a_strFilePathDest = File.m_strFilePath;
    a_streamFileDataDest.Clear();
    arrFileData.Resize( File.m_uDataSize );
    m_streamArchive.Seek( File.m_uDataOffset );
    m_streamArchive.Read( arrFileData.AccessBuffer(), File.m_uDataSize );
    if ( File.m_bCompressed )
    {
        MIU32 u32FileSize = File.m_uFileSize;
        mCByteArray arrUncompressedFileData;
        arrUncompressedFileData.Resize( u32FileSize );
        if ( ( *s_pfuncUncompress )( arrUncompressedFileData.AccessBuffer(), &u32FileSize, arrFileData.GetBuffer(), arrFileData.GetCount() ) != 0 )
            return MIFalse;
        arrUncompressedFileData.Swap( arrFileData );
    }
    a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
    return MITrue;
}

MIBool mCGenomeVolume::IsOpen( void )
{
    return m_streamArchive.IsOpen();
}

mEResult mCGenomeVolume::Open( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries )
{
    Close();
    m_streamArchive.Open( a_strFilePath, mEFileOpenMode_Read );
    if ( !m_streamArchive.IsOpen() || m_streamArchive.GetSize() < 48 )
        return Close(), mEResult_False;
    MIUInt const uVersion = m_streamArchive.ReadU32();
    mCString const strId = m_streamArchive.ReadString( 4 );
    m_streamArchive.Skip( 4 );
    MIUInt const uEncryption = m_streamArchive.ReadU32();
    MIUInt const uCompression = m_streamArchive.ReadU32();
    if ( uVersion > 1 || strId != "G3V0" || uEncryption || uCompression > 2 )
        return Close(), mEResult_False;
    m_streamArchive.Skip( 12 );
    MIUInt const uRootOffset = static_cast< MIUInt >( m_streamArchive.ReadU64() );
    m_streamArchive.Seek( uRootOffset );
    if ( uVersion == 0 )
        ReadOldVersionedEntry( m_streamArchive, m_arrFiles, "" );
    else
        ReadNewVersionedDirectory( m_streamArchive, m_arrFiles, "" );
    mCString strFileNameDB, strFileNameCSV;
    m_mapFileIndices.Reserve( m_arrFiles.GetCount() );
    for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
    {
        mCString const & strFilePath = m_arrFiles[ u ].m_strFilePath;
        m_mapFileIndices[ g_GetFileNameNoExt( strFilePath ).ToLower() ] = u;
        m_mapFileIndices[ g_GetFileName( strFilePath ).ToLower() ] = u;
        if ( g_GetFileName( strFilePath ).Count( '_' ) == 3 )
        {
            if ( g_GetFileExt( strFilePath ).ToLower() == "db" )
                strFileNameDB = g_GetFileNameNoExt( strFilePath );
            if ( g_GetFileExt( strFilePath ).ToLower() == "csv" )
                strFileNameCSV = g_GetFileNameNoExt( strFilePath );
        }
    }
    if ( !a_bResolveRisen3Entries || strFileNameDB == "" || strFileNameDB != strFileNameCSV )
        return mEResult_Ok;
    // Risen 3 PAK
    m_strRisen3Name = strFileNameDB;
    mCString strTemp;
    mCMemoryStream streamBinaryCSV;
    if ( !FindFile( strFileNameCSV + ".csv", strTemp, streamBinaryCSV ) ||
         !FindFile( strFileNameDB + ".db", strTemp, m_streamResourceDatabase ) ||
         strFileNameCSV.Scan( "%*c_%3c_%*c_%s", mCString::AccessStaticBuffer(), mCString::AccessStaticBuffer() + 3 ) != 2 )
        return Close(), mEResult_False;
    m_strResourceToken.SetText( mCString::AccessStaticBuffer(), 3 );
    m_strResourceToken.ToLower();
    m_strLanguage.SetText( mCString::AccessStaticBuffer() + 3 );
    m_strLanguage.ToLower();
    mCStringStream streamCSV;
    streamCSV << streamBinaryCSV;
    streamCSV.Seek( 0 );
    mCString strLine = streamCSV.ReadLine();
    MIUInt const uColumnCount = strLine.Count( '|' );
    MIUInt uHashColumn = 0, uNameColumn = 0;
    for ( MIUInt u = 0; u != uColumnCount; ++u )
    {
        mCString strColumnTitle = strLine.Left( strLine.FirstOf( '|' ) );
        strLine.TrimLeft( strColumnTitle.GetLength() + 1 );
        if ( strColumnTitle.ToLower() == "hash" )
            uHashColumn = u;
        else if ( strColumnTitle.ToLower() == "name" )
            uNameColumn = u;
    }
    while ( streamCSV.Tell() != streamCSV.GetSize() )
    {
        strLine = streamCSV.ReadLine();
        mCString strHash, strName;
        for ( MIUInt u = 0; u != uColumnCount; ++u )
        {
            mCString strEntry = strLine.Left( strLine.FirstOf( '|' ) );
            strLine.TrimLeft( strEntry.GetLength() + 1 );
            if ( u == uHashColumn )
                strHash = strEntry;
            else if ( u == uNameColumn )
                strName = strEntry;
        }
        MIUInt uFileIndex;
        if ( !m_mapFileIndices.GetAt( ( m_strRisen3Name + "_" + strHash ).ToLower(), uFileIndex ) )
            continue;
        SFile & File = m_arrFiles[ uFileIndex ];
        File.m_strResourceName = strName;
    }
    m_streamResourceDatabase.Seek( 0 );
    if ( m_streamResourceDatabase.ReadString( 4 ) != "GAR5" )
        return Close(), mEResult_False;
    m_streamResourceDatabase.Skip( 8 );
    m_u32ResourceClassHash = m_streamResourceDatabase.ReadU32();
    m_u32ResourceClassRevision = m_streamResourceDatabase.ReadU32();
    MIUInt uResourceCount = m_streamResourceDatabase.ReadU32();
    for ( MIUInt u = 0; u != uResourceCount; ++u )
    {
        MIUInt const uOffset = m_streamResourceDatabase.Tell();
        union
        {
            MIU32 RawTime[ 2 ];
            MIU64 u64RawTime;
        };
        m_streamResourceDatabase >> RawTime[ 1 ] >> RawTime[ 0 ];
        m_streamResourceDatabase.Skip( 24 );
        mCString strResourceName = m_streamResourceDatabase.ReadString( m_streamResourceDatabase.ReadU32() );
        m_streamResourceDatabase.Skip( 36 );
        MIUInt uReferencedFileCount = m_streamResourceDatabase.ReadU32();
        m_streamResourceDatabase.Skip( uReferencedFileCount * 8 );
        for ( MIUInt v = uReferencedFileCount; v--; m_streamResourceDatabase.Skip( m_streamResourceDatabase.ReadU32() ) );
        m_streamResourceDatabase.ReadU32();
        m_streamResourceDatabase.Skip( m_streamResourceDatabase.ReadU32() );
        MIUInt uFileIndex;
        if ( !m_mapFileIndices.GetAt( ( m_strRisen3Name + mCString().Format( "_%.8x", g_djb2( strResourceName.ToLower().GetText() ) ) ).ToLower(), uFileIndex ) )
            continue;
        if ( u64RawTime != 0 )
            m_arrFiles[ uFileIndex ].m_u64Created = m_arrFiles[ uFileIndex ].m_u64Modified = u64RawTime;
        m_arrFiles[ uFileIndex ].m_uDatabaseOffset = uOffset;
        m_arrFiles[ uFileIndex ].m_uDatabaseEntrySize = m_streamResourceDatabase.Tell() - uOffset;
    }
    return mEResult_Ok;
}

mCGenomeVolume::CDir & mCGenomeVolume::GetDir( mCGenomeVolume::CDir & a_Root, mCString const & a_strDirPath, mTStringMap< SFileTime > const * a_pDirTimes )
{
    if ( a_strDirPath == "" )
        return a_Root;
    mCString const strDirName = g_GetFileName( a_strDirPath );
    mCGenomeVolume::CDir & ParentDir = GetDir( a_Root, g_GetDirectoryPath( a_strDirPath ), a_pDirTimes );
    for ( MIUInt u = 0, ue = ParentDir.GetSubDirs().GetCount(); u != ue; ++u )
        if ( ParentDir.GetSubDirs()[ u ].GetName() == strDirName )
            return ParentDir.AccessSubDirs()[ u ];
    ParentDir.AccessSubDirs().AddNew().AccessName() = strDirName;
    CDir & Result = ParentDir.AccessSubDirs().Back();
    SFileTime DirTime;
    DirTime.m_u64Accessed = DirTime.m_u64Created = DirTime.m_u64Modified = 0;
    if ( a_pDirTimes )
        a_pDirTimes->GetAt( a_strDirPath + "\\", DirTime );
    Result.AccessCreationTime() = DirTime.m_u64Created;
    Result.AccessLastAccessTime() = DirTime.m_u64Accessed;
    Result.AccessLastWriteTime() = DirTime.m_u64Modified;
    return Result;
}

MIBool mCGenomeVolume::WritePakDir( mCGenomeVolume::CDir const & a_Dir, mTArray< mCString > const & a_arrFilePaths, mTArray< mCGenomeVolume::SFileTime > const & a_arrFileTimes )
{
    *s_pEntryStream << ( MIU32 )( a_Dir.GetName().GetLength() ) << a_Dir.GetName();
    if ( a_Dir.GetName().GetLength() )
        *s_pEntryStream << ( MIU8 ) 0;
    *s_pEntryStream << a_Dir.GetCreationTime() << a_Dir.GetLastAccessTime() << a_Dir.GetLastWriteTime();
    *s_pEntryStream << ( MIU32 )( RisenPakAttribute_Packed | RisenPakAttribute_Directory ) << ( MIU32 )( a_Dir.GetFiles().GetCount() + a_Dir.GetSubDirs().GetCount() + ( s_bResourceDatabase && a_Dir.GetName() == "" ? 2 : 0 ) );
    for ( MIUInt u = 0, ue = a_Dir.GetFiles().GetCount(); u != ue; ++u )
    {
        MIUInt uFile = a_Dir.GetFiles()[ u ].GetFileIndex();
        if ( !WritePakFile( a_arrFilePaths[ uFile ], a_arrFileTimes[ uFile ] ) )
            return MIFalse;
    }
    for ( MIUInt u = 0, ue = a_Dir.GetSubDirs().GetCount(); u != ue; ++u )
    {
        *s_pEntryStream << ( MIU32 )( RisenPakAttribute_Packed | RisenPakAttribute_Directory );
        if ( !WritePakDir( a_Dir.GetSubDirs()[ u ], a_arrFilePaths, a_arrFileTimes ) )
            return MIFalse;
    }
    return MITrue;
}

MIBool mCGenomeVolume::WritePakFile( mCString const & a_strFilePath, mCGenomeVolume::SFileTime const & a_Time )
{
    MIU64 u64CompiledTime = g_time();
    mCString strExt = g_GetFileExt( a_strFilePath ), strFileName = g_GetFileName( a_strFilePath ), strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCMemoryStream streamIn;
    if ( a_strFilePath == "<db>" )
    {
        streamIn.Swap( *s_pDBStream );
        strFileName.Format( "w_%s_%u_%s.db", s_strResourceToken.GetText(), s_uGeneration, s_strLanguage.GetText() );
    }
    else if ( a_strFilePath == "<csv>" )
    {
        streamIn.Swap( *s_pCSVStream );
        strFileName.Format( "w_%s_%u_%s.csv", s_strResourceToken.GetText(), s_uGeneration, s_strLanguage.GetText() );
    }
    else if ( streamIn.FromFile( a_strFilePath ) != mEResult_Ok )
        return s_strProblematicFilePath = a_strFilePath, MIFalse;
    if ( s_bResourceDatabase && a_strFilePath[ 0 ] != '<' )
    {
        MIU32 u32Hash = g_djb2( mCString( strResourceName ).ToLower().GetText() );
        strFileName.Format( "w_%s_%u_%s_%.8x.rom", s_strResourceToken.GetText(), s_uGeneration, s_strLanguage.GetText(), u32Hash );
        time_t TimeRaw = static_cast< time_t >( g_filetimeToCtime( a_Time.m_u64Modified ) ), TimeCompiled = static_cast< time_t >( g_filetimeToCtime( u64CompiledTime ) );
        MI_CRT_NO_WARNINGS( struct tm * pTimeInfoRaw = localtime( &TimeRaw ); )
        strftime( mCString::AccessStaticBuffer(), 80, "%d.%m.%Y - %H:%M:%S", pTimeInfoRaw );
        mCString strTimeRaw( mCString::AccessStaticBuffer() );
        MI_CRT_NO_WARNINGS( struct tm * pTimeInfoCompiled = localtime( &TimeCompiled ); )
        strftime( mCString::AccessStaticBuffer(), 80, "%d.%m.%Y - %H:%M:%S", pTimeInfoCompiled );
        mCString strTimeCompiled( mCString::AccessStaticBuffer() );
        *s_pCSVStream << mCString().Format( "%.8x|%s|%s|%s|\r\n", u32Hash, strResourceName.GetText(), strTimeRaw.GetText(), strTimeCompiled.GetText() );
        if ( streamIn.ReadString( 4 ) != "R3RF" )
            return s_strProblematicFilePath = a_strFilePath, MIFalse;
        MIUInt uRomSize = streamIn.ReadU32();
        streamIn.Seek( uRomSize );
        if ( s_bFirstDatabaseEntry )
        {
            s_bFirstDatabaseEntry = MIFalse;
            MIU32 u32ClassHash = streamIn.ReadU32();
            MIU32 u32ClassRevision = streamIn.ReadU32();
            s_pDBStream->Seek( 12 );
            *s_pDBStream << u32ClassHash << u32ClassRevision;
            s_pDBStream->Skip( 4 );
        }
        else
            streamIn.Skip( 8 );
        streamIn.Skip( 4 + 4 * 8 );
        streamIn.Skip( streamIn.ReadU32() );
        *s_pDBStream << SwapHighLow( a_Time.m_u64Modified ) << SwapHighLow( a_Time.m_u64Created ) << SwapHighLow( a_Time.m_u64Created ) << SwapHighLow( u64CompiledTime );
        *s_pDBStream << ( MIU32 )( strResourceName.GetLength() ) << strResourceName;
        s_pDBStream->Write( static_cast< MILPCByte >( streamIn.GetBuffer() ) + streamIn.Tell(), streamIn.GetSize() - streamIn.Tell() );
        streamIn.Resize( uRomSize );
        streamIn.Seek( 0 );
        streamIn << "GAR5" << ( MIU32 ) 0x20;
    }
    mCMemoryStream streamCompressed;
    MIBool bCompressed = MIFalse;
    MIU32 u32CompressedSize = streamIn.GetSize();
    if ( s_pfuncCompress && strExt != "r3img" && strExt != "r3snd" && strExt != "r3dlg" )
    {
        streamCompressed.Resize( streamIn.GetSize() );
        bCompressed = 0 == ( *s_pfuncCompress )( static_cast< MILPByte >( streamCompressed.AccessBuffer() ), &u32CompressedSize, static_cast< MILPCByte >( streamIn.GetBuffer() ), u32CompressedSize );
        if ( bCompressed && u32CompressedSize * 3 > streamIn.GetSize() * 2 )
            bCompressed = MIFalse;
    }
    *s_pEntryStream << ( MIU32 )( RisenPakAttribute_Packed | RisenPakAttribute_Archive | ( bCompressed ? RisenPakAttribute_Compressed : 0 ) );
    *s_pEntryStream << ( MIU32 )( strFileName.GetLength() ) << strFileName;
    if ( strFileName.GetLength() )
        *s_pEntryStream << ( MIU8 ) 0;
    *s_pEntryStream << ( MIU64 )( s_pDataStream->Tell() );
    if ( s_bResourceDatabase )
        *s_pEntryStream << u64CompiledTime << u64CompiledTime << u64CompiledTime;
    else
        *s_pEntryStream << a_Time.m_u64Created << a_Time.m_u64Accessed << a_Time.m_u64Modified;
    *s_pEntryStream << ( MIU32 )( RisenPakAttribute_Packed | RisenPakAttribute_Archive ) << ( MIU32 ) 0 << ( MIU32 )( bCompressed ? 1 : 0 ) << ( bCompressed ? u32CompressedSize : streamIn.GetSize() ) << streamIn.GetSize();
    if ( bCompressed )
    {
        streamCompressed.Resize( u32CompressedSize );
        *s_pDataStream << streamCompressed;
    }
    else
        *s_pDataStream << streamIn;
    if ( s_pfuncShowProgress )
        ( *s_pfuncShowProgress )( ++s_uCurrentFileCount, s_uTotalFileCount );
    return MITrue;
}

namespace
{
    mCString ReadString( mCIOStreamBinary & a_streamSource )
    {
        MIUInt uLength = a_streamSource.ReadU32();
        if ( !uLength )
            return "";
        mCString strResult = a_streamSource.ReadString( uLength );
        a_streamSource.Skip( 1 );
        return strResult;
    }
}

void mCGenomeVolume::ReadNewVersionedDirectory( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    a_strPath += ReadString( a_streamSource ) + "\\";
    SFileTime & DirTime = m_mapDirTimes[ a_strPath ];
    DirTime.m_u64Created = a_streamSource.ReadU64();
    DirTime.m_u64Accessed = a_streamSource.ReadU64();
    DirTime.m_u64Modified = a_streamSource.ReadU64();
    a_streamSource.Skip( 4 );
    for ( MIUInt u = a_streamSource.ReadU32(); u--; )
    {
        if ( a_streamSource.ReadU32() & 0x10 )
            ReadNewVersionedDirectory( a_streamSource, a_arrFilesDest, a_strPath );
        else
            ReadNewVersionedFile( a_streamSource, a_arrFilesDest, a_strPath );
    }
}

void mCGenomeVolume::ReadNewVersionedFile( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    SFile & File = a_arrFilesDest.AddNew();
    File.m_strFilePath = a_strPath + ReadString( a_streamSource );
    File.m_uDataOffset = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_u64Created = a_streamSource.ReadU64();
    File.m_u64Accessed = a_streamSource.ReadU64();
    File.m_u64Modified = a_streamSource.ReadU64();
    MIU32 u32Attributes = a_streamSource.ReadU32();
    a_streamSource.Skip( 4 );
    File.m_bCompressed = a_streamSource.ReadU32() != 0;
    File.m_uDataSize = a_streamSource.ReadU32();
    File.m_uFileSize = a_streamSource.ReadU32();
    if ( u32Attributes & 0xC000 )
        a_arrFilesDest.RemoveAt( a_arrFilesDest.GetCount() - 1 );
}

void mCGenomeVolume::ReadOldVersionedEntry( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath )
{
    SFile File;
    File.m_u64Created = a_streamSource.ReadU64();
    File.m_u64Accessed = a_streamSource.ReadU64();
    File.m_u64Modified = a_streamSource.ReadU64();
    a_streamSource.Skip( 8 );
    MIU32 u32Attributes = a_streamSource.ReadU32();
    if ( u32Attributes & 0x10 )
    {
        a_strPath = "\\" + ReadString( a_streamSource ).Replace( '/', '\\' );
        SFileTime & DirTime = m_mapDirTimes[ a_strPath ];
        DirTime.m_u64Created = File.m_u64Created;
        DirTime.m_u64Accessed = File.m_u64Accessed;
        DirTime.m_u64Modified = File.m_u64Modified;
        for ( MIUInt u = a_streamSource.ReadU32(); u--; ReadOldVersionedEntry( a_streamSource, a_arrFilesDest, a_strPath ) );
        for ( MIUInt u = a_streamSource.ReadU32(); u--; ReadOldVersionedEntry( a_streamSource, a_arrFilesDest, a_strPath ) );
        return;
    }
    File.m_uDataOffset = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_uDataSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_uFileSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    a_streamSource.Skip( 4 );
    File.m_bCompressed = ( ( a_streamSource.ReadU32() & 2 ) != 0 );
    File.m_strFilePath = "\\" + ReadString( a_streamSource ).Replace( '/', '\\' );
    ReadString( a_streamSource );
    if ( !( u32Attributes & 0xC000 ) )
        a_arrFilesDest.Add( File );
}