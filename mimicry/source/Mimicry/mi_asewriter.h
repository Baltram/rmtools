#ifndef MI_ASEWRITER_H_INCLUDED
#define MI_ASEWRITER_H_INCLUDED

class mCAseWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        MIBool m_bGothicAscFormat;
    };
public:
    static mEResult WriteAseFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif