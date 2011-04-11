#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpacerItem>
#include "shapematcombobox.h"
#include "rmXcomFile.h"

class TriangleMeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TriangleMeshWidget(rmXcomFile::TriangleMesh * link, QWidget * parent = 0);
    virtual ~TriangleMeshWidget(void);

	void retranslate(void);

public slots:
    virtual void show(void);

private slots:
    void on_ignoredByTraceRayCheckBox_clicked(bool checked);
    void on_disableCollisionCheckBox_clicked(bool checked);
    void on_disableResponseCheckBox_clicked(bool checked);

private:
    QHBoxLayout * mainLayout;
    QVBoxLayout * propertiesLayout;
    QVBoxLayout * meshInfoLayout;
    QHBoxLayout * shapeMatLayout;

    QLabel * shapeMatLabel;
    ShapeMatComboBox * shapeMatComboBox;

    QCheckBox * ignoredByTraceRayCheckBox;
    QCheckBox * disableCollisionCheckBox;
    QCheckBox * disableResponseCheckBox;

    QGroupBox * propertiesGroup;
    QGroupBox * meshInfoGroup;

    QLabel * meshInfoLabel;

    QSpacerItem * meshInfoVSpacer;

    rmXcomFile::TriangleMesh * linkedTriangleMesh;
};
