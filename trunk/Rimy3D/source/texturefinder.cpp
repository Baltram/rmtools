#include "texturefinder.h"
#include "sceneinfo.h"
#include "rimy3d.h"
#include <QDir>

TextureFinder * TextureFinder::s_pInstance = 0;

TextureFinder & TextureFinder::getInstance( void )
{
    if ( !s_pInstance )
        s_pInstance = new TextureFinder;
    return *s_pInstance;
}

void TextureFinder::addSearchPath( QString const & a_strPath )
{
    m_arrPaths.append( a_strPath );
}

QString TextureFinder::findTextureFile( QString const & a_strFilePathGuess, QString const & a_strCurrentDir )
{
    if ( a_strFilePathGuess == "" )
        return "";
    QFileInfo File( a_strFilePathGuess );
    if ( File.exists() )
        return File.canonicalFilePath();
    char const * arrExts[] = { ".jpg", ".jpeg", ".png", ".gif", ".bmp" };
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( a_strCurrentDir + QDir::separator() + File.baseName() + arrExts[ i ] );
        if ( File.exists() )
            return File.canonicalFilePath();
    }
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( "bitmap:" + File.baseName() + arrExts[ i ] );
        if ( File.exists() )
            return File.canonicalFilePath();
    }
    return "";
}

int TextureFinder::getNumSearchPaths( void )
{
    return m_arrPaths.count();
}

void TextureFinder::removeSearchPathAt( int a_iPosition )
{
    m_arrPaths.removeAt( a_iPosition );
}

void TextureFinder::showDialog( void )
{
    m_Dialog.exec();
}

void TextureFinder::updateSearchPaths( void )
{
    QDir::setSearchPaths( "bitmap", m_arrPaths );
}

void TextureFinder::loadSettings( QSettings & a_Settings )
{
    m_Dialog.addItems( a_Settings.value( "searchpaths" ).toStringList() );
    updateSearchPaths();
}

void TextureFinder::saveSettings( QSettings & a_Settings )
{
    a_Settings.setValue( "searchpaths", m_arrPaths );
}

TextureFinder::TextureFinder( void )
{
    connect( Rimy3D::getInstance(), SIGNAL( onSaveSettings( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( onLoadSettings( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

TextureFinder::TextureFinder( TextureFinder const & )
{
}
