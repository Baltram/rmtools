#ifndef SHORTINSTRUCTION_H
#define SHORTINSTRUCTION_H

#include <QDialog>

namespace Ui {
    class ShortInstruction;
}

class ShortInstruction : public QDialog
{
    Q_OBJECT

public:
    explicit ShortInstruction(QWidget *parent = 0);
    ~ShortInstruction();

    void retranslate(void);

private:
    Ui::ShortInstruction *ui;
};

#endif // SHORTINSTRUCTION_H
