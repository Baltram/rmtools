#include "mi_include_converters.h"

eSConverterOptions::eSConverterOptions( void ) :
    m_bDropVertexColors( MIFalse ),
    m_bDropVertexNormals( MIFalse ),
    m_bRecalculateVertexNormals( MIFalse ),
    m_bUseAnglesInsteadOfSGs( MIFalse ),
    m_fMaxAngleInDegrees( 70.0f ),
    m_strTextureFileExtension( "jpg" )
{
}
