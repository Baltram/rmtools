#ifndef MI_3DBWRITER_H_INCLUDED
#define MI_3DBWRITER_H_INCLUDED

class mC3dbWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult Write3dbFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif