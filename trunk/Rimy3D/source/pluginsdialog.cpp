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
    QString strReadMePath( ":/plugins/plugins/" );
    if ( a_enuPlugin == GMax )
    {
        setWindowTitle( tr( "Extended Saving for GMax" ) );
        if ( Rimy3D::getLanguage() == Rimy3D::ELanguage_German )
            strReadMePath += "Extended Saving/Extended Saving_DE.txt";
        else
            strReadMePath += "Extended Saving/Extended Saving.txt";
    }
    else if ( a_enuPlugin == Blender )
    {
        setWindowTitle( tr( "3db Import/Export for Blender" ) );
        if ( Rimy3D::getLanguage() == Rimy3D::ELanguage_German )
            strReadMePath += "blender_3db/3db_plugin_DE.txt";
        else
            strReadMePath += "blender_3db/3db_plugin.txt";
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
    QString strDir = QFileDialog::getExistingDirectory( this, tr( "Please specify your Blender installation directory" ), QDir::homePath() );
    if ( strDir == "" )
        return;
    QDir dirBlender( strDir );
    if ( !getBlenderAddonsDirectory( dirBlender ) )
    {
        Rimy3D::showError( tr( "Could not find Blender's addons directory at the specified location." ) );
        return;
    }
    QString strDirDest = dirBlender.absolutePath() + "/io_scene_3db/";
    QString strDirSource = ":/plugins/plugins/blender_3db/";
    if ( !( QDir( strDirDest ).exists() || QDir( strDirDest ).mkdir( "." ) ) ||
         !overwriteFile( strDirDest + "__init__.py", strDirSource + "__init__.py" ) ||
         !overwriteFile( strDirDest + "export_3db.py", strDirSource + "export_3db.py" ) ||
         !overwriteFile( strDirDest + "import_3db.py", strDirSource + "import_3db.py" ) )
    {
        Rimy3D::showError( tr( "Rimy3D has not write permissions to %1\nTry running Rimy3D with administrator rights." ).arg( dirBlender.absolutePath() ) );
        return;
    }
    Rimy3D::showMessage( tr( "The script files have been successfully copied to %1\nYou can now start Blender and enable the addon in 'File' > 'User Preferences'." ).arg( dirBlender.absolutePath() ) );
}
