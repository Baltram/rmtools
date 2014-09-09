#include "mi_include_converters.h"

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

MIInt( *mCGenomeVolume::s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );

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
    m_strRisen3Name = m_strResourceToken = m_strResourceSuffix = "";
    m_u32ResourceClassRevision = m_u32ResourceClassHash = 0;
    m_streamResourceDatabase.Clear();
}

mCString mCGenomeVolume::GetRisen3ResourceType( void )
{
    for ( MIUInt u = sizeof( s_arrResourceTokens ) / sizeof( *s_arrResourceTokens ); u--; )
        if ( s_arrResourceTokens[ u ] == m_strResourceToken )
            return s_arrResourceTypes[ u ] + ( "_" + m_strResourceSuffix );
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
            GetDir( Root, g_GetDirectoryPath( m_arrFiles[ u ].m_strFilePath ) ).AccessFiles().Add( CFileHandle( this, u ) );
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
         strFileNameCSV.Scan( "%*c_%3c_%c_%2c", mCString::AccessStaticBuffer(), mCString::AccessStaticBuffer() + 3, mCString::AccessStaticBuffer() + 5 ) != 3 )
        return Close(), mEResult_False;
    m_strResourceToken.SetText( mCString::AccessStaticBuffer(), 3 );
    m_strResourceToken.ToLower();
    mCString::AccessStaticBuffer()[ 4 ] = '_';
    m_strResourceSuffix.SetText( mCString::AccessStaticBuffer() + 3, 4 );
    m_strResourceSuffix.ToLower();
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

mCGenomeVolume::CDir & mCGenomeVolume::GetDir( mCGenomeVolume::CDir & a_Root, mCString const & a_strDirPath )
{
    if ( a_strDirPath == "" )
        return a_Root;
    mCString const strDirName = g_GetFileName( a_strDirPath );
    mCGenomeVolume::CDir & ParentDir = GetDir( a_Root, g_GetDirectoryPath( a_strDirPath ) );
    for ( MIUInt u = 0, ue = ParentDir.GetSubDirs().GetCount(); u != ue; ++u )
        if ( ParentDir.GetSubDirs()[ u ].GetName() == strDirName )
            return ParentDir.AccessSubDirs()[ u ];
    ParentDir.AccessSubDirs().AddNew().AccessName() = strDirName;
    CDir & Result = ParentDir.AccessSubDirs().Back();
    SDirTime DirTime;
    if ( m_mapDirTimes.GetAt( a_strDirPath + "\\", DirTime ) )
    {
        Result.AccessCreationTime() = DirTime.m_u64Created;
        Result.AccessLastAccessTime() = DirTime.m_u64Accessed;
        Result.AccessLastWriteTime() = DirTime.m_u64Modified;
    }
    else
    {
        Result.AccessCreationTime() = 0;
        Result.AccessLastAccessTime() = 0;
        Result.AccessLastWriteTime() = 0;
    }
    return Result;
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
    SDirTime & DirTime = m_mapDirTimes[ a_strPath ];
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
        SDirTime & DirTime = m_mapDirTimes[ a_strPath ];
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