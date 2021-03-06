#ifndef MI_MEMORYSTREAM_H_INCLUDED
#define MI_MEMORYSTREAM_H_INCLUDED

class mCMemoryStream :
    public mCMemoryStreamBaseBinary
{
public:
    virtual ~mCMemoryStream( void );
public:
    mCMemoryStream( void );
    mCMemoryStream( mCMemoryStream const & a_Source );
    mCMemoryStream( mCIOStreamBinary & a_Source );
public:
    mCMemoryStream & operator = ( mCMemoryStream const & a_Source );
public:
    virtual MIBool                  IsAtEnd( void ) const;
    using mCMemoryStreamBaseBinary::Read;
    virtual mEResult                Read( mCString & a_strDest );
private:
    virtual mEResult ReadFormatted( MILPVoid, MILPCChar );
    virtual mEResult WriteFormatted( MILPCVoid, MILPCChar );
};

#endif