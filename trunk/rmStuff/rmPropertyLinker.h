#pragma once

#include "rmDummyClass.h"

#ifdef RM_QTPROPERTYBROWSER

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

#include <QList>
#include <QtAlgorithms>
#include <QDateTime>

#endif // RM_QTPROPERTYBROWSER

#ifdef RM_QTPROPERTYBROWSER

class rmPropertyLinker : public QObject
{
    Q_OBJECT

#else

class rmPropertyLinker
{

#endif // RM_QTPROPERTYBROWSER

public:
    static void assingAsProperty(void * src, string name, rmBool enabled = true);
    static void assingAsProperty(void ** src, string name, rmBool enabled = true);
    static void assingAsProperty(string name, rmBool enabled = true);

#ifdef RM_QTPROPERTYBROWSER

private:
    struct PropertyLink
    {
        QtProperty * property;
        void * data;
    };

public:
    rmPropertyLinker(void);
    ~rmPropertyLinker(void);

    void setUpAsCurrentLinker(void);
    void assignToPropertyBrowser(QtAbstractPropertyBrowser * dest);
    void resetAssociatedPropertyBrowser(void);

	const QtStringPropertyManager * getStringPropertyManager(void);

    static void startAddingSubProperties(void);
    static void endAddingSubProperties(void);

    static void assingAsProperty(rmDummyClass::eCColorScale::ColorScaleItem * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmDummyClass::EnumValue * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmDummyClass::EnumValue ** src, string name, rmBool enabled = true);
    static void assingAsProperty(rmGuid * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmF32 * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmBool * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmU8 * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmU16 * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmU32 * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmI32 * src, string name, rmBool enabled = true);
    static void assingAsProperty(string * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmU64 * src, string name, rmBool enabled = true); // Taken for FILETIME.
    static void assingAsProperty(rmDummyClass::WrittenClassBase * src, string name, rmBool enabled = true);
    static void assingAsProperty(rmDummyClass::WrittenClassBase ** src, string name, rmBool enabled = true);
    template <class T>
    static void assingAsProperty(list<T> * src, string name)
    {
        rmChar buffer[20];
        assingAsProperty(name + "[" + _itoa(src->size(), buffer, 10) + "]");
        startAddingSubProperties();
        rmU32 j = 0;
        for (list<T>::iterator i = src->begin(), ie = src->end(); i != ie; ++i) {
            assingAsProperty(&(*i), name + "[" + _itoa(j++, buffer, 10) + "]");
        }
        endAddingSubProperties();
    }
    template <class T>
    static void assingAsProperty(list<T *> * src, string name)
    {
        rmChar buffer[20];
        assingAsProperty(name + "[" + _itoa(src->size(), buffer, 10) + "]");
        startAddingSubProperties();
        rmU32 j = 0;
        for (list<T *>::iterator i = src->begin(), ie = src->end(); i != ie; ++i) {
            assingAsProperty((*i), name + "[" + _itoa(j++, buffer, 10) + "]");
        }
        endAddingSubProperties();
    }
private slots:
     void setValue(QtProperty * property, int value);
     void setValue(QtProperty * property, const QString & value);
     void setGuidValue(QtProperty * property, const QString & value);
     void setValue(QtProperty * property, bool value);
     void setValue(QtProperty * property, double value);

signals:
    void modified(void);

private:
    static rmBool lessThan(const PropertyLink & l1, const PropertyLink & l2);

    static void setUpProperty(QtProperty * p, void * data = 0, rmBool enabled = true);

    void * getPropertyData(QtProperty * p);

    QtAbstractPropertyBrowser * associatedPropertyBrowser;

    QList<QtProperty *> topLevelProperties;
    QList<PropertyLink> propertyLinks;

    rmU32 nNewProperties;

    QtGroupPropertyManager * groupPropertyManager;
    QtIntPropertyManager * intPropertyManager;
    QtStringPropertyManager * stringPropertyManager;
    QtDateTimePropertyManager * dateTimePropertyManager;
    QtBoolPropertyManager * boolPropertyManager;
    QtDoublePropertyManager * doublePropertyManager;
    QtGuidPropertyManager * guidPropertyManager;
    QtEnumPropertyManager * enumPropertyManager;

    PropertyLink lastPropLink;

    static QtSpinBoxFactory * spinBoxFactory;
    static QtLineEditFactory * lineEditFactory;
    static QtCheckBoxFactory * checkBoxFactory;
    static QtDoubleSpinBoxFactory * doubleSpinBoxFactory;
    static QtEnumEditorFactory * enumEditorFactory;

    static rmPropertyLinker * currentLinker;
    static list<QtProperty *> currentProperties;

    static rmOut out;

#endif // RM_QTPROPERTYBROWSER.

};
