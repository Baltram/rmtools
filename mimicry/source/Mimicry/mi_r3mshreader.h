#ifndef MI_R3MSHREADER_H_INCLUDED
#define MI_R3MSHREADER_H_INCLUDED

class mCR3mshReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult ReadR3mshFileData(mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions());
};

#endif
