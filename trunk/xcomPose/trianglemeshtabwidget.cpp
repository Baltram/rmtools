#include "trianglemeshtabwidget.h"

TriangleMeshTabWidget::TriangleMeshTabWidget(QWidget * parent) : QTabWidget(parent)
{
    xcom = 0;
    reset();
}

TriangleMeshTabWidget::~TriangleMeshTabWidget(void)
{
    freeMemory();
}

void TriangleMeshTabWidget::retranslate(void)
{
	for (rmU32 i = 0, ie = count(); i != ie; ++i) {
		try {
			dynamic_cast<TriangleMeshWidget *>(widget(i))->retranslate();
		}
		catch (exception & e) {
		}
	}
}

void TriangleMeshTabWidget::freeMemory(void)
{
    for (rmU32 i = this->count(), ie = 0; i != ie; --i) {
	delete widget(i - 1);
    }
    delete xcom;
    xcom = 0;
}

void TriangleMeshTabWidget::reset()
{
    freeMemory();
    xcom = new rmXcomFile;
    this->addTab(new TriangleMeshWidget(0, this), "");
}

void TriangleMeshTabWidget::add(const rmMesh * src)
{
    rmXcomFile::TriangleMesh * tm = xcom->addTriangleMesh();
    tm->mesh = *src;
    this->addTab(new TriangleMeshWidget(tm, this), "");
    removePlaceholder();
    renameTabs();
}

void TriangleMeshTabWidget::remove(rmU32 index)
{
    if (index >= count()) {
	return;
    }
    delete widget(index);
    xcom->removeTriangleMesh(index);
    if (count() == 0) {
	reset();
    }
    renameTabs();
}

rmBool TriangleMeshTabWidget::hasContents(void)
{
    return (xcom->getNumTriangleMeshes() != 0);
}

void TriangleMeshTabWidget::getMesh(rmU32 index, rmMesh & dest)
{
    if (index >= count()) {
	return;
    }
    dest = xcom->getTriangleMesh(index)->mesh;
}

QString TriangleMeshTabWidget::getMaterialString(rmU32 index)
{
    if (index >= count()) {
	return "";
    }
    return QString(rmXcomFile::shapeMaterialNames[xcom->getTriangleMesh(index)->shapeMaterial]);
}

rmBool TriangleMeshTabWidget::fromFile(string filePath)
{
    reset();
    if (!xcom->read(filePath)) {
	return false;
    }

    for (rmU32 i = 0, ie = xcom->getNumTriangleMeshes(); i != ie; ++i) {
	this->addTab(new TriangleMeshWidget(xcom->getTriangleMesh(i), this), "");
    }

    removePlaceholder();
    renameTabs();
    return true;
}

rmBool TriangleMeshTabWidget::toFile(string filePath)
{
    return xcom->write(filePath);
}

void TriangleMeshTabWidget::removePlaceholder(void)
{
    if (this->count() > 1) {
	if (tabText(0) == "")
	{
	    delete widget(0);
	}
    }
}

void TriangleMeshTabWidget::renameTabs(void)
{
    if (!hasContents()) {
	return;
    }
    for (rmU32 i = 0, ie = count(); i != ie; ++i) {
	setTabText(i, QVariant(i + 1).toString());
    }
}
