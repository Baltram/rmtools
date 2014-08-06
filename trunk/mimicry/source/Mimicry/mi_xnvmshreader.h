#ifndef MI_XNVMSHREADER_H_INCLUDED
#define MI_XNVMSHREADER_H_INCLUDED

class mCXnvmshReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult ReadXnvmshFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() );
};

#endif
