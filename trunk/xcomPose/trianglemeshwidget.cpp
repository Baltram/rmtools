#include "trianglemeshwidget.h"

TriangleMeshWidget::TriangleMeshWidget(rmXcomFile::TriangleMesh * link, QWidget * parent) : QWidget(parent)
{
    if (link == 0) {
	link = new rmXcomFile::TriangleMesh;
	link->disableCollision = false;
	link->disableResponse = false;
	link->ignoredByTraceRay = false;
	link->shapeMaterial = static_cast<rmXcomFile::ShapeMaterial>(666);
    }
    linkedTriangleMesh = link;

    delete this->layout();
    mainLayout = new QHBoxLayout(this);
    propertiesLayout = new QVBoxLayout();
    meshInfoLayout = new QVBoxLayout();
    shapeMatLayout = new QHBoxLayout();
    propertiesLayout->addLayout(shapeMatLayout);

    meshInfoGroup = new QGroupBox(this);
    meshInfoGroup->setLayout(meshInfoLayout);
    mainLayout->addWidget(meshInfoGroup);

    meshInfoLabel = new QLabel(meshInfoGroup);
    meshInfoLayout->addWidget(meshInfoLabel);
    meshInfoVSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    meshInfoLayout->addItem(meshInfoVSpacer);

    propertiesGroup = new QGroupBox(this);
    propertiesGroup->setLayout(propertiesLayout);
    propertiesGroup->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    mainLayout->addWidget(propertiesGroup);

    shapeMatLabel = new QLabel("ShapeMaterial:", this);
    shapeMatLayout->addWidget(shapeMatLabel);
    shapeMatComboBox = new ShapeMatComboBox(this);
    shapeMatComboBox->setShapeMat(linkedTriangleMesh->shapeMaterial);
    shapeMatComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    shapeMatLayout->addWidget(shapeMatComboBox);

    ignoredByTraceRayCheckBox = new QCheckBox("IgnoredByTraceRay", propertiesGroup);
    ignoredByTraceRayCheckBox->setChecked(linkedTriangleMesh->ignoredByTraceRay);
    propertiesLayout->addWidget(ignoredByTraceRayCheckBox);

    disableCollisionCheckBox = new QCheckBox("DisableCollision", propertiesGroup);
    disableCollisionCheckBox->setChecked(linkedTriangleMesh->disableCollision);
    propertiesLayout->addWidget(disableCollisionCheckBox);

    disableResponseCheckBox = new QCheckBox("DisableResponse", propertiesGroup);
    disableResponseCheckBox->setChecked(linkedTriangleMesh->disableResponse);
    propertiesLayout->addWidget(disableResponseCheckBox);

    if (link->shapeMaterial != 666) {
	connect(ignoredByTraceRayCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_ignoredByTraceRayCheckBox_clicked(bool)));
	connect(disableCollisionCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_disableCollisionCheckBox_clicked(bool)));
	connect(disableResponseCheckBox, SIGNAL(clicked(bool)), this, SLOT(on_disableResponseCheckBox_clicked(bool)));
	connect(shapeMatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_shapeMatComboBox_currentIndexChanged(int)));
    }
    else {
	setEnabled(false);
    }

    retranslate();
}

TriangleMeshWidget::~TriangleMeshWidget(void)
{
    if (linkedTriangleMesh->shapeMaterial == 666) {
	delete linkedTriangleMesh;
    }
}

void TriangleMeshWidget::show(void)
{
    retranslate();
    QWidget::show();
}

void TriangleMeshWidget::retranslate(void)
{
    propertiesGroup->setTitle(tr("Properties"));

    meshInfoGroup->setTitle(tr("Mesh"));

    rmMesh & m = linkedTriangleMesh->mesh;
    rmBox extents = m.calcExtents();
    rmVec3 size = extents.max - extents.min;
    meshInfoLabel->setText(tr("Vertex count: %1\nTriangle count: %2\n\nLength: %3\nWidth: %4\nHeight: %5")
			   .arg(m.getNumVerts())
			   .arg(m.getNumFaces())
			   .arg(size.x)
			   .arg(size.y)
			   .arg(size.z));
}

void TriangleMeshWidget::on_ignoredByTraceRayCheckBox_clicked(bool checked)
{
    linkedTriangleMesh->ignoredByTraceRay = checked;
}

void TriangleMeshWidget::on_disableCollisionCheckBox_clicked(bool checked)
{
    linkedTriangleMesh->disableCollision = checked;
}

void TriangleMeshWidget::on_disableResponseCheckBox_clicked(bool checked)
{
    linkedTriangleMesh->disableResponse = checked;
}

void TriangleMeshWidget::on_shapeMatComboBox_currentIndexChanged(int index)
{
	index;
	linkedTriangleMesh->shapeMaterial = shapeMatComboBox->getShapeMat();
}
