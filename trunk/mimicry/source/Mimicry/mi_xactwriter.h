#ifndef MI_XACTWRITER_H_INCLUDED
#define MI_XACTWRITER_H_INCLUDED

class mCXactWriter
{
public:
    struct SOptions :
        public eSConverterOptions
    {
    public:
        SOptions( void );
    public:
        mTArray< mCString > * m_pReplacedMeshesArray;
        MIBool                m_bReplaceOnlyVertices;
        MIBool                m_bIndirectVertexMatching;
        mCIOStreamBinary *    m_pBaseXactStream;
    };
public:
    static mEResult WriteXactFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest, SOptions a_Options );
};

#endif
