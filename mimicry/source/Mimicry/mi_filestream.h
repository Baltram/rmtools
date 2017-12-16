#ifndef MI_FILESTREAM_H_INCLUDED
#define MI_FILESTREAM_H_INCLUDED

enum mEFileOpenMode
{
    mEFileOpenMode_Invalid    = 0,
    mEFileOpenMode_Read       = 1 << 0,
    mEFileOpenMode_Write      = 1 << 1,
    mEFileOpenMode_ReadWrite  = 1 << 0 | 1 << 1,
    mEFileOpenMode_ForceDWORD = MI_FORCE_DWORD
};

class mCFileStream :
    public mCIOStreamBinary
{
public:
    virtual ~mCFileStream( void );
public:
    mCFileStream( void );
    mCFileStream( mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode );
public:
    virtual MIU64    GetSize64( void ) const;
    virtual MIBool   IsAtEnd( void ) const;
    virtual mEResult Read( MILPVoid a_pDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest );
    virtual mEResult Seek( MIU64 a_u64Position, mEStreamSeekMode a_enuMode = mEStreamSeekMode_Begin );
    virtual MIU64    Tell64( void ) const;
    virtual mEResult Write( MILPCVoid a_pSource, MIUInt a_uSize );
    virtual mEResult Write( mCString const & a_strSource );
public:
    void             Buffer( MIU64 a_u64Position );
    void             Close( void );
    void             Flush( void );
    mCString const & GetFileName( void );
    MIBool           IsOpen( void );
    mEResult         Open( mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode );
protected:
    virtual void DirectRead( MILPVoid a_pDest, MIU64 a_u64Position, MIUInt a_uSize );
    virtual void Init( MILPVoid a_pFile, mCString const & a_strFileName, mEFileOpenMode a_enuOpenMode );
protected:
    void Clear( void );
private:
    mCFileStream( mCFileStream const & );
private:
    mCFileStream & operator = ( mCFileStream const & );
private:
    virtual mEResult ReadFormatted( MILPVoid, MILPCChar );
    virtual mEResult WriteFormatted( MILPCVoid, MILPCChar );
protected:
    mCCharArray    m_arrBuffer;
    mCString       m_strFileName;
    mEFileOpenMode m_enuOpenMode;
    MILPVoid       m_pFile;
    MIU64          m_u64RecentFileSize;
    MIU64          m_u64BufferedPosition;
    MIUInt         m_uOffset;
    MIUInt         m_uOffsetPending;
    MIBool         m_bPendingData;
};

#endif