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
    MIBool   DocumentRisen3DlgData2( void );
    MIUInt   DocumentRisen3Class( void );
    MIBool   DocumentRisen3Template( void );
    mCString ReadHash( mCString const & a_strType = "hash" );
private:
    mCRisenDoc( mCRisenDoc const & );
private:
    mCRisenDoc & operator = ( mCRisenDoc const & );
private:
    MIBool DocumentRisen3TemplateClass( MIBool a_bMasterEntity );
};

#endif