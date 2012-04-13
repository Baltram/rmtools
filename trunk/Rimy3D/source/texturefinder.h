#ifndef TEXTUREFINDER_H_INCLUDED
#define TEXTUREFINDER_H_INCLUDED

#include <QObject>

class TextureFinder :
    public QObject
{
    Q_OBJECT
public:
    static TextureFinder & getInstance( void );
public:
    QString findTextureFile( QString const & a_strFilePathGuess );
signals:
public slots:
protected:
    TextureFinder( void );
    TextureFinder( TextureFinder const & );
private:
    static TextureFinder * s_pInstance;
};

#endif // TEXTUREFINDER_H_INCLUDED
