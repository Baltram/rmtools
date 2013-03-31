#ifndef SCENEINFO_H_INCLUDED
#define SCENEINFO_H_INCLUDED

#include "Mimicry.h"
#include "preferencesdialog.h"
#include <QSettings>

class GLC_World;
class exportSettingsDialog;

class SceneInfo :
    public QObject
{
    Q_OBJECT
public:
    SceneInfo( void );
   ~SceneInfo( void );
public:
    QString &       accessMaterialDir( void );
    GLC_World       buildGlcWorld( void );
    void            clearScene( void );
    QString const & getCurrentDir( void );
    QString const & getCurrentFile( void );
    QString const & getCurrentSaveDir( void );
    mCScene const & getCurrentScene( void );
    bool            openSceneFile( QString a_strFilePath );
    bool            saveSceneFile( QString a_strFilePath, exportSettingsDialog const & a_SettingsDialog );
    bool            sceneContainsUnskinnedMeshes( void );
signals:
    void sceneChanged( void );
public slots:
    void loadSettings( QSettings & a_Settings );
    void lookUpGenomeMaterials( void );
    void saveSettings( QSettings & a_Settings );
private:
    static void errorMessageTranslations( void );
    static void showLastMimicryError( mCError const * a_pLastError, QString a_strTitle );
private:
    QString m_strCurrentDir;
    QString m_strCurrentFile;
    QString m_strCurrentSaveDir;
    QString m_strMaterialDir;
    mCScene m_sceneCurrentScene;
};

#endif  // SCENEINFO_H_INCLUDED
