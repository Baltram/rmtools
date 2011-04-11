#pragma once

#include <QComboBox>
#include "rmXcomFile.h"

class ShapeMatComboBox : public QComboBox
{
    Q_OBJECT

public:
    ShapeMatComboBox(QWidget * parent = 0);
    rmXcomFile::ShapeMaterial getShapeMat(void);
    void setShapeMat(rmXcomFile::ShapeMaterial shapeMat);
};
