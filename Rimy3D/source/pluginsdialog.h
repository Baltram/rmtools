#ifndef PLUGINSDIALOG_H_INCLUDED
#define PLUGINSDIALOG_H_INCLUDED

#include <QDialog>

namespace Ui
{
    class PluginsDialog;
}

class PluginsDialog :
    public QDialog
{
    Q_OBJECT
public:
    enum Plugin
    {
        GMax,
        Blender
    };
public:
    explicit PluginsDialog( QWidget * a_pParent = 0 );
            ~PluginsDialog( void );
public slots:
    int exec( Plugin a_enuPlugin );
private slots:
    void on_pushButton_clicked( void );
    void on_pushButton_2_clicked( void );
    void on_pushButton_3_clicked( void );
private:
    Ui::PluginsDialog * m_pUi;
};

#endif // PLUGINSDIALOG_H_INCLUDED
