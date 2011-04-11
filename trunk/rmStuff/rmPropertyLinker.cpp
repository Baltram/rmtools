#include "rmPropertyLinker.h"

#ifndef RM_QTPROPERTYBROWSER

void rmPropertyLinker::assingAsProperty(void * src, string name, rmBool enabled) {}
void rmPropertyLinker::assingAsProperty(void ** src, string name, rmBool enabled) {}
void rmPropertyLinker::assingAsProperty(string name, rmBool enabled) {}

#else

rmOut rmPropertyLinker::out("rmPropertyLinker.cpp");

QtSpinBoxFactory * rmPropertyLinker::spinBoxFactory = new QtSpinBoxFactory;
QtLineEditFactory * rmPropertyLinker::lineEditFactory = new QtLineEditFactory;
QtCheckBoxFactory * rmPropertyLinker::checkBoxFactory = new QtCheckBoxFactory;
QtDoubleSpinBoxFactory * rmPropertyLinker::doubleSpinBoxFactory = new QtDoubleSpinBoxFactory;
QtEnumEditorFactory * rmPropertyLinker::enumEditorFactory = new QtEnumEditorFactory;

rmPropertyLinker * rmPropertyLinker::currentLinker = new rmPropertyLinker;
list<QtProperty *> rmPropertyLinker::currentProperties;

rmBool rmPropertyLinker::lessThan(const PropertyLink & l1, const PropertyLink & l2)
{
    return (l1.property < l2.property);
}

rmPropertyLinker::rmPropertyLinker(void)
{
    groupPropertyManager = new QtGroupPropertyManager;
    intPropertyManager = new QtIntPropertyManager;
    stringPropertyManager = new QtStringPropertyManager;
    dateTimePropertyManager = new QtDateTimePropertyManager;
    boolPropertyManager = new QtBoolPropertyManager;
    doublePropertyManager = new QtDoublePropertyManager;
    guidPropertyManager = new QtGuidPropertyManager;
    enumPropertyManager = new QtEnumPropertyManager;

    associatedPropertyBrowser = 0;
}

rmPropertyLinker::~rmPropertyLinker(void)
{
    //resetAssociatedPropertyBrowser(); // What if the associated browser has been deleted previously?
    delete groupPropertyManager;
    delete intPropertyManager;
    delete stringPropertyManager;
    delete dateTimePropertyManager;
    delete boolPropertyManager;
    delete doublePropertyManager;
    delete guidPropertyManager;
    delete enumPropertyManager;
}

void rmPropertyLinker::setUpAsCurrentLinker(void)
{
    rmPropertyLinker::currentLinker = this;
}

void * rmPropertyLinker::getPropertyData(QtProperty * p)
{
    if (lastPropLink.property == p) {
        return lastPropLink.data;
    }
    PropertyLink pl;
    pl.property = p;
    QList<PropertyLink>::iterator pli = qBinaryFind(propertyLinks.begin(), (propertyLinks.end() - nNewProperties), pl, &rmPropertyLinker::lessThan);
    lastPropLink.property = pli->property;
    lastPropLink.data = pli->data;
    return pli->data;
}

void rmPropertyLinker::setUpProperty(QtProperty * p, void * data, rmBool enabled)
{
    if (data != 0) {
        p->setEnabled(enabled);
    }
    ++currentLinker->nNewProperties;
    currentLinker->propertyLinks.push_back(PropertyLink());
    PropertyLink & pl = currentLinker->propertyLinks.back();
    pl.property = p;
    pl.data = data;
    if (currentProperties.size() != 0) {
        currentProperties.back()->addSubProperty(p);
    }
    else {
        currentLinker->topLevelProperties.push_back(p);
    }
}

void rmPropertyLinker::assignToPropertyBrowser(QtAbstractPropertyBrowser * dest)
{
    if (nNewProperties != 0) {
        qSort(propertyLinks.begin(), propertyLinks.end(), &rmPropertyLinker::lessThan);
        nNewProperties = 0;
    }

    dest->clear();

    dest->setFactoryForManager(intPropertyManager, spinBoxFactory);
    connect(intPropertyManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(setValue(QtProperty *, int)));

    dest->setFactoryForManager(stringPropertyManager, lineEditFactory);
    connect(stringPropertyManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(setValue(QtProperty *, const QString &)));

    dest->setFactoryForManager(boolPropertyManager, checkBoxFactory);
    connect(boolPropertyManager, SIGNAL(valueChanged(QtProperty *, bool)), this, SLOT(setValue(QtProperty *, bool)));

    dest->setFactoryForManager(doublePropertyManager, doubleSpinBoxFactory);
    connect(doublePropertyManager, SIGNAL(valueChanged(QtProperty *, double)), this, SLOT(setValue(QtProperty *, double)));

    dest->setFactoryForManager(dynamic_cast<QtStringPropertyManager *>(guidPropertyManager), lineEditFactory);
    connect(guidPropertyManager, SIGNAL(valueChanged(QtProperty *, const QString &)), this, SLOT(setGuidValue(QtProperty *, const QString &)));

    dest->setFactoryForManager(enumPropertyManager, enumEditorFactory);
    connect(enumPropertyManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(setValue(QtProperty *, int)));

    for (rmU32 i = 0, ie = topLevelProperties.size(); i != ie; ++i) {
        dest->addProperty(topLevelProperties[i]);
    }
}

void rmPropertyLinker::resetAssociatedPropertyBrowser(void)
{
    if (associatedPropertyBrowser != 0) {
        associatedPropertyBrowser->unsetFactoryForManager(intPropertyManager);
        associatedPropertyBrowser->unsetFactoryForManager(stringPropertyManager);
        associatedPropertyBrowser->unsetFactoryForManager(boolPropertyManager);
        associatedPropertyBrowser->unsetFactoryForManager(doublePropertyManager);
        associatedPropertyBrowser->unsetFactoryForManager(guidPropertyManager);
        associatedPropertyBrowser->unsetFactoryForManager(enumPropertyManager);
        associatedPropertyBrowser->clear();
    }
}

const QtStringPropertyManager * rmPropertyLinker::getStringPropertyManager(void)
{
	return stringPropertyManager;
}

void rmPropertyLinker::setValue(QtProperty * property, int value)
{
    if (property->propertyManager() != intPropertyManager) {
        if (property->propertyManager() == enumPropertyManager) {
            *reinterpret_cast<rmU32 *>(getPropertyData(property)) = value;
        }
        emit modified();
        return;
    }

    rmU32 max = intPropertyManager->maximum(property);
    switch (max)
    {
    case 255:
        {
            *reinterpret_cast<rmU8 *>(getPropertyData(property)) = value;
            break;
        }
    case 65535:
        {
            *reinterpret_cast<rmU16 *>(getPropertyData(property)) = value;
            break;
        }
    case 2147483647:
        {
            *reinterpret_cast<rmU32 *>(getPropertyData(property)) = value;
            break;
        }
    default:
        {
            OUT_SERR("Range not supported")
        }
    }
    emit modified();
}

void rmPropertyLinker::setValue(QtProperty * property, const QString & value)
{
    *reinterpret_cast<string *>(getPropertyData(property)) = string(value.toAscii());
    emit modified();
}

void rmPropertyLinker::setGuidValue(QtProperty * property, const QString & value)
{
    reinterpret_cast<rmGuid *>(getPropertyData(property))->fromText(string(value.toAscii()));
    emit modified();
}

void rmPropertyLinker::setValue(QtProperty * property, bool value)
{
    *reinterpret_cast<rmBool *>(getPropertyData(property)) = value;
    emit modified();
}

void rmPropertyLinker::setValue(QtProperty * property, double value)
{
    *reinterpret_cast<rmF32 *>(getPropertyData(property)) = value;
    emit modified();
}

void rmPropertyLinker::startAddingSubProperties(void)
{
    if (currentLinker == 0) {
        return;
    }
    if (currentLinker->propertyLinks.size() != 0) {
        currentProperties.push_back(currentLinker->propertyLinks.back().property);
    }
}

void rmPropertyLinker::endAddingSubProperties(void)
{
    if (currentProperties.size() != 0) {
        currentProperties.pop_back();
    }
}

void rmPropertyLinker::assingAsProperty(rmDummyClass::eCColorScale::ColorScaleItem * src, string name, rmBool enabled)
{
    assingAsProperty(name);
    startAddingSubProperties();

    assingAsProperty(&src->Time, "Time");
    assingAsProperty(&src->Red, "Red");
    assingAsProperty(&src->Green, "Green");
    assingAsProperty(&src->Blue, "Blue");
    assingAsProperty(&src->Alpha, "Alpha");

    endAddingSubProperties();
}

void rmPropertyLinker::assingAsProperty(rmDummyClass::EnumValue ** src, string name, rmBool enabled)
{
    assingAsProperty(*src, name, enabled);
}

void rmPropertyLinker::assingAsProperty(rmDummyClass::EnumValue * src, string name, rmBool enabled)
{
    rmU32 iEnum = findCStringInArray(src->getClassName().c_str(), rmDummyClass::DummyClassManager::enumNames, rmDummyClass::DummyClassManager::NEnums);
    if (iEnum != (-1)) {
        rmU32 nEnums = rmDummyClass::DummyClassManager::enumStringListSizes[iEnum];
        if (src->getValue() < nEnums) {
            QStringList enumValueStrings;
            for (rmU32 i = 0; i != nEnums; ++i) {
                enumValueStrings << rmDummyClass::DummyClassManager::enumValueStrings[iEnum][i];
            }

            QtProperty * p = currentLinker->enumPropertyManager->addProperty(QString(name.c_str()));
            currentLinker->enumPropertyManager->setEnumNames(p, enumValueStrings);
            currentLinker->enumPropertyManager->setValue(p, src->getValue());
            setUpProperty(p, &src->getValue(), enabled);
            return;
        }
    }
    assingAsProperty(&src->getValue(), name, enabled);
}

void rmPropertyLinker::assingAsProperty(rmGuid * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->guidPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->guidPropertyManager->setValue(p, src->toText().c_str());
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmF32 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->doublePropertyManager->addProperty(QString(name.c_str()));
    currentLinker->doublePropertyManager->setValue(p, *src);
    currentLinker->doublePropertyManager->setDecimals(p, 8);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmBool * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->boolPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->boolPropertyManager->setValue(p, *src);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmU8 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->intPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->intPropertyManager->setValue(p, *src);
    currentLinker->intPropertyManager->setRange(p, 0, 255);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmU16 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->intPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->intPropertyManager->setValue(p, *src);
    currentLinker->intPropertyManager->setRange(p, 0, 65535);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmU32 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->intPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->intPropertyManager->setValue(p, *src);
    currentLinker->intPropertyManager->setRange(p, 0, 2147483647);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmI32 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->intPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->intPropertyManager->setValue(p, *src);
    currentLinker->intPropertyManager->setRange(p, -2147483648, 2147483647);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(string * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->stringPropertyManager->addProperty(QString(name.c_str()));
    currentLinker->stringPropertyManager->setValue(p, QString(src->c_str()));
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmU64 * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->dateTimePropertyManager->addProperty(QString(name.c_str()));
    QDateTime t;
    t.setTime_t((*src - 116444736000000000) / 10000000);
    currentLinker->dateTimePropertyManager->setValue(p, t);
    setUpProperty(p, src, enabled);
}

void rmPropertyLinker::assingAsProperty(rmDummyClass::WrittenClassBase * src, string name, rmBool enabled)
{
    if (src->doSpecialPropAssign(name) == true) {
        return;
    }
    assingAsProperty(name, enabled);
    startAddingSubProperties();
    src->assignVarsAsProps();
    endAddingSubProperties();
}

void rmPropertyLinker::assingAsProperty(rmDummyClass::WrittenClassBase ** src, string name, rmBool enabled)
{
    assingAsProperty(*src, name, enabled);
}

void rmPropertyLinker::assingAsProperty(string name, rmBool enabled)
{
    QtProperty * p = currentLinker->groupPropertyManager->addProperty(QString(name.c_str()));
    p->setEnabled(enabled);
    setUpProperty(p);
}

void rmPropertyLinker::assingAsProperty(void * src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->groupPropertyManager->addProperty(QString(name.c_str()));
    setUpProperty(p);
}

void rmPropertyLinker::assingAsProperty(void ** src, string name, rmBool enabled)
{
    QtProperty * p = currentLinker->groupPropertyManager->addProperty(QString(name.c_str()));
    setUpProperty(p);
}

#endif // RM_QTPROPERTYBROWSER.
