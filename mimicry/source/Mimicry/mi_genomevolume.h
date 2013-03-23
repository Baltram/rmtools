#ifndef MI_GENOMEVOLUME_H_INCLUDED
#define MI_GENOMEVOLUME_H_INCLUDED

class mCGenomeVolume
{
public:
    explicit mCGenomeVolume( mCString const & a_strFilePath );
             mCGenomeVolume( void );
public:
    static void RegisterZlibUncompressFunction( MIInt ( * a_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 ) );
public:
    void     Close( void );
    MIBool   FindFile( mCString a_strFileBaseName, mCString & a_strFilePathDest, mCMemoryStream & a_streamFileDataDest );
    MIBool   IsOpen( void );
    mEResult Open( mCString const & a_strFilePath );
private:
    struct SFile
    {
        mCString m_strFilePath;
        MIUInt   m_uDataOffset;
        MIUInt   m_uDataSize;
        MIUInt   m_uFileSize;
        MIBool   m_bCompressed;
    };
private:
    static void ReadNewVersionedDirectory( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    static void ReadNewVersionedFile( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
    static void ReadOldVersionedEntry( mCIOStreamBinary & a_streamSource, mTArray< SFile > & a_arrFilesDest, mCString a_strPath );
private:
    static MIInt ( * s_pfuncUncompress )( MILPByte, MIU32 *, MILPCByte, MIU32 );
private:
    mTStringMap< SFile > m_mapFiles;
    mCFileStream         m_streamArchive;
};

#endif
