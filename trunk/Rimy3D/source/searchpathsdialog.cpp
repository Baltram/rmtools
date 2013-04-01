#include "searchpathsdialog.h"
#include "ui_searchpathsdialog.h"
#include "texturefinder.h"
#include <QFileDialog>

SearchPathsDialog::SearchPathsDialog( QWidget * a_pParent ) :
    QDialog( a_pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint ),
    m_pUi( new Ui::SearchPathsDialog )
{
    m_pUi->setupUi( this );
    m_pUi->pbUp->setIcon( qApp->style()->standardIcon( QStyle::SP_ArrowUp ) );
    m_pUi->pbDown->setIcon( qApp->style()->standardIcon( QStyle::SP_ArrowDown ) );
}

SearchPathsDialog::~SearchPathsDialog( void )
{
    delete m_pUi;
}

void SearchPathsDialog::addArchives( QStringList const & a_arrArchives )
{
    for ( int i = 0, ie = a_arrArchives.count(); i != ie; ++i )
        if ( m_pUi->lwArchives->findItems( a_arrArchives[ i ], Qt::MatchFixedString ).count() == 0 )
            m_pUi->lwArchives->addItem( a_arrArchives[ i ] );
}


void SearchPathsDialog::addPaths( QStringList const & a_arrPaths )
{
    for ( int i = 0, ie = a_arrPaths.count(); i != ie; ++i )
        if ( m_pUi->lwPaths->findItems( a_arrPaths[ i ], Qt::MatchFixedString ).count() == 0 )
            m_pUi->lwPaths->addItem( a_arrPaths[ i ] );
}

QStringList SearchPathsDialog::getArchives( void )
{
    QStringList arrResult;
    for ( int i = 0, ie = m_pUi->lwArchives->count(); i != ie; ++i )
        arrResult.append( m_pUi->lwArchives->item( i )->text() );
    return arrResult;
}

QStringList SearchPathsDialog::getPaths( void )
{
    QStringList arrResult;
    for ( int i = 0, ie = m_pUi->lwPaths->count(); i != ie; ++i )
        arrResult.append( m_pUi->lwPaths->item( i )->text() );
    return arrResult;
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
    TextureFinder::getInstance().updateArchives();
    TextureFinder::getInstance().updateSearchPaths();
    QDialog::closeEvent( a_pEvent );
}

void SearchPathsDialog::updateLanguage( void )
{
    m_pUi->retranslateUi( this );
}

void SearchPathsDialog::on_lwArchives_currentRowChanged( int a_iCurrentRow )
{
    m_pUi->pbRemoveArchive->setEnabled( a_iCurrentRow != -1 );
    m_pUi->pbUp->setEnabled( a_iCurrentRow != -1 );
    m_pUi->pbDown->setEnabled( a_iCurrentRow != -1 );
}

void SearchPathsDialog::on_lwPaths_currentRowChanged( int a_iCurrentRow )
{
    m_pUi->pbRemovePath->setEnabled( a_iCurrentRow != -1 );
}

void SearchPathsDialog::on_pbAddArchive_clicked( void )
{
    static QString s_strPath = QDir::homePath();
    QStringList arrArchives( QFileDialog::getOpenFileName( this, tr( "Choose an archive" ), s_strPath, "Genome Volumes and Patches (*.pak *.p00 *.00 *.p0? *.0?)" ) );
    if ( arrArchives.front() == "" )
        return;
    s_strPath = QFileInfo( arrArchives.front() ).absolutePath();
    addArchives( arrArchives );
}

void SearchPathsDialog::on_pbAddPath_clicked( void )
{
    static QString s_strPath = QDir::homePath();
    QStringList arrDirs( QFileDialog::getExistingDirectory( this, "Choose a folder", s_strPath ) );
    if ( arrDirs.front() == "" )
        return;
    s_strPath = QFileInfo( arrDirs.front() + ".1" ).absolutePath();
    if ( m_pUi->cbAddSubpaths->isChecked() )
        addSubDirectories( arrDirs );
    addPaths( arrDirs );
}

void SearchPathsDialog::on_pbDown_clicked()
{
    int iCurrentRow = m_pUi->lwArchives->currentRow();
    if ( iCurrentRow < 0 || iCurrentRow + 1 == m_pUi->lwArchives->count() )
        return;
    QListWidgetItem * pItem = m_pUi->lwArchives->takeItem( iCurrentRow );
    m_pUi->lwArchives->insertItem( iCurrentRow + 1, pItem );
    m_pUi->lwArchives->setCurrentItem( pItem );
}

void SearchPathsDialog::on_pbOk_clicked( void )
{
    close();
}

void SearchPathsDialog::on_pbRemoveArchive_clicked( void )
{
    delete m_pUi->lwArchives->currentItem();
}

void SearchPathsDialog::on_pbRemovePath_clicked( void )
{
    delete m_pUi->lwPaths->currentItem();
}

void SearchPathsDialog::on_pbUp_clicked( void )
{
    int iCurrentRow = m_pUi->lwArchives->currentRow();
    if ( iCurrentRow < 1 )
        return;
    QListWidgetItem * pItem = m_pUi->lwArchives->takeItem( iCurrentRow );
    m_pUi->lwArchives->insertItem( iCurrentRow - 1, pItem );
    m_pUi->lwArchives->setCurrentItem( pItem );
}
