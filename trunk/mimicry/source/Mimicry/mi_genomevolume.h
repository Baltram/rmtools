#ifndef MI_GENOMEVOLUME_H_INCLUDED
#define MI_GENOMEVOLUME_H_INCLUDED

class mCGenomeVolume
{
public:
    class CFileHandle
    {
    public:
        CFileHandle( mCGenomeVolume * a_pVolume, MIUInt a_uFileIndex, MIBool a_bResolveRisen3Entries = MIFalse );
        CFileHandle( void );
    public:
        MIU64    GetCreationTime( void ) const;
        MIBool   GetFileData( mCIOStreamBinary & a_streamFileDataDest ) const;
        mCString GetFilePath( void ) const;
        MIU64    GetLastAccessTime( void ) const;
        MIU64    GetLastWriteTime( void ) const;
    private:
        mCGenomeVolume * m_pVolume;
        MIUInt           m_uFileIndex;
        MIBool           m_bResolveRisen3Entries;
    };
    class CDir
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
        MIU64                  m_u64Accessed;
        MIU64                  m_u64Created;
        MIU64                  m_u64Modified;
    };
public:
    explicit mCGenomeVolume( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries = MIFalse );
             mCGenomeVolume( void );
            ~mCGenomeVolume( void );
public:
    static void RegisterZlibUncompressFunction( MIInt ( * a_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) );
public:
    void         Close( void );
    mCString     GetRisen3ResourceType( void );
    CDir const & GetRoot( void );
    MIBool       FindFile( mCString a_strFileName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest );
    MIBool       IsOpen( void );
    mEResult     Open( mCString const & a_strFilePath, MIBool a_bResolveRisen3Entries = MIFalse );
private:
    struct SFile
    {
        mCString m_strFilePath;
        mCString m_strResourceName;     // Risen 3 only
        MIUInt   m_uDatabaseOffset;     // Risen 3 only
        MIUInt   m_uDatabaseEntrySize;  // Risen 3 only
        MIUInt   m_uDataOffset;
        MIUInt   m_uDataSize;
        MIUInt   m_uFileSize;
        MIBool   m_bCompressed;
        MIU64    m_u64Accessed;
        MIU64    m_u64Created;
        MIU64    m_u64Modified;
    };
    struct SDirTime
    {
        MIU64    m_u64Accessed;
        MIU64    m_u64Created;
        MIU64    m_u64Modified;
    };
private:
    CDir & GetDir( CDir & a_Root, mCString const & a_strDirPath );
    void ReadNewVersionedDirectory( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    void ReadNewVersionedFile( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    void ReadOldVersionedEntry( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
private:
    static MIInt ( * s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
private:
    mTArray< SFile >        m_arrFiles;
    mTStringMap< MIUInt >   m_mapFileIndices;
    mTStringMap< SDirTime > m_mapDirTimes;
    mCFileStream            m_streamArchive;
    CDir *                  m_pRoot;
    mCString                m_strRisen3Name;             // Risen 3 only
    mCString                m_strResourceToken;          // Risen 3 only
    mCString                m_strResourceSuffix;         // Risen 3 only
    MIU32                   m_u32ResourceClassRevision;  // Risen 3 only
    MIU32                   m_u32ResourceClassHash;      // Risen 3 only
    mCMemoryStream          m_streamResourceDatabase;    // Risen 3 only
};

#endif
