#ifndef TEXTUREFINDER_H_INCLUDED
#define TEXTUREFINDER_H_INCLUDED

#include "searchpathsdialog.h"
#include <QObject>

class QSettings;

class TextureFinder :
    public QObject
{
    Q_OBJECT
public:
    static TextureFinder & getInstance( void );
public:
    void    addSearchPath( QString const & a_strPath );
    QString findTextureFile( QString const & a_strFilePathGuess, QString const & a_strCurrentDir );
    int     getNumSearchPaths( void );
    void    removeSearchPathAt( int a_iPosition );
    void    showDialog( void );
    void    updateSearchPaths( void );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    TextureFinder( void );
    TextureFinder( TextureFinder const & );
private:
    static TextureFinder * s_pInstance;
private:
    SearchPathsDialog m_Dialog;
    QStringList       m_arrPaths;
};

#endif // TEXTUREFINDER_H_INCLUDED
