#include "searchpathsdialog.h"
#include "ui_searchpathsdialog.h"
#include "texturefinder.h"
#include <QFileDialog>

SearchPathsDialog::SearchPathsDialog( QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::SearchPathsDialog )
{
    m_pUi->setupUi( this );
}

SearchPathsDialog::~SearchPathsDialog( void )
{
    delete m_pUi;
}

void SearchPathsDialog::addItems( QStringList const & a_arrItems )
{
    for ( int i = 0, ie = a_arrItems.count(); i != ie; ++i )
    {
        m_pUi->listWidget->addItem( a_arrItems[ i ] );
        TextureFinder::getInstance().addSearchPath( a_arrItems[ i ] );
    }
}

void SearchPathsDialog::addSubDirectories( QStringList & a_arrDirs )
{
    for ( int i = a_arrDirs.count(); i--; )
    {
        QStringList arrSubDirs = QDir( a_arrDirs[ i ] ).entryList( QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks );
        for ( int j = arrSubDirs.count(); j--; arrSubDirs[ j ] = a_arrDirs[ i ] + QDir::separator() + arrSubDirs[ j ] );
        addSubDirectories( arrSubDirs );
        a_arrDirs << arrSubDirs;
    }
}

void SearchPathsDialog::changeEvent( QEvent * a_pEvent )
{
    if ( a_pEvent->type() == QEvent::LanguageChange )
        updateLanguage();
    QDialog::changeEvent( a_pEvent );
}

void SearchPathsDialog::closeEvent( QCloseEvent * a_pEvent )
{
    TextureFinder::getInstance().updateSearchPaths();
    QDialog::closeEvent( a_pEvent );
}

void SearchPathsDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
}

void SearchPathsDialog::on_listWidget_currentRowChanged( int a_iCurrentRow )
{
    m_pUi->pushButton_2->setEnabled( a_iCurrentRow != -1 );
}

void SearchPathsDialog::on_pushButton_clicked()
{
    QStringList arrDirs( QFileDialog::getExistingDirectory( this ) );
    if ( arrDirs.front() == "" )
        return;
    if ( m_pUi->checkBox->isChecked() )
        addSubDirectories( arrDirs );
    arrDirs.sort();
    addItems( arrDirs );
}

void SearchPathsDialog::on_pushButton_2_clicked( void )
{
    TextureFinder::getInstance().removeSearchPathAt( m_pUi->listWidget->currentRow() );
    delete m_pUi->listWidget->currentItem();
}

void SearchPathsDialog::on_pushButton_3_clicked( void )
{
    close();
}
