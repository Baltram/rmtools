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
    void addItems( QStringList const & a_arrItems );
protected:
    void addSubDirectories( QStringList & a_arrDirs );
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
private:
    void updateLanguage( void );
private slots:
    void on_listWidget_currentRowChanged( int a_iCurrentRow );
    void on_pushButton_clicked( void );
    void on_pushButton_2_clicked( void );
    void on_pushButton_3_clicked( void );
private:
    Ui::SearchPathsDialog * m_pUi;
};

#endif // SEARCHPATHSDIALOG_H_INCLUDED
