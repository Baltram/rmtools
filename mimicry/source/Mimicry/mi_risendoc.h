#ifndef MI_RISENDOC_H_INCLUDED
#define MI_RISENDOC_H_INCLUDED

class mCRisenDoc :
    public mCDoc
{
public:
    mCRisenDoc( mCIOStreamBinary & a_streamIn, mCIOStreamBinary & a_streamOut, mCString const & a_strIndent = "" );
public:
    static mCString FormatData( mCIOStreamBinary & a_streamSource, mCString const & a_strType, MIUInt uSize = MI_DW_INVALID, mCString const & a_strIndent = "" );
public:
    virtual void FormatData( mCString a_strType, MIUInt uSize = MI_DW_INVALID );
public:
    MIBool   DocumentElexWorld( void );
    MIBool   DocumentRisen3DlgData2( void );
    MIUInt   DocumentRisen3Class( void );
    void     DocumentRisen3ClassData( mCString a_strName, MIUInt uVersion );
    MIBool   DocumentRisen3Sector( MIBool a_bElex = MIFalse );
    MIBool   DocumentRisen3Template( MIBool a_bElex = MIFalse );
    MIBool   DocumentRisen3Hdr( void );
    mCString ReadHash( mCString const & a_strType = "hash" );
private:
    mCRisenDoc( mCRisenDoc const & );
private:
    mCRisenDoc & operator = ( mCRisenDoc const & );
private:
    MIBool DocumentRisen3DynamicEntity( MIBool a_bElex = MIFalse );
    MIBool DocumentRisen3TemplateClass( MIBool a_bMasterEntity, MIBool a_bElex = MIFalse );
    void   FormatVariable( mCString const & a_strName, mCString const & a_strType, MIUInt a_uSize = MI_DW_INVALID );
};

#endif