#ifndef MI_GENOMEVOLUME_H_INCLUDED
#define MI_GENOMEVOLUME_H_INCLUDED

MI_ERROR_DEFINE_DERIVED( mCGenomeArchiveError,
                         mCError,
                         EMixedRisen3ResourceTypes,
                         ESourceFileOpenError )

class mCGenomeVolume
{
public:
    struct SFileTime
    {
        MIU64    m_u64Accessed;
        MIU64    m_u64Created;
        MIU64    m_u64Modified;
    };
    class CFileHandle
    {
    public:
        CFileHandle( mCGenomeVolume * a_pVolume, MIUInt a_uFileIndex, MIBool a_bResolveRisen3Entries = MIFalse );
        CFileHandle( void );
    public:
        MIU64    GetCreationTime( void ) const;
        MIBool   GetFileData( mCIOStreamBinary & a_streamFileDataDest ) const;
        MIUInt   GetFileIndex( void ) const;
        mCString GetFilePath( void ) const;
        MIU64    GetLastAccessTime( void ) const;
        MIU64    GetLastWriteTime( void ) const;
    private:
        mCGenomeVolume * m_pVolume;
        MIUInt           m_uFileIndex;
        MIBool           m_bResolveRisen3Entries;
    };
    class CDir :
        public SFileTime
    {
    public:
        CDir( void );
    public:
        MIU64 &                        AccessCreationTime( void );
        mTArray< CFileHandle > &       AccessFiles( void );
        MIU64 &                        AccessLastAccessTime( void );
        MIU64 &                        AccessLastWriteTime( void );
        mCString &                     AccessName( void );
        mTArray< CDir >  &             AccessSubDirs( void );
        MIU64                          GetCreationTime( void ) const;
        MIUInt                         GetFileCount( void ) const;
        mTArray< CFileHandle > const & GetFiles( void ) const;
        MIU64                          GetLastAccessTime( void ) const;
        MIU64                          GetLastWriteTime( void ) const;
        mCString const &               GetName( void ) const;
        mTArray< CDir > const &        GetSubDirs( void ) const;
    private:
        mCString               m_strName;
        mTArray< CFileHandle > m_arrFiles;
        mTArray< CDir >        m_arrSubDirs;
    };
public:
    explicit mCGenomeVolume( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries = MIFalse );
             mCGenomeVolume( void );
            ~mCGenomeVolume( void );
public:
    static mEResult CreateRisen3Archive( mCString a_strRootPath, mTArray< mCString > const & a_arrFilePaths, mTArray< SFileTime > const & a_arrFileTimes, mCIOStreamBinary & a_streamDest, MIUInt ( * a_pfuncRequestGeneration )( void ), mCString * a_pVolumeName = 0, void ( * a_pfuncShowProgress )( MIUInt, MIUInt ) = 0 );
    static void     RegisterZlibCompressFunction( MIInt ( * a_pfuncCompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) );
    static void     RegisterZlibUncompressFunction( MIInt( *a_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) );
public:
    void         Close( void );
    mCString     GetRisen3ResourceType( void );
    CDir const & GetRoot( void );
    MIBool       FindFile( mCString a_strFileName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest );
    MIBool       IsOpen( void );
    mEResult     Open( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries = MIFalse );
private:
    struct SFile :
        public SFileTime
    {
        mCString m_strFilePath;
        mCString m_strResourceName;     // Risen 3 only
        MIUInt   m_uDatabaseOffset;     // Risen 3 only
        MIUInt   m_uDatabaseEntrySize;  // Risen 3 only
        MIUInt   m_uDataOffset;
        MIUInt   m_uDataSize;
        MIUInt   m_uFileSize;
        MIBool   m_bCompressed;
    };
private:
    static CDir & GetDir( CDir & a_Root, mCString const & a_strDirPath, mTStringMap< SFileTime > const * a_pDirTimes = 0 );
    static MIBool WritePakDir( mCGenomeVolume::CDir const & a_Dir, mTArray< mCString > const & a_arrFilePaths, mTArray< mCGenomeVolume::SFileTime > const & a_arrFileTimes );
    static MIBool WritePakFile( mCString const & a_strFilePath, mCGenomeVolume::SFileTime const & a_Time );
private:
    void ReadNewVersionedDirectory( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    void ReadNewVersionedFile( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    void ReadOldVersionedEntry( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
private:
    static MIInt ( * s_pfuncCompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
    static MIInt ( * s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
private:
    mTArray< SFile >        m_arrFiles;
    mTStringMap< MIUInt >   m_mapFileIndices;
    mTStringMap< SFileTime > m_mapDirTimes;
    mCFileStream            m_streamArchive;
    CDir *                  m_pRoot;
    mCString                m_strRisen3Name;             // Risen 3 only
    mCString                m_strResourceToken;          // Risen 3 only
    mCString                m_strLanguage;               // Risen 3 only
    MIU32                   m_u32ResourceClassRevision;  // Risen 3 only
    MIU32                   m_u32ResourceClassHash;      // Risen 3 only
    mCMemoryStream          m_streamResourceDatabase;    // Risen 3 only
};

#endif
