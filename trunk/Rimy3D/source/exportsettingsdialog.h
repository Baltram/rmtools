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
        CreateMtl     = 1 << 6,
        AutoSkin      = 1 << 7
    };
    Q_DECLARE_FLAGS( Flags, Options )
public:
    exportSettingsDialog( QWidget * a_pParent, QString a_strExt, Flags a_Flags );
   ~exportSettingsDialog( void );
public:
    int     angle( void ) const;
    bool    anglesNotSgs( void ) const;
    QString baseXact( void ) const;
    bool    colors( void ) const;
    bool    createMtl( void ) const;
    bool    indirectMatching( void ) const;
    bool    normals( void ) const;
    bool    recalcNormals( void ) const;
    void    setAutoSkinEnabled( bool a_bEnabled );
    bool    vertsOnly( void ) const;
protected:
    void changeEvent( QEvent * a_pEvent );
private:
    void updateLanguage( void );
private slots:
    void loadSettings( QSettings & a_Settings );
    void on_cbNormals_toggled( bool a_bIsChecked );
    void on_pbBaseXact_clicked( void );
    void saveSettings( QSettings & a_Settings );
private:
    QString                    m_strExt;
    Ui::exportSettingsDialog * m_pUi;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( exportSettingsDialog::Flags )

#endif // EXPORTSETTINGSDIALOG_H_INCLUDED
