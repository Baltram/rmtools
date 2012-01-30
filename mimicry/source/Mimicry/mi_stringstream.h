#ifndef MI_STRINGSTREAM_H_INCLUDED
#define MI_STRINGSTREAM_H_INCLUDED

class mCStringStream :
    public mCMemoryStreamBaseFormatted
{
public:
    virtual ~mCStringStream( void );
public:
    mCStringStream( void );
    mCStringStream( mCStringStream const & a_Source );
    mCStringStream( mCIOStreamFormatted & a_Source );
    mCStringStream( mCString const & a_strSource );
public:
    mCStringStream & operator = ( mCStringStream const & a_Source );
public:
    using mCMemoryStreamBaseFormatted::Read;
    virtual mEResult                   Read( mCString & a_strDest );
public:
    mCString ReadLine( void );
    void     ToString( mCString & a_strDest );
    void     WriteLine( mCString const & a_strText );
private:
    virtual mEResult ReadFormatted( MILPVoid a_pDest, MILPCChar a_Format );
    virtual mEResult WriteFormatted( MILPCVoid a_pSource, MILPCChar a_Format );
};

#endif