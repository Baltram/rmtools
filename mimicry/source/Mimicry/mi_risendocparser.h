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
    MIBool ParseRisen3Sector( MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseRisen3Template( mCString const a_strName, MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseRisen3Hdr( MIBool a_bSetLastErrorLine = MITrue );
protected:
    void   CheckNewPosRot( void );
    MIBool ParseData( mCString a_strType, MIBool a_bWriteSize = MIFalse, MIBool a_bSetLastErrorLine = MITrue );
    MIBool ParseVariable( mCString const a_strName, mCString const a_strType, MIBool a_bSetLastErrorLine = MITrue, MIBool a_bCheckNewPosRot = MIFalse );
    MIBool ParseVersion( MIU16 & a_u16Version, MIBool a_bSetLastErrorLine = MITrue );
private:
    mCRisenDocParser( mCRisenDocParser const & );
private:
    mCRisenDocParser & operator = ( mCRisenDocParser const & );
private:
    MIBool ParseRisen3DynamicEntity( MIBool a_bSetLastErrorLine = MITrue, MILPCVoid a_pParentMat = 0 );
    MIBool ParseRisen3TemplateClass( mCString a_strName, MIU64 a_u4TimeSwapped, MIBool a_bSetLastErrorLine = MITrue );
};

#endif