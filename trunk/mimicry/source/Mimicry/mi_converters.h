#ifndef MI_CONVERTERS_H_INCLUDED
#define MI_CONVERTERS_H_INCLUDED

MI_ERROR_DEFINE_DERIVED( mCConverterError, 
                         mCError, 
                         EBadFormat,
                         EBadScene,
                         EMiscellaneous )

struct eSConverterOptions
{
public:
    eSConverterOptions( void );
public:
    MIBool   m_bDropVertexColors;
    MIBool   m_bDropVertexNormals;
    MIBool   m_bRecalculateVertexNormals;
    MIBool   m_bUseAnglesInsteadOfSGs;
    MIFloat  m_fMaxAngleInDegrees;
    mCString m_strTextureFileExtension;
};

#endif
