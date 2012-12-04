#ifndef EXTENDEDSAVINGDIALOG_H_INCLUDED
#define EXTENDEDSAVINGDIALOG_H_INCLUDED

#include <QDialog>

namespace Ui
{
    class ExtendedSavingDialog;
}

class ExtendedSavingDialog :
    public QDialog
{
    Q_OBJECT
public:
    explicit ExtendedSavingDialog( QWidget * a_pParent = 0 );
            ~ExtendedSavingDialog( void );
private slots:
    void on_pushButton_clicked( void );
    void on_pushButton_2_clicked( void );
private:
    Ui::ExtendedSavingDialog * m_pUi;
};

#endif // EXTENDEDSAVINGDIALOG_H_INCLUDED
