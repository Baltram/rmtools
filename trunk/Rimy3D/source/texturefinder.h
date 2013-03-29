#ifndef TEXTUREFINDER_H_INCLUDED
#define TEXTUREFINDER_H_INCLUDED

#include "searchpathsdialog.h"
#include <QHash>

class mCGenomeVolume;

class QSettings;

class TextureFinder :
    public QObject
{
    Q_OBJECT
public:
    static TextureFinder & getInstance( void );
public:
    void findFile( char const * a_pcFileName, char const * a_pcFileExt, QByteArray * a_pDest );
    bool findTexture( QString const & a_strFilePathGuess, QString const & a_strCurrentDir, QImage & a_imgDest, bool a_bExtensiveSearch );
    void showDialog( void );
    void updateArchives( void );
    void updateSearchPaths( void );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
protected:
    TextureFinder( void );
    TextureFinder( TextureFinder const & );
private:
    static TextureFinder * s_pInstance;
private:
    SearchPathsDialog                  m_Dialog;
    QHash< QString, mCGenomeVolume * > m_mapArchives;
};

#endif // TEXTUREFINDER_H_INCLUDED
