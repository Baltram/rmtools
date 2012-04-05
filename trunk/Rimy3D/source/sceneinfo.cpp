#include <QFileInfo>
#include "sceneinfo.h"
#include "rimy3d.h"

SceneInfo::SceneInfo( void )
{
    connect( Rimy3D::getInstance(), SIGNAL( onSaveSettings( QSettings & ) ), this, SLOT( saveSettings( QSettings & ) ) );
    connect( Rimy3D::getInstance(), SIGNAL( onLoadSettings( QSettings & ) ), this, SLOT( loadSettings( QSettings & ) ) );
}

SceneInfo::~SceneInfo( void )
{
}

void SceneInfo::clearScene( void )
{
    m_strCurrentFile = "";
    m_sceneCurrentScene.Clear();
    emit sceneChanged();
}

QString const & SceneInfo::getCurrentDir( void )
{
    return m_strCurrentDir;
}

QString const & SceneInfo::getCurrentFile( void )
{
    return m_strCurrentFile;
}

mCScene const & SceneInfo::getCurrentScene( void )
{
    return m_sceneCurrentScene;
}

bool SceneInfo::openSceneFile( QString a_strFileName )
{
    QFileInfo FileInfo( a_strFileName );
    if ( !FileInfo.exists() )
        return false;
    mCFileStream streamIn;
    if ( streamIn.Open( FileInfo.absoluteFilePath().toAscii().data(), mEFileOpenMode_Read ) == mEResult_False )
        return false;
    mCError const * pLastError = mCError::GetLastError< mCError >();
    mEResult enuResult = mEResult_False;
    mCScene sceneNew;
    QString strExt = FileInfo.suffix().toLower();
    if ( strExt == "obj" )
    {
        enuResult = mCObjReader::ReadObjFileData( sceneNew, streamIn, FileInfo.absolutePath().toAscii().data() );
    }
    else if ( strExt == "3db" )
    {
        enuResult = mC3dbReader::Read3dbFileData( sceneNew, streamIn );
    }
    else if ( strExt == "gmax" )
    {
        mCMaxFileStream streamInMax( streamIn.GetFileName() );
        enuResult = mCMaxReader::ReadInMaxFileData( sceneNew, streamInMax );
        streamInMax.Close();
    }
    else if ( ( strExt == "ase" ) || ( strExt == "asc" ) )
    {
        enuResult = mCAseReader::ReadAseFileData( sceneNew, streamIn );
    }
    else if ( strExt == "xact" )
    {
        enuResult = mCXactReader::ReadXactFileData( sceneNew, streamIn );
    }
    streamIn.Close();
    if ( enuResult == mEResult_Ok )
    {
        m_strCurrentDir = FileInfo.absolutePath();
        m_strCurrentFile = FileInfo.fileName();
        m_sceneCurrentScene.Swap( sceneNew );
        emit sceneChanged();
        return true;
    }
    mCString strError;
    for ( mCError const * pError = 0; ( pError = mCError::GetLastError< mCError >() ) != pLastError; mCError::ClearError( pError ) )
        strError = pError->GetText();
    if ( strError != "" )
        Rimy3D::showError( tr( strError.GetText() ) );
    return false;
}

void SceneInfo::loadSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    m_strCurrentDir = a_Settings.value( "dir", "" ).toString();
    a_Settings.endGroup();
}

void SceneInfo::saveSettings( QSettings & a_Settings )
{
    a_Settings.beginGroup( "SceneInfo" );
    a_Settings.setValue( "dir", m_strCurrentDir );
    a_Settings.endGroup();
}

void SceneInfo::errorMessageTranslations( void )
{
    tr( "Invalid .gmax file. The file might have been saved without Extended Saving enabled." );
    tr( "Unknown Extended Saving version." );
    tr( "Unknown .ase file version." );
    tr( "Invalid .ase file." );
    tr( "Invalid .xact file." );
    tr( "Invalid .3db file." );
    tr( "Invalid source .xact file." );
    tr( "Cannot find same-named mesh in source .xact file." );
    tr( "New mesh has wrong vertex count." );
    tr( "New mesh has no material." );
    tr( "Skinning includes bone not present in .xact file." );
    tr( "Skinning does not cover all vertices." );
}
