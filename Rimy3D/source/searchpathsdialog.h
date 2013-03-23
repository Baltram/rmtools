#ifndef SEARCHPATHSDIALOG_H_INCLUDED
#define SEARCHPATHSDIALOG_H_INCLUDED

#include <QDialog>

namespace Ui
{
    class SearchPathsDialog;
}

class SearchPathsDialog :
    public QDialog
{
    Q_OBJECT
public:
    explicit SearchPathsDialog( QWidget * a_pParent = 0 );
            ~SearchPathsDialog( void );
public:
    void        addArchives( QStringList const & a_arrArchives );
    void        addPaths( QStringList const & a_arrPaths );
    QStringList getArchives( void );
    QStringList getPaths( void );
protected:
    void addSubDirectories( QStringList & a_arrDirs );
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
private:
    void updateLanguage( void );
private slots:
    void on_lwArchives_currentRowChanged( int a_iCurrentRow );
    void on_lwPaths_currentRowChanged( int a_iCurrentRow );
    void on_pbAddArchive_clicked( void );
    void on_pbAddPath_clicked( void );
    void on_pbDown_clicked();
    void on_pbOk_clicked( void );
    void on_pbRemoveArchive_clicked( void );
    void on_pbRemovePath_clicked( void );
    void on_pbUp_clicked( void );
private:
    Ui::SearchPathsDialog * m_pUi;
};

#endif // SEARCHPATHSDIALOG_H_INCLUDED
