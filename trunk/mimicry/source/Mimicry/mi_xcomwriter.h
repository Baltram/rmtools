#ifndef MI_XCOMWRITER_H_INCLUDED
#define MI_XCOMWRITER_H_INCLUDED

class mCXcomWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        MIBool m_bConvex;
    };
public:
    static mEResult WriteXcomFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options );
};

#endif
