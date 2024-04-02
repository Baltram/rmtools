#include "pluginsdialog.h"
#include "ui_pluginsdialog.h"
#include "rimy3d.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QRegExp>
#include <QUrl>

PluginsDialog::PluginsDialog( QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::PluginsDialog )
{
    m_pUi->setupUi( this );
}

PluginsDialog::~PluginsDialog( void )
{
    delete m_pUi;
}

int PluginsDialog::exec( Plugin a_enuPlugin )
{
    m_pUi->wBlender->setVisible( a_enuPlugin == Blender );
    m_pUi->wGmax->setVisible( a_enuPlugin == GMax );
    m_pUi->wMax->setVisible( a_enuPlugin == Max );
    QString strReadMePath( ":/plugins/plugins/" );
    if ( a_enuPlugin == GMax )
    {
        setWindowTitle( tr( "Extended Saving For GMax" ) );
        if ( Rimy3D::getLanguage() == Rimy3D::ELanguage_German )
            strReadMePath += "Extended Saving/Extended Saving_DE.txt";
        else
            strReadMePath += "Extended Saving/Extended Saving.txt";
    }
    else if ( a_enuPlugin == Blender )
    {
        setWindowTitle( tr( "3db Import/Export For Blender" ) );
        if ( Rimy3D::getLanguage() == Rimy3D::ELanguage_German )
            strReadMePath += "blender_3db/3db_plugin_DE.txt";
        else
            strReadMePath += "blender_3db/3db_plugin.txt";
    }
    else if ( a_enuPlugin == Max )
    {
        setWindowTitle( tr( "3db Tools For 3ds Max" ) );
        if ( Rimy3D::getLanguage() == Rimy3D::ELanguage_German )
            strReadMePath += "max_3db/3db Tools_DE.txt";
        else
            strReadMePath += "max_3db/3db Tools.txt";
    }
    QFile readMeFile( strReadMePath );
    readMeFile.open( QIODevice::ReadOnly | QIODevice::Text );
    m_pUi->plainTextEdit->setPlainText( readMeFile.readAll() );
    return QDialog::exec();
}

namespace
{
    bool overwriteFile( QString a_strPathDest, QString a_strPathSource )
    {
        QFile::setPermissions( a_strPathDest, QFile::permissions( a_strPathDest ) | QFile::WriteUser );
        QFile::remove( a_strPathDest );
        return QFile::copy( a_strPathSource, a_strPathDest );
    }
}

void PluginsDialog::on_pushButton_clicked( void )
{
    QString const strFileName = "installExtendedSaving.gmax";
    QString const strTempFilePath = QDir::temp().absolutePath() + '/' + strFileName;
    overwriteFile( strTempFilePath, ":/plugins/plugins/Extended Saving/" + strFileName );
    QDesktopServices::openUrl( QUrl::fromLocalFile( strTempFilePath ) );
    close();
}

void PluginsDialog::on_pushButton_2_clicked( void )
{
    close();
}

namespace
{
    void getSubDirs( QDir a_Directory, QList< QDir > & a_arrDest )
    {
        QStringList arrDirs = a_Directory.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
        a_arrDest.clear();
        for ( int i = 0, ie = arrDirs.count(); i != ie; ++i )
            a_arrDest.append( QDir( a_Directory.path() + "/" + arrDirs[ i ] ) );
    }

    bool getBlenderAddonsDirectory( QDir & a_Directory, int iDepth = 2 )
    {
        QRegExp versionFolder( "\\d\\.\\d\\d" );
        QList< QDir > arrSubDirs;
        getSubDirs( a_Directory, arrSubDirs );
        if ( !versionFolder.exactMatch( a_Directory.dirName() ) )
        {
            if ( iDepth != 0 && a_Directory.dirName().toLower().contains( "blender" ) )
                for ( int i = arrSubDirs.count(); i--; )
                    if ( ( arrSubDirs[ i ].dirName().toLower().contains( "blender" ) || arrSubDirs[ i ].dirName().length() == 4 ) && getBlenderAddonsDirectory( arrSubDirs[ i ], iDepth - 1 ) )
                        return a_Directory = arrSubDirs[ i ], true;
            return false;
        }
        if ( !a_Directory.cd( "scripts" ) || !a_Directory.cd( "addons" ) )
            return false;
        return true;
    }
}

void PluginsDialog::on_pushButton_3_clicked( void )
{
    QString strDest = QDir::currentPath() + "/io_scene_3db.zip";
    if ( !overwriteFile( strDest, ":/plugins/plugins/blender_3db/io_scene_3db.zip" ) )
    {
        Rimy3D::showError( tr( "Failed to create %1.\nTry running Rimy3D with administrator privileges." ).arg( strDest ) );
        return;
    }
    Rimy3D::showMessage( tr( "The addon has been made available as %1. \nIt can now be installed via the Blender preferences menu." ).arg( strDest ) );
}

void PluginsDialog::on_pushButton_4_clicked( void )
{
    QString strDir = QFileDialog::getExistingDirectory( this, tr( "Please specify your 3ds Max installation directory" ), QDir::homePath() );
    if ( strDir == "" )
        return;
    QDir dirMax( strDir );
    if ( !dirMax.cd( "scripts" ) || !dirMax.cd( "startup" ) )
    {
        Rimy3D::showError( tr( "Could not find 3ds Max's scripts\\startup directory at the specified location." ) );
        return;
    }
    if ( !overwriteFile( dirMax.absolutePath() + "/3dbTools.ms", ":/plugins/plugins/max_3db/3dbTools.ms" ) )
    {
        Rimy3D::showError( tr( "Failed to write to the directory %1\nTry running Rimy3D with administrator privileges." ).arg( dirMax.absolutePath() ) );
        return;
    }
    Rimy3D::showMessage( tr( "3db Tools have been successfully installed." ) );
}
