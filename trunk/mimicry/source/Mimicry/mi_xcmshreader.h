#ifndef MI_XCMSHREADER_H_INCLUDED
#define MI_XCMSHREADER_H_INCLUDED

class mCXcmshReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult ReadXcmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() ); 
};

#endif