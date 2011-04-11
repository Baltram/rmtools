#include "shortinstruction.h"
#include "ui_shortinstruction.h"

ShortInstruction::ShortInstruction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortInstruction)
{
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    ui->setupUi(this);
}

ShortInstruction::~ShortInstruction()
{
    delete ui;
}

void ShortInstruction::retranslate(void)
{
    ui->retranslateUi(this);
}
