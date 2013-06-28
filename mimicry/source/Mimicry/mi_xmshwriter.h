#ifndef MI_XMSHWRITER_H_INCLUDED
#define MI_XMSHWRITER_H_INCLUDED

class mCXmshWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult WriteXmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif
