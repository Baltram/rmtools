#ifndef MI_OBJWRITER_H_INCLUDED
#define MI_OBJWRITER_H_INCLUDED

class mCObjWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        MIBool m_bWriteMtlFile;
        mCString m_strMtlFilePath;
    };
public:
    static mEResult WriteObjFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options = SOptions() );
};

#endif