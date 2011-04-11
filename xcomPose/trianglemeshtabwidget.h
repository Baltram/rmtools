#pragma once

#include <QTabWidget>
#include "rmMesh.h"
#include "trianglemeshwidget.h"

class TriangleMeshTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TriangleMeshTabWidget(QWidget * parent = 0);
    virtual ~TriangleMeshTabWidget(void);

	void retranslate(void);

    void reset(void);
    void add(const rmMesh * src);
    void remove(rmU32 index);

    rmBool hasContents(void);

    void getMesh(rmU32 index, rmMesh & dest);
    QString getMaterialString(rmU32 index);

    rmBool fromFile(string filePath);
    rmBool toFile(string filePath);

private:
    void removePlaceholder(void);
    void freeMemory(void);
    void renameTabs(void);

    rmXcomFile * xcom;
};
