#ifndef EXPORTSETTINGSDIALOG_H_INCLUDED
#define EXPORTSETTINGSDIALOG_H_INCLUDED

#include <QDialog>

class QSettings;

namespace Ui
{
    class exportSettingsDialog;
}

class exportSettingsDialog :
    public QDialog
{
    Q_OBJECT
public:
    enum Options
    {
        None          = 0,
        Colors        = 1 << 0,
        Normals       = 1 << 1,
        NormalsCalc   = 1 << 2,
        NormalsRecalc = 1 << 3,
        VertsOnly     = 1 << 4,
        BaseXact      = 1 << 5,
        BaseXmac      = 1 << 6,
        CreateMtl     = 1 << 7,
        AutoSkin      = 1 << 8,
        Convex        = 1 << 9,
        NoTextures    = 1 << 10,
        ExportSGs     = 1 << 11,
        Gothic3ds     = 1 << 12
    };
    Q_DECLARE_FLAGS( Flags, Options )
public:
    exportSettingsDialog( QWidget * a_pParent, QString a_strExt, Flags a_Flags );
   ~exportSettingsDialog( void );
public:
    int     angle( void ) const;
    bool    anglesNotSgs( void ) const;
    QString baseXact( void ) const;
    QString baseXmac( void ) const;
    bool    colors( void ) const;
    bool    convex( void ) const;
    bool    createMtl( void ) const;
    bool    exportSGs( void ) const;
    bool    gothic3ds( void ) const;
    bool    indirectMatching( void ) const;
    bool    normals( void ) const;
    bool    recalcNormals( void ) const;
    void    setAutoSkinVisible( bool a_bVisible );
    QString textureImageFileExtension( void ) const;
    bool    vertsOnly( void ) const;
protected:
    void changeEvent( QEvent * a_pEvent );
private:
    void updateLanguage( void );
private slots:
    void loadSettings( QSettings & a_Settings );
    void on_cbNormals_toggled( bool a_bIsChecked );
    void on_cbSaveTextures_toggled( bool a_bChecked );
    void on_cbVertsOnly_toggled( bool a_bChecked );
    void on_pbBaseXact_clicked( void );
    void on_pbBaseXmac_clicked( void );
    void on_pbDefault_clicked( void );
    void saveSettings( QSettings & a_Settings ); 
private:
    QString                    m_strExt;
    Ui::exportSettingsDialog * m_pUi;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( exportSettingsDialog::Flags )

#endif // EXPORTSETTINGSDIALOG_H_INCLUDED
