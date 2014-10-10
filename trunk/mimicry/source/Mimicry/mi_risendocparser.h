#ifndef MI_RISENDOCPARSER_H_INCLUDED
#define MI_RISENDOCPARSER_H_INCLUDED

class mCRisenDocParser :
    public mCDocParser
{
public:
    mCRisenDocParser( mCStringStream & a_streamIn, mCIOStreamBinary & a_streamOut );
public:
    MIBool ParseRisen3Class( MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseProperty( MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseRisen3DlgData2( MIBool a_bSetLastErrorLine = MITrue );
protected:
    MIBool ParseData( mCString a_strType, MIBool a_bWriteSize = MIFalse, MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseVersion( MIU16 & a_u16Version, MIBool a_bSetLastErrorLine = MITrue );
private:
    mCRisenDocParser( mCRisenDocParser const & );
private:
    mCRisenDocParser & operator = ( mCRisenDocParser const & );
};

#endif