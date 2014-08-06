#ifndef MI_XNVMSHWRITER_H_INCLUDED
#define MI_XNVMSHWRITER_H_INCLUDED

class mCXnvmshWriter
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
    static mEResult WriteXnvmshFileData( mCScene a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options );
};

#endif
