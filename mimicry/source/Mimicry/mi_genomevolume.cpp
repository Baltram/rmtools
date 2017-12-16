#include "mi_include_converters.h"
#include "mi_include_risen.h"
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
        "wrl",
        // New in Elex
        "vid",
        "tfn",
        "pxc",
        "ocm",
        "flm",
        "fll"
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
        "World",
        // New in Elex
        "Video",
        "TrueTypeFont",
        "PhysXCollection",
        "OcclusionMesh",
        "FlashMovie",
        "FlashLibrary"
    };
}

MIInt ( *mCGenomeVolume::s_pfuncCompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
MIInt ( *mCGenomeVolume::s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );

namespace
{
    struct SResourceDir
    {
    public:
        SResourceDir( void ) :
            m_bFirstDatabaseEntry( MITrue )
        {}
    public:
        mCString            m_strName;
        mCString            m_strFilExt;
        MIBool              m_bFirstDatabaseEntry;
        mCString            m_strLanguage;
        mCString            m_strResourceToken;
        mTArray< mCString > m_arrFiles;
        mCMemoryStream      m_streamDB;
        mCMemoryStream      m_streamCSV;
    };

    // Used during mCGenomeVolume::CreateRisen3Archive()
    mTStringMap< SResourceDir > s_mapResourceDirs;
    MIUInt                      s_uGeneration = 0;
    mCString                    s_strProblematicFilePath;
    mCIOStreamBinary          * s_pDataStream = 0;
    mCIOStreamBinary          * s_pEntryStream = 0;
    MIUInt                      s_uTotalFileCount = 0;
    MIUInt                      s_uCurrentFileCount = 0;
    void                    ( * s_pfuncShowProgress )( MIUInt, MIUInt ) = 0;

    MIU64 SwapHighLow( MIU64 a_u64Time )
    {
        return ( a_u64Time << 32 ) | ( a_u64Time >> 32 );
    }

    mCString GetRisen3ResourceType( mCString a_strResourceToken, mCString a_strLanguage )
    {
        for ( MIUInt u = sizeof( s_arrResourceTokens ) / sizeof( *s_arrResourceTokens ); u--; )
            if ( s_arrResourceTokens[ u ] == a_strResourceToken )
                return s_arrResourceTypes[ u ] + ( a_strLanguage != "na" ? "_" + a_strLanguage : "" );
        return "";
    }
}

mCGenomeVolume::CFileHandle::CFileHandle( mCGenomeVolume * a_pVolume, MIUInt a_uFileIndex, MIBool a_bResolveRisen3Entries ) :
    m_pVolume( a_pVolume ),
    m_pDir( 0 ),
    m_uFileIndex( a_uFileIndex ),
    m_bResolveRisen3Entries( a_bResolveRisen3Entries )
{
}

mCGenomeVolume::CFileHandle::CFileHandle( void ) :
    m_pVolume( 0 ),
    m_pDir( 0 ),
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
    m_pVolume->m_streamArchive.Seek( File.m_u64DataOffset );
    m_pVolume->m_streamArchive.Read( arrFileData.AccessBuffer(), File.m_uDataSize );
    if ( File.m_bCompressed )
    {
        MIU32 u32FileSize = File.m_uFileSize;
        mCByteArray arrUncompressedFileData;
        arrUncompressedFileData.Resize( u32FileSize );
        if ( !s_pfuncUncompress || ( ( *mCGenomeVolume::s_pfuncUncompress )( arrUncompressedFileData.AccessBuffer(), &u32FileSize, arrFileData.GetBuffer(), arrFileData.GetCount() ) != 0 ) )
            return MIFalse;
        arrUncompressedFileData.Swap( arrFileData );
    }
    a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
    if ( m_bResolveRisen3Entries && m_pDir )
    {
        MIU32 u32Offset = a_streamFileDataDest.Tell();
        arrFileData.Resize( File.m_uDatabaseEntrySize );
        m_pDir->m_streamResourceDatabase.Seek( File.m_uDatabaseOffset );
        m_pDir->m_streamResourceDatabase.Read( arrFileData.AccessBuffer(), File.m_uDatabaseEntrySize );
        a_streamFileDataDest << m_pDir->m_u32ResourceClassHash << m_pDir->m_u32ResourceClassRevision << ( MIU32 )( File.m_uDatabaseEntrySize );
        a_streamFileDataDest.Write( arrFileData.GetBuffer(), arrFileData.GetCount() );
        a_streamFileDataDest.Seek( 0 );
        a_streamFileDataDest << ( mCRisenName::GetGame() == mEGame_Elex ? "E1RF" : "R3RF" ) << u32Offset;
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
    if ( m_bResolveRisen3Entries && m_pDir )
    {
        mCString strDirPath = m_pDir->GetName() == "" ? "\\" : "\\" + m_pDir->GetName() + "\\";
        return strDirPath + m_pVolume->m_arrFiles[ m_uFileIndex ].m_strResourceName + ( mCRisenName::GetGame() == mEGame_Elex ? ".elex" : ".r3" ) + m_pDir->m_strResourceToken;
    }
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

MIUInt mCGenomeVolume::CFileHandle::GetSize( void ) const
{
    return m_pVolume->m_arrFiles[ m_uFileIndex ].m_uFileSize;
}
mCGenomeVolume::CDir::CDir( void ) :
    m_u32ResourceClassRevision( 0 ),
    m_u32ResourceClassHash( 0 )
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

MIUInt mCGenomeVolume::CDir::GetTotalFileCount( void ) const
{
    MIUInt uResult = m_arrFiles.GetCount();
    for ( MIUInt u = m_arrSubDirs.GetCount(); u--; uResult += m_arrSubDirs[ u ].GetTotalFileCount() );
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

mCString mCGenomeVolume::CDir::GetRisen3ResourceType( void ) const
{
    return ::GetRisen3ResourceType( m_strResourceToken, m_strLanguage );
}

mTArray< mCGenomeVolume::CDir > const & mCGenomeVolume::CDir::GetSubDirs( void ) const
{
    return m_arrSubDirs;
}

MIU64 mCGenomeVolume::CDir::GetTotalSize( void ) const
{
    MIU64 u64Result = 0;
    for ( MIUInt u = m_arrFiles.GetCount(); u--; u64Result += m_arrFiles[ u ].GetSize() );
    for ( MIUInt u = m_arrSubDirs.GetCount(); u--; u64Result += m_arrSubDirs[ u ].GetTotalSize() );
    return u64Result;
}

void mCGenomeVolume::CDir::ResolveRisen3Entries( void )
{
    mCGenomeVolume::CFileHandle * pFileDB = 0;
    for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
    {
        mCString const strFilePath = m_arrFiles[ u ].GetFilePath();
        if ( g_GetFileName( strFilePath ).Count( '_' ) == 3 && g_GetFileExt( strFilePath ).ToLower() == "db" )
            pFileDB = &m_arrFiles[ u ];
    }
    if ( !pFileDB || m_arrSubDirs.GetCount() != 1 )
    {
        if ( m_strName == "" )  // root
            for ( MIUInt u = 0, ue = m_arrSubDirs.GetCount(); u != ue; ++u )
                m_arrSubDirs[ u ].ResolveRisen3Entries();
        return;
    }
    m_strRisen3Name = g_GetFileNameNoExt( pFileDB->GetFilePath() );
    mCString const strFileNameDB = g_GetFileNameNoExt( pFileDB->GetFilePath() );
    if ( !pFileDB->GetFileData( m_streamResourceDatabase ) ||
         strFileNameDB.Scan( "%*c_%3c_%*c_%s", mCString::AccessStaticBuffer(), mCString::AccessStaticBuffer() + 3 ) != 2 )
         return;
    pFileDB = 0;
    m_streamResourceDatabase.Seek( 0 );
    if ( m_streamResourceDatabase.ReadString( 4 ) != "GAR5" )
        return;
    m_strResourceToken.SetText( mCString::AccessStaticBuffer(), 3 );
    m_strResourceToken.ToLower();
    m_strLanguage.SetText( mCString::AccessStaticBuffer() + 3 );
    m_strLanguage.ToLower();
    mTStringMap< CFileHandle > mapFiles;
    for ( MIUInt u = 0, ue = m_arrSubDirs[ 0 ].m_arrSubDirs.GetCount(); u != ue; ++u )
    {
        for ( MIUInt v = 0, ve = m_arrSubDirs[ 0 ].m_arrSubDirs[ u ].m_arrFiles.GetCount(); v != ve; ++v )
        {
            CFileHandle & fileResource = m_arrSubDirs[ 0 ].m_arrSubDirs[ u ].m_arrFiles[ v ];
            mapFiles[ g_GetFileNameNoExt( fileResource.GetFilePath() ) ] = fileResource;
        }
    }
    m_arrSubDirs.Clear();
    m_streamResourceDatabase.Skip( 8 );
    m_u32ResourceClassHash = m_streamResourceDatabase.ReadU32();
    m_u32ResourceClassRevision = m_streamResourceDatabase.ReadU32();
    MIUInt uResourceCount = m_streamResourceDatabase.ReadU32();
    m_arrFiles.Clear();
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
        CFileHandle fileResource;
        if ( !mapFiles.GetAt( ( m_strRisen3Name + mCString().Format( "_%.8x", g_djb2( strResourceName.Lower().GetText() ) ) ).ToLower(), fileResource ) )
            continue;
        fileResource.m_pDir = this;
        m_arrFiles.Add( fileResource );
        fileResource.m_pVolume->m_mapFileIndices[ strResourceName.Lower() ] = fileResource.m_uFileIndex;
        SFile & File = fileResource.m_pVolume->m_arrFiles[ fileResource.m_uFileIndex ];
        File.m_strResourceName = strResourceName;
        if ( u64RawTime != 0 )
            File.m_u64Created = File.m_u64Modified = u64RawTime;
        File.m_uDatabaseOffset = uOffset;
        File.m_uDatabaseEntrySize = m_streamResourceDatabase.Tell() - uOffset;
    }
}

mCGenomeVolume::mCGenomeVolume( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries ) :
    m_bResolveRisen3Entries( a_bResolveRisen3Entries ),
    m_pRoot( 0 )
{
    Open( a_strFilePath, a_bResolveRisen3Entries );
}

mCGenomeVolume::mCGenomeVolume( void ) :
    m_bResolveRisen3Entries( MIFalse ),
    m_pRoot( 0 )
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
        if ( a_Dir.GetCreationTime() > a_Time.m_u64Created )
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
        mCString const strDir = g_GetDirectoryPath( a_strFilePath ).ToLower();
        mCString const strDirArchive = g_GetDirectoryPath( a_strFilePath ).TrimLeft( a_strRootPath.GetLength() );
        if ( !s_mapResourceDirs.Contains( strDir ) )
            return strDirArchive;
        MIU32 u32Hash = g_djb2( g_GetFileNameNoExt( a_strFilePath ).ToLower().GetText() );
        return strDirArchive + "\\0\\" + mCString().Format( "%.8x", u32Hash ).Left( 1 );
    }
}

mEResult mCGenomeVolume::CreateRisen3Archive( mCString a_strRootPath, mTArray< mCString > const & a_arrFilePaths, mTArray< SFileTime > const & a_arrFileTimes, mCIOStreamBinary & a_streamDest, MIUInt ( *a_pfuncRequestGeneration )( void ), mCString * a_pVolumeName, void ( * a_pfuncShowProgress )( MIUInt, MIUInt ) )
{
    s_uTotalFileCount = a_arrFilePaths.GetCount();
    s_uCurrentFileCount = 0;
    s_pfuncShowProgress = a_pfuncShowProgress;
    s_mapResourceDirs.Clear();
    s_uGeneration = 0;
    s_strProblematicFilePath = "";
    CDir Root;
    a_strRootPath.TrimRight( "\\/" );
    MIBool bMixedFileExt = MIFalse;
    for ( MIUInt u = a_arrFilePaths.GetCount(); u--; )
    {
        mCString const strExt = g_GetFileExt( a_arrFilePaths[ u ] ).ToLower();
        mCString const strDir = g_GetDirectoryPath( a_arrFilePaths[ u ] ).ToLower();
        if ( !s_mapResourceDirs.Contains( strDir ) )
        {
            MIBool bRisen3Resource = MIFalse;
            if ( strExt.StartsWith( mCRisenName::GetGame() == mEGame_Elex ? "elex" : "r3" ) )
                for ( MIUInt v = sizeof( s_arrResourceTokens ) / sizeof( *s_arrResourceTokens ); v--; )
                    if ( strExt == mCString( ( mCRisenName::GetGame() == mEGame_Elex ? "elex" : "r3" ) ) + s_arrResourceTokens[ v ] )
                        bRisen3Resource = MITrue;
            if ( !bRisen3Resource )
                continue;
            SResourceDir & ResourceDir = s_mapResourceDirs[ strDir ];
            ResourceDir.m_strFilExt = strExt;
            ResourceDir.m_strName = g_GetFileName( strDir );
        }
        SResourceDir & ResourceDir = s_mapResourceDirs[ strDir ];
        if ( strExt != ResourceDir.m_strFilExt )
            bMixedFileExt = MITrue;
        ResourceDir.m_arrFiles.Add( a_arrFilePaths[ u ] );
    }
    if ( bMixedFileExt )
        return MI_ERROR( mCGenomeArchiveError, EMixedRisen3ResourceTypes, "Failed to create PAK archive because one of the resource folders contains mixed resource types." ), mEResult_False;
    Root.AccessName() = g_GetFileName( a_strRootPath );
    if ( s_mapResourceDirs.GetCount() || mCRisenName::GetGame() == mEGame_Elex )
    {
        s_uGeneration = ( *a_pfuncRequestGeneration )();
        mCString strName = Root.GetName();
        if ( strName.GetLength() >= 4 && strName[ 0 ] == 'm' && strName[ 1 ] == '_' && isdigit( strName[ 2 ] ) && strName[ 3 ] == '_' )
            strName.TrimLeft( ( MIUInt )( 4 ) );
        Root.AccessName().Format( "m_%u_%s", s_uGeneration, strName.GetText() );
    }
    for ( MIUInt u = a_arrFilePaths.GetCount(); u--; )
    {
        CDir & Dir = GetDir( Root, GetArchiveDirPath( a_arrFilePaths[ u ], a_strRootPath ) );
        Dir.AccessFiles().Add( CFileHandle( 0, u ) );
        UpdateDirTime( Dir, a_arrFileTimes[ u ] );
    }
    ReviseDirTime( Root );
    for ( mTStringMap< SResourceDir >::CIterator i = s_mapResourceDirs.Begin(), ie = s_mapResourceDirs.End(); i != ie; ++i )
    {
        s_uTotalFileCount += 2;
        i->m_strLanguage = "na";
        if ( i->m_strName.Scan( "%*[^_]_%99s", mCString::AccessStaticBuffer() ) == 1 )
            i->m_strLanguage.SetText( mCString::AccessStaticBuffer() );
        if ( i->m_strLanguage.Scan( "%*[^_]_%99s", mCString::AccessStaticBuffer() ) == 1 )  // Backward compatibility
            i->m_strLanguage.SetText( mCString::AccessStaticBuffer() );
        i->m_strResourceToken = i->m_strFilExt.Right( 3 ).ToLower();
        i->m_streamCSV << "Hash|Name|Raw File Time|Compiled File Time|\r\n";
        i->m_streamDB << "GAR5" << ( MIU32 ) 0x20 << "RT02" << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 )( i->m_arrFiles.GetCount() );
        CDir & Dir = GetDir( Root, g_GetDirectoryPath( g_GetDirectoryPath( GetArchiveDirPath( i->m_arrFiles.Front(), a_strRootPath ) ) ) );
        Dir.AccessName().Format( "%u_%s_%s", s_uGeneration, i->m_strLanguage.GetText(), i->m_strResourceToken.GetText() );
        Dir.m_strResourceDir = i.GetKey();
    }
    mCMemoryStream streamEntryTable;
    s_pEntryStream = &streamEntryTable;
    s_pDataStream = &a_streamDest;
    a_streamDest << ( MIU32 ) 1 << "G3V0" << ( MIU32 ) 0 << ( MIU32 ) 0 << ( MIU32 ) 1 << ( MIU32 ) 0xFEEDFACE << ( MIU64 ) 48 << ( MIU64 ) 0 << ( MIU64 ) 0;  // Header
    if ( a_pVolumeName )
        *a_pVolumeName = Root.GetName() + ".pak";
    Root.AccessName().Clear();
    if ( !WritePakDir( Root, a_arrFilePaths, a_arrFileTimes ) )
        return MI_ERROR( mCGenomeArchiveError, ESourceFileOpenError, mCString().Format( "Failed to create PAK volume. Error when processing %s", s_strProblematicFilePath.GetText() ).GetText() ), mEResult_False;
    MIU64 const u64RootOffset = a_streamDest.Tell64();
    a_streamDest << streamEntryTable;
    MIU64 const u64VolumeSize = a_streamDest.Tell64();
    a_streamDest.Seek( 32 );
    a_streamDest << u64RootOffset << u64VolumeSize;
    a_streamDest.Seek( u64VolumeSize );
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
    delete m_pRoot;
    m_pRoot = 0;
}

mCGenomeVolume::CDir const & mCGenomeVolume::GetRoot( void )
{
    if ( m_pRoot )
        return *m_pRoot;
    CDir & Root = *( m_pRoot = new CDir );
    for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
        GetDir( Root, g_GetDirectoryPath( m_arrFiles[ u ].m_strFilePath ), &m_mapDirTimes ).AccessFiles().Add( CFileHandle( this, u, m_bResolveRisen3Entries ) );
    Root.AccessCreationTime() = Root.AccessLastAccessTime() = Root.AccessLastWriteTime() = 0;
    if ( m_bResolveRisen3Entries )
        Root.ResolveRisen3Entries();
    return Root;
}

MIBool mCGenomeVolume::FindFile( mCString a_strFileBaseName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest )
{
    MIUInt uFile;
    if ( !m_mapFileIndices.GetAt( a_strFileBaseName.ToLower(), uFile ) || ( !s_pfuncUncompress && m_arrFiles[ uFile ].m_bCompressed ) )
        return MIFalse;
    CFileHandle Handle( this, uFile );
    a_strFilePathDest = Handle.GetFilePath();
    a_streamFileDataDest.Clear();
    Handle.GetFileData( a_streamFileDataDest );
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
    MIU64 const u64RootOffset = m_streamArchive.ReadU64();
    m_streamArchive.Seek( u64RootOffset );
    if ( uVersion == 0 )
        ReadOldVersionedEntry( m_streamArchive, m_arrFiles, "" );
    else
        ReadNewVersionedDirectory( m_streamArchive, m_arrFiles, "" );
    m_bResolveRisen3Entries = a_bResolveRisen3Entries;
    GetRoot();
    m_mapFileIndices.Reserve( m_arrFiles.GetCount() );
    for ( MIUInt u = 0, ue = m_arrFiles.GetCount(); u != ue; ++u )
    {
        mCString const & strFilePath = m_arrFiles[ u ].m_strFilePath;
        m_mapFileIndices[ g_GetFileNameNoExt( strFilePath ).ToLower() ] = u;
        m_mapFileIndices[ g_GetFileName( strFilePath ).ToLower() ] = u;
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
    *s_pEntryStream << ( MIU32 )( RisenPakAttribute_Packed | RisenPakAttribute_Directory ) << ( MIU32 )( a_Dir.GetFiles().GetCount() + a_Dir.GetSubDirs().GetCount() + ( a_Dir.m_strResourceDir != "" ? 2 : 0 ) );
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
    if ( a_Dir.m_strResourceDir != "" )
    {
        mCString const strDirPath = g_GetDirectoryPath( s_mapResourceDirs[ a_Dir.m_strResourceDir ].m_arrFiles.Front() );
        if ( !WritePakFile( strDirPath + "\\<db>", a_Dir ) ||
             !WritePakFile( strDirPath + "\\<csv>", a_Dir ) )
             return MIFalse;
    }
    return MITrue;
}

MIBool mCGenomeVolume::WritePakFile( mCString const & a_strFilePath, mCGenomeVolume::SFileTime const & a_Time )
{
    MIU64 u64CompiledTime = g_time();
    mCString strExt = g_GetFileExt( a_strFilePath ), strFileName = g_GetFileName( a_strFilePath ), strResourceName = g_GetFileNameNoExt( a_strFilePath );
    mCMemoryStream streamIn;
    SResourceDir * pResourceDir = 0;
    if ( s_mapResourceDirs.Contains( g_GetDirectoryPath( a_strFilePath ).ToLower() ) )
        pResourceDir = &s_mapResourceDirs[ g_GetDirectoryPath( a_strFilePath ).ToLower() ];
    if ( pResourceDir && g_GetFileNameNoExt( a_strFilePath ) == "<db>" )
    {
        streamIn.Swap( pResourceDir->m_streamDB );
        strFileName.Format( "w_%s_%u_%s.db", pResourceDir->m_strResourceToken.GetText(), s_uGeneration, pResourceDir->m_strLanguage.GetText() );
    }
    else if ( pResourceDir && g_GetFileNameNoExt( a_strFilePath ) == "<csv>" )
    {
        streamIn.Swap( pResourceDir->m_streamCSV );
        strFileName.Format( "w_%s_%u_%s.csv", pResourceDir->m_strResourceToken.GetText(), s_uGeneration, pResourceDir->m_strLanguage.GetText() );
    }
    else if ( streamIn.FromFile( a_strFilePath ) != mEResult_Ok )
        return s_strProblematicFilePath = a_strFilePath, MIFalse;
    if ( pResourceDir && !a_strFilePath.EndsWith( "\\<db>" ) && !a_strFilePath.EndsWith( "\\<csv>" ) )
    {
        MIU32 u32Hash = g_djb2( mCString( strResourceName ).ToLower().GetText() );
        strFileName.Format( "w_%s_%u_%s_%.8x.rom", pResourceDir->m_strResourceToken.GetText(), s_uGeneration, pResourceDir->m_strLanguage.GetText(), u32Hash );
        time_t TimeRaw = static_cast< time_t >( g_filetimeToCtime( a_Time.m_u64Modified ) ), TimeCompiled = static_cast< time_t >( g_filetimeToCtime( u64CompiledTime ) );
        MI_CRT_NO_WARNINGS( struct tm * pTimeInfoRaw = localtime( &TimeRaw ); )
        strftime( mCString::AccessStaticBuffer(), 80, "%d.%m.%Y - %H:%M:%S", pTimeInfoRaw );
        mCString strTimeRaw( mCString::AccessStaticBuffer() );
        MI_CRT_NO_WARNINGS( struct tm * pTimeInfoCompiled = localtime( &TimeCompiled ); )
        strftime( mCString::AccessStaticBuffer(), 80, "%d.%m.%Y - %H:%M:%S", pTimeInfoCompiled );
        mCString strTimeCompiled( mCString::AccessStaticBuffer() );
        pResourceDir->m_streamCSV << mCString().Format( "%.8x|%s|%s|%s|\r\n", u32Hash, strResourceName.GetText(), strTimeRaw.GetText(), strTimeCompiled.GetText() );
        if ( streamIn.ReadString( 4 ) != ( mCRisenName::GetGame() == mEGame_Elex ? "E1RF" : "R3RF" ) )
            return s_strProblematicFilePath = a_strFilePath, MIFalse;
        MIUInt uRomSize = streamIn.ReadU32();
        streamIn.Seek( uRomSize );
        if ( pResourceDir->m_bFirstDatabaseEntry )
        {
            pResourceDir->m_bFirstDatabaseEntry = MIFalse;
            MIU32 u32ClassHash = streamIn.ReadU32();
            MIU32 u32ClassRevision = streamIn.ReadU32();
            pResourceDir->m_streamDB.Seek( 12 );
            pResourceDir->m_streamDB << u32ClassHash << u32ClassRevision;
            pResourceDir->m_streamDB.Skip( 4 );
        }
        else
            streamIn.Skip( 8 );
        streamIn.Skip( 4 + 4 * 8 );
        streamIn.Skip( streamIn.ReadU32() );
        pResourceDir->m_streamDB << SwapHighLow( a_Time.m_u64Modified ) << SwapHighLow( a_Time.m_u64Created ) << SwapHighLow( a_Time.m_u64Created ) << SwapHighLow( u64CompiledTime );
        pResourceDir->m_streamDB << ( MIU32 )( strResourceName.GetLength() ) << strResourceName;
        pResourceDir->m_streamDB.Write( static_cast< MILPCByte >( streamIn.GetBuffer() ) + streamIn.Tell(), streamIn.GetSize() - streamIn.Tell() );
        streamIn.Resize( uRomSize );
        streamIn.Seek( 0 );
        streamIn << "GAR5" << ( MIU32 ) 0x20;
    }
    mCMemoryStream streamCompressed;
    MIBool bCompressed = MIFalse;
    MIU32 u32CompressedSize = streamIn.GetSize();
    if ( s_pfuncCompress && strExt != "r3img" && strExt != "r3snd" && strExt != "r3dlg" && strExt != "eleximg" && strExt != "elexsnd" && strExt != "elexdlg" )
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
    *s_pEntryStream << ( MIU64 )( s_pDataStream->Tell64() );
    if ( pResourceDir )
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
    File.m_u64DataOffset = a_streamSource.ReadU64();
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
    File.m_u64DataOffset = a_streamSource.ReadU64();
    File.m_uDataSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    File.m_uFileSize = static_cast< MIUInt >( a_streamSource.ReadU64() );
    a_streamSource.Skip( 4 );
    File.m_bCompressed = ( ( a_streamSource.ReadU32() & 2 ) != 0 );
    File.m_strFilePath = "\\" + ReadString( a_streamSource ).Replace( '/', '\\' );
    ReadString( a_streamSource );
    if ( !( u32Attributes & 0xC000 ) )
        a_arrFilesDest.Add( File );
}