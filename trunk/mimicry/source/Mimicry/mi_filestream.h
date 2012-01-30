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
    public mCIStreamBinary,
    public mCOStreamBinary,
    public mCStreamSeekable
{
public:
    virtual ~mCFileStream( void );
public:
    mCFileStream( void );
    mCFileStream( mCString a_strFileName, mEFileOpenMode a_enuOpenMode );
public:
    virtual MIUInt   GetSize( void ) const;
    virtual mEResult Read( MILPVoid a_pDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest, MIUInt a_uSize );
    virtual mEResult Read( mCString & a_strDest );
    virtual mEResult Seek( MIUInt a_uPosition, mEStreamSeekMode a_enuMode = mEStreamSeekMode_Begin );
    virtual MIUInt   Tell( void ) const;
    virtual mEResult Write( MILPCVoid a_pSource, MIUInt a_uSize );
    virtual mEResult Write( mCString const & a_strSource );
public:
    void     Buffer( MIUInt a_uPosition );
    void     Close( void );
    void     Flush( void );
    mCString GetFileName( void );
    MIBool   IsOpen( void );
    mEResult Open( mCString a_strFileName, mEFileOpenMode a_enuOpenMode );
protected:
    virtual void DirectRead( MILPVoid a_pDest, MIUInt a_uPosition, MIUInt a_uSize );
    virtual void Init( MILPVoid a_pFile, mCString a_strFileName, mEFileOpenMode a_enuOpenMode );
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
    MIUInt         m_uRecentFileSize;
    MIUInt         m_uBufferedPosition;
    MIUInt         m_uOffset;
    MIUInt         m_uOffsetPending;
    MIBool         m_bPendingData;
};

#endif