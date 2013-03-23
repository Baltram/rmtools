#include "texturefinder.h"
#include "sceneinfo.h"
#include "rimy3d.h"
#include "Mimicry.h"
#include <QBuffer>
#include <QDir>

TextureFinder * TextureFinder::s_pInstance = 0;

TextureFinder & TextureFinder::getInstance( void )
{
    if ( !s_pInstance )
        s_pInstance = new TextureFinder;
    return *s_pInstance;
}

bool TextureFinder::findTexture( QString const & a_strFilePathGuess, QString const & a_strCurrentDir, QImage & a_imgDest )
{
    char const * arrExts[] = { "jpg", "jpeg", "png", "gif", "bmp", "dds", "ximg", "_ximg", "tga" };
    if ( a_strFilePathGuess == "" )
        return false;
    QFileInfo File( a_strFilePathGuess );
    if ( File.exists() )
        for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
            if ( File.suffix().toLower() == arrExts[ i ] )
                if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                    return true;
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( a_strCurrentDir + QDir::separator() + File.baseName() + "." + arrExts[ i ] );
        if ( File.exists() )
            if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                return true;
    }
    QStringList arrArchives = m_Dialog.getArchives();
    for ( int i = 0, ie = arrArchives.count(); i != ie; ++i )
    {
        mCGenomeVolume * pArchive = m_mapArchives.contains( arrArchives[ i ] ) ? m_mapArchives[ arrArchives[ i ] ] : 0;
        if ( !pArchive || !pArchive->IsOpen() )
            continue;
        mCString strRelativeFilePath;
        mCMemoryStream streamData;
        if ( pArchive->FindFile( File.baseName().toAscii().data(), strRelativeFilePath, streamData ) )
        {
            File.setFile( strRelativeFilePath.GetText() );
            QBuffer ImageData;
            ImageData.setData( static_cast< char const * >( streamData.GetBuffer() ), streamData.GetSize() );
            if ( a_imgDest.load( &ImageData, File.suffix().toAscii().data() ) )
                return true;
        }
    }
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( "bitmap:" + File.baseName() + "." + arrExts[ i ] );
        if ( File.exists() )
            if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                return true;
    }
    return false;
}

void TextureFinder::showDialog( void )
{
    m_Dialog.exec();
}

void TextureFinder::updateArchives( void )
{
    QStringList arrArchives = m_Dialog.getArchives();
    QStringList arrObsoleteArchives;
    for ( QHash< QString, mCGenomeVolume * >::iterator i = m_mapArchives.begin(), ie = m_mapArchives.end(); i != ie; ++i )
        if ( !arrArchives.contains( i.key() ) )
            arrObsoleteArchives.append( i.key() );
    for ( int i = 0, ie = arrObsoleteArchives.count(); i != ie; ++i )
        m_mapArchives.remove( ( delete m_mapArchives[ arrObsoleteArchives[ i ] ], arrObsoleteArchives[ i ] ) );
    for ( int i = 0, ie = arrArchives.count(); i != ie; ++i )
        if ( !m_mapArchives.contains( arrArchives[ i ] ) )
            m_mapArchives[ arrArchives[ i ] ] = new mCGenomeVolume( arrArchives[ i ].toAscii().data() );
}

void TextureFinder::updateSearchPaths( void )
{
    QStringList arrPaths = m_Dialog.getPaths();
    QDir::setSearchPaths( "bitmap", arrPaths );
}

void TextureFinder::loadSettings( QSettings & a_Settings )
{
    m_Dialog.addArchives( a_Settings.value( "archives" ).toStringList() );
    m_Dialog.addPaths( a_Settings.value( "searchpaths" ).toStringList() );
    updateArchives();
    updateSearchPaths();
}

void TextureFinder::saveSettings( QSettings & a_Settings )
{
    QStringList arrArchives = m_Dialog.getArchives();
    QStringList arrPaths = m_Dialog.getPaths();
    a_Settings.setValue( "archives", arrArchives );
    a_Settings.setValue( "searchpaths", arrPaths );
}

TextureFinder::TextureFinder( void )
{
    connect( Rimy3D::getInstance(), SIGNAL( settingsSaving( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( settingsLoading( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

TextureFinder::TextureFinder( TextureFinder const & )
{
}
