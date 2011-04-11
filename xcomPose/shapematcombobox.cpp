#include "shapematcombobox.h"

ShapeMatComboBox::ShapeMatComboBox(QWidget * parent) : QComboBox(parent)
{
    QStringList shapeMatNames;
    for (rmU32 i = 0; i != rmXcomFile::NShapeMaterials; ++i) {
	shapeMatNames << rmXcomFile::shapeMaterialNames[i];
    }
    addItems(shapeMatNames);
}

rmXcomFile::ShapeMaterial ShapeMatComboBox::getShapeMat(void)
{
    return static_cast<rmXcomFile::ShapeMaterial>(currentIndex());
}

void ShapeMatComboBox::setShapeMat(rmXcomFile::ShapeMaterial shapeMat)
{
    this->setCurrentIndex(shapeMat);
}
