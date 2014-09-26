#ifndef MI_DOCPARSER_H_INCLUDED
#define MI_DOCPARSER_H_INCLUDED

class mCDocParser
{
public:
    mCDocParser( mCStringStream & a_streamIn, mCIOStreamBinary & a_streamOut );
public:
    MIBool EnterArray( MIBool a_bSetLastErrorLine = MIFalse );
    MIBool EnterBlock( mCString * a_pName = 0, MIBool a_bSetLastErrorLine = MIFalse, MIBool a_bCaseSensitive = MIFalse );
    MIBool EnterBlock( mCString const & a_strName, MIBool a_bSetLastErrorLine = MIFalse, MIBool a_bCaseSensitive = MIFalse );
    MIUInt GetLastErrorLine( void );
    MIBool LeaveArray( MIBool a_bSetLastErrorLine = MIFalse );
    MIBool LeaveBlock( MIBool a_bSetLastErrorLine = MIFalse );
    MIBool Match( mCString a_strText, mCStringArray & a_arrTokens, MIBool a_bSetLastErrorLine = MIFalse, MIBool a_bCaseSensitive = MIFalse, MIBool a_bFulltext = MIFalse );
    MIBool MatchImmediate( mCString a_strText, MIBool a_bSetLastErrorLine = MIFalse, MIBool a_bCaseSensitive = MIFalse, MIBool a_bFulltext = MIFalse, MIBool a_bAtStart = MITrue );
protected:
    void SetLastErrorLine( MIBool a_bForReal, MIBool a_bSkipWhitespaces = MITrue );
private:
    mCDocParser( mCDocParser const & );
private:
    mCDocParser & operator = ( mCDocParser const & );
protected:
    MIUInt             m_uLastErrorLine;
    mCStringStream   & m_streamIn;
    mCIOStreamBinary & m_streamOut;
};

#endif