#include "texturefinder.h"
#include "sceneinfo.h"
#include "rimy3d.h"
#include "Mimicry.h"
#include <QBuffer>
#include <QDir>

namespace
{
    mCIOStreamBinary * s_pStreamFile = 0;

    void findFileStatic( char const * a_pcFileName, char const * a_pcFileExt, mCIOStreamBinary *& a_pDest )
    {
        TextureFinder::getInstance().findFile( a_pcFileName, a_pcFileExt, 0 );
        a_pDest = s_pStreamFile;
        s_pStreamFile = 0;
    }
}

TextureFinder * TextureFinder::s_pInstance = 0;

TextureFinder & TextureFinder::getInstance( void )
{
    if ( !s_pInstance )
        s_pInstance = new TextureFinder;
    return *s_pInstance;
}

void TextureFinder::findFile( char const * a_pcFileName, char const * a_pcFileExt, QByteArray * a_pDest )
{
    delete s_pStreamFile;
    s_pStreamFile = 0;
    for ( ; ; )
    {
        QFileInfo File( a_pcFileName );
        File.setFile( File.path() + QDir::separator() + File.baseName() + "." + a_pcFileExt );
        if ( File.exists() )
            s_pStreamFile = new mCFileStream( File.absoluteFilePath().toAscii().data(), mEFileOpenMode_Read );
        if ( s_pStreamFile )
            break;
        QStringList arrArchives = m_Dialog.getArchives();
        for ( int i = 0, ie = arrArchives.count(); i != ie; ++i )
        {
            mCGenomeVolume * pArchive = m_mapArchives.contains( arrArchives[ i ] ) ? m_mapArchives[ arrArchives[ i ] ] : 0;
            if ( !pArchive || !pArchive->IsOpen() )
                continue;
            mCString strRelativeFilePath;
            s_pStreamFile = new mCMemoryStream;
            if ( pArchive->FindFile( File.baseName().toAscii().data(), strRelativeFilePath, *dynamic_cast< mCMemoryStream * >( s_pStreamFile ) ) &&
                 g_GetFileExt( strRelativeFilePath ).ToLower() == a_pcFileExt )
                break;
            delete s_pStreamFile;
            s_pStreamFile = 0;
        }
        if ( s_pStreamFile )
            break;
        File.setFile( "bitmap:" + File.fileName() );
        if ( File.exists() )
            s_pStreamFile = new mCFileStream( File.absoluteFilePath().toAscii().data(), mEFileOpenMode_Read );
        break;
    }
    if ( dynamic_cast< mCFileStream * >( s_pStreamFile ) && !dynamic_cast< mCFileStream * >( s_pStreamFile )->IsOpen() )
    {
        delete s_pStreamFile;
        s_pStreamFile = 0;
    }
    if ( s_pStreamFile )
        s_pStreamFile->Seek( 0 );
    if ( a_pDest && s_pStreamFile )
    {
        a_pDest->resize( s_pStreamFile->GetSize() );
        s_pStreamFile->Read( a_pDest->data(), s_pStreamFile->GetSize() );
        delete s_pStreamFile;
        s_pStreamFile = 0;
    }
}

bool TextureFinder::findTexture( QString const & a_strFilePathGuess, QString const & a_strCurrentDir, QImage & a_imgDest, bool a_bExtensiveSearch )
{
    char const * arrExts[] = { "jpg", "jpeg", "png", "gif", "bmp", "dds", "ximg", "_ximg", "tga" };
    if ( a_strFilePathGuess == "" )
        return false;
    QFileInfo File( a_strFilePathGuess );
    if ( File.exists() )
        for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
            if ( File.suffix().toLower() == arrExts[ i ] )
                if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                    return a_imgDest.setText( "OriginalFileName", File.fileName() ), true;
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( a_strCurrentDir + QDir::separator() + File.baseName() + "." + arrExts[ i ] );
        if ( File.exists() )
            if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                return a_imgDest.setText( "OriginalFileName", File.fileName() ), true;
    }
    if ( !a_bExtensiveSearch )
        return false;
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
                return a_imgDest.setText( "OriginalFileName", QFileInfo( strRelativeFilePath.GetText() ).fileName() ), true;
        }
    }
    for ( int i = 0, ie = sizeof( arrExts ) / sizeof( arrExts[ 0 ] ); i != ie; ++i )
    {
        File.setFile( "bitmap:" + File.baseName() + "." + arrExts[ i ] );
        if ( File.exists() )
            if ( a_imgDest.load( File.canonicalFilePath(), File.suffix().toAscii().data() ) )
                return a_imgDest.setText( "OriginalFileName", File.fileName() ), true;
    }
    return false;
}

void TextureFinder::showDialog( void )
{
    m_Dialog.move( QApplication::activeWindow()->geometry().center() - m_Dialog.rect().center() );
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
    mCGenomeMaterial::RegisterMaterialFileFinderFunction( &findFileStatic );
}

TextureFinder::TextureFinder( TextureFinder const & )
{
}
