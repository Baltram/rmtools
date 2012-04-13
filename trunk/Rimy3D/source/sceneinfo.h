#ifndef SCENEINFO_H_INCLUDED
#define SCENEINFO_H_INCLUDED

#include <QSettings>
#include <GLC_World>
#include "Mimicry.h"

class SceneInfo :
    public QObject
{
    Q_OBJECT
public:
    SceneInfo( void );
   ~SceneInfo( void );
public:
    GLC_World       buildGlcWorld( void );
    void            clearScene( void );
    QString const & getCurrentDir( void );
    QString const & getCurrentFile( void );
    mCScene const & getCurrentScene( void );
    bool            openSceneFile( QString a_strFileName );
signals:
    void sceneChanged( void );
public slots:
    void loadSettings( QSettings & a_Settings );
    void saveSettings( QSettings & a_Settings );
private:
    static void errorMessageTranslations( void );
private:
    QString m_strCurrentDir;
    QString m_strCurrentFile;
    mCScene m_sceneCurrentScene;
};

#endif  // SCENEINFO_H_INCLUDED
