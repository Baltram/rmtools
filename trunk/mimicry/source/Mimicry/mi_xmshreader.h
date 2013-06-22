#ifndef MI_XMSHREADER_H_INCLUDED
#define MI_XMSHREADER_H_INCLUDED

class mCXmshReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult ReadXmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() );
};

#endif
