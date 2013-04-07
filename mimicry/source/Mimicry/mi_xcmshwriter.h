#ifndef MI_XCMSHWRITER_H_INCLUDED
#define MI_XCMSHWRITER_H_INCLUDED

class mCXcmshWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult WriteXcmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif
