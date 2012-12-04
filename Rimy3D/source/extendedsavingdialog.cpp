#include "extendedsavingdialog.h"
#include "ui_extendedsavingdialog.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QUrl>

ExtendedSavingDialog::ExtendedSavingDialog( QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::ExtendedSavingDialog )
{
    m_pUi->setupUi( this );
    QFile readMeFile( ":/extended_saving/Extended Saving.txt" );
    readMeFile.open( QIODevice::ReadOnly | QIODevice::Text );
    m_pUi->plainTextEdit->setPlainText( readMeFile.readAll() );
}

ExtendedSavingDialog::~ExtendedSavingDialog( void )
{
    delete m_pUi;
}

void ExtendedSavingDialog::on_pushButton_clicked( void )
{
    QString const strFileName = "installExtendedSaving.gmax";
    QString const strTempFilePath = QDir::temp().absolutePath() + '/' + strFileName;
    QFile::remove( strTempFilePath );
    QFile::copy( ":/extended_saving/" + strFileName, strTempFilePath );
    QDesktopServices::openUrl( QUrl::fromLocalFile( strTempFilePath ) );
    close();
}

void ExtendedSavingDialog::on_pushButton_2_clicked( void )
{
    close();
}
