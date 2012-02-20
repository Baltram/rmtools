#ifndef MI_ASEWRITER_H_INCLUDED
#define MI_ASEWRITER_H_INCLUDED

class mCAseWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    };
public:
    static mEResult WriteAseFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options );
};

#endif