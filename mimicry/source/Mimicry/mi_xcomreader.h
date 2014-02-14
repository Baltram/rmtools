#ifndef MI_XCOMREADER_H_INCLUDED
#define MI_XCOMREADER_H_INCLUDED

class mCXcomReader
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult ReadXcomFileData( mCScene & a_sceneDest, mCIOStreamBinary & a_streamSource, SOptions a_Options = SOptions() );
public:
    static MILPCChar const s_arrShapeMaterialNames[];
};

#endif
