#ifndef MI_DOC_H_INCLUDED
#define MI_DOC_H_INCLUDED

class mCDoc
{
public:
    mCDoc( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut, mCString const & a_strIndent = "" );
public:
    void EndArray( MIBool a_bNewLine = MITrue );
    void EndBlock( MIBool a_bNewLine = MITrue );
    void StartArray( mCString const & a_strName = "" );
    void StartBlock( mCString const & a_strTitle = "" );
    void Write( mCString const & a_strText );
    void WriteComment( mCString const & a_strText, MIBool a_bNewLine = MITrue );
    void WriteLine( mCString const & a_strText = "" );
protected:
    void DecrementIndent( void );
    void IncrementIndent( void );
private:
    mCDoc( mCDoc const & );
private:
    mCDoc & operator = ( mCDoc const & );
protected:
    mCString           m_strIndent;
    mCIOStreamBinary & m_streamIn;
    mCIOStreamBinary & m_streamOut;
};

#endif