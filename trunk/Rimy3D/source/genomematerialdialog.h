#ifndef GENOMEMATERIALDIALOG_H_INCLUDED
#define GENOMEMATERIALDIALOG_H_INCLUDED

#include <QDialog>

class mCGenomeMaterial;
class SceneInfo;
class QGroupBox;

namespace Ui
{
    class GenomeMaterialDialog;
}

class GenomeMaterialDialog :
    public QDialog
{
    Q_OBJECT
public:
    GenomeMaterialDialog( SceneInfo & a_SceneInfo, QWidget * a_pParent = 0 );
   ~GenomeMaterialDialog( void );
public:
    void closeMaterial( void );
    void openMaterial( QString const & a_strMaterialName );
    void updateMaterialSuggestions( void );
protected:
    void changeEvent( QEvent * a_pEvent );
    void closeEvent( QCloseEvent * a_pEvent );
private slots:
    void on_cbMaterialName_textChanged( QString const & a_strText );
    void on_cbProperty_currentIndexChanged( int );
    void on_cbShaderElement_currentIndexChanged( int );
    void on_pbCancel_clicked( void );
    void on_pbOpen_clicked( void );
    void on_pbSaveAs_clicked( void );
    void on_pbSelectMaterial_clicked( void );
private:
    bool getActivePropertyIndex( unsigned int & a_uDest );
    bool getActiveShaderElementIndex( unsigned int & a_uDest );
    void syncProperty( unsigned int a_uShaderElementIndex, unsigned int a_uPropertyIndex, QGroupBox * a_pActiveGroupBox, bool a_bWrite );
    void updateActiveGroupBox( void );
    void updateLanguage( void );
private:
    mCGenomeMaterial *         m_pMaterial;
    SceneInfo &                m_SceneInfo;
    Ui::GenomeMaterialDialog * m_pUi;
};

#endif // GENOMEMATERIALDIALOG_H_INCLUDED
