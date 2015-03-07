#ifndef MI_3DSWRITER_H_INCLUDED
#define MI_3DSWRITER_H_INCLUDED

class mC3dsWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        MIBool m_bExportSmoothingGroups;
        MIBool m_bGothic3ds;
    };
public:
    static mEResult Write3dsFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif
