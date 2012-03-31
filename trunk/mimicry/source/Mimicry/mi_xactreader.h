#ifndef MI_XACTREADER_H_INCLUDED
#define MI_XACTREADER_H_INCLUDED

class mCXactReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        mCString m_strTextureFileExtension;
    };
public:
    static mEResult ReadXactFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() ); 
};

#endif