#include "texturefinder.h"

TextureFinder * TextureFinder::s_pInstance = 0;

TextureFinder & TextureFinder::getInstance( void )
{
    if ( !s_pInstance )
        s_pInstance = new TextureFinder;
    return *s_pInstance;
}

QString TextureFinder::findTextureFile( QString const & a_strFilePathGuess )
{
    return a_strFilePathGuess;
}

TextureFinder::TextureFinder( void )
{
}

TextureFinder::TextureFinder( TextureFinder const & )
{

}
