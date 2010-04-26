/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include <QtGui/QApplication>
#include <QtCore/QMap>
#include <QtGui/QDoubleSpinBox>
#include "qtpropertybrowser.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

class DecoratedDoublePropertyManager : public QtDoublePropertyManager
{
    Q_OBJECT
public:
    DecoratedDoublePropertyManager(QObject *parent = 0);
    ~DecoratedDoublePropertyManager();

    QString prefix(const QtProperty *property) const;
    QString suffix(const QtProperty *property) const;
public Q_SLOTS:
    void setPrefix(QtProperty *property, const QString &prefix);
    void setSuffix(QtProperty *property, const QString &suffix);
Q_SIGNALS:
    void prefixChanged(QtProperty *property, const QString &prefix);
    void suffixChanged(QtProperty *property, const QString &suffix);
protected:
    QString valueText(const QtProperty *property) const;
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
private:
    struct Data {
        QString prefix;
        QString suffix;
    };
    QMap<const QtProperty *, Data> propertyToData;
};

DecoratedDoublePropertyManager::DecoratedDoublePropertyManager(QObject *parent)
    : QtDoublePropertyManager(parent)
{
}

DecoratedDoublePropertyManager::~DecoratedDoublePropertyManager()
{
}

QString DecoratedDoublePropertyManager::prefix(const QtProperty *property) const
{
    if (!propertyToData.contains(property))
        return QString();
    return propertyToData[property].prefix;
}

QString DecoratedDoublePropertyManager::suffix(const QtProperty *property) const
{
    if (!propertyToData.contains(property))
        return QString();
    return propertyToData[property].suffix;
}

void DecoratedDoublePropertyManager::setPrefix(QtProperty *property, const QString &prefix)
{
    if (!propertyToData.contains(property))
        return;

    DecoratedDoublePropertyManager::Data data = propertyToData[property];
    if (data.prefix == prefix)
        return;

    data.prefix = prefix;
    propertyToData[property] = data;

    emit propertyChanged(property);
    emit prefixChanged(property, prefix);
}

void DecoratedDoublePropertyManager::setSuffix(QtProperty *property, const QString &suffix)
{
    if (!propertyToData.contains(property))
        return;

    DecoratedDoublePropertyManager::Data data = propertyToData[property];
    if (data.suffix == suffix)
        return;

    data.suffix = suffix;
    propertyToData[property] = data;

    emit propertyChanged(property);
    emit suffixChanged(property, suffix);
}

QString DecoratedDoublePropertyManager::valueText(const QtProperty *property) const
{
    QString text = QtDoublePropertyManager::valueText(property);
    if (!propertyToData.contains(property))
        return text;

    DecoratedDoublePropertyManager::Data data = propertyToData[property];
    text = data.prefix + text + data.suffix;

    return text;
}

void DecoratedDoublePropertyManager::initializeProperty(QtProperty *property)
{
    propertyToData[property] = DecoratedDoublePropertyManager::Data();
    QtDoublePropertyManager::initializeProperty(property);
}

void DecoratedDoublePropertyManager::uninitializeProperty(QtProperty *property)
{
    propertyToData.remove(property);
    QtDoublePropertyManager::uninitializeProperty(property);
}


class DecoratedDoubleSpinBoxFactory : public QtAbstractEditorFactory<DecoratedDoublePropertyManager>
{
    Q_OBJECT
public:
    DecoratedDoubleSpinBoxFactory(QObject *parent = 0);
    ~DecoratedDoubleSpinBoxFactory();
protected:
    void connectPropertyManager(DecoratedDoublePropertyManager *manager);
    QWidget *createEditor(DecoratedDoublePropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(DecoratedDoublePropertyManager *manager);
private slots:

    void slotPrefixChanged(QtProperty *property, const QString &prefix);
    void slotSuffixChanged(QtProperty *property, const QString &prefix);
    void slotEditorDestroyed(QObject *object);
private:
    /* We delegate responsibilities for QtDoublePropertyManager, which is a base class
       of DecoratedDoublePropertyManager to appropriate QtDoubleSpinBoxFactory */
    QtDoubleSpinBoxFactory *originalFactory;
    QMap<QtProperty *, QList<QDoubleSpinBox *> > createdEditors;
    QMap<QDoubleSpinBox *, QtProperty *> editorToProperty;
};

DecoratedDoubleSpinBoxFactory::DecoratedDoubleSpinBoxFactory(QObject *parent)
    : QtAbstractEditorFactory<DecoratedDoublePropertyManager>(parent)
{
    originalFactory = new QtDoubleSpinBoxFactory(this);
}

DecoratedDoubleSpinBoxFactory::~DecoratedDoubleSpinBoxFactory()
{
    // not need to delete editors because they will be deleted by originalFactory in its destructor
}

void DecoratedDoubleSpinBoxFactory::connectPropertyManager(DecoratedDoublePropertyManager *manager)
{
    originalFactory->addPropertyManager(manager);
    connect(manager, SIGNAL(prefixChanged(QtProperty *, const QString &)), this, SLOT(slotPrefixChanged(QtProperty *, const QString &)));
    connect(manager, SIGNAL(suffixChanged(QtProperty *, const QString &)), this, SLOT(slotSuffixChanged(QtProperty *, const QString &)));
}

QWidget *DecoratedDoubleSpinBoxFactory::createEditor(DecoratedDoublePropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    QtAbstractEditorFactoryBase *base = originalFactory;
    QWidget *w = base->createEditor(property, parent);
    if (!w)
        return 0;

    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(w);
    if (!spinBox)
        return 0;

    spinBox->setPrefix(manager->prefix(property));
    spinBox->setSuffix(manager->suffix(property));

    createdEditors[property].append(spinBox);
    editorToProperty[spinBox] = property;

    return spinBox;
}

void DecoratedDoubleSpinBoxFactory::disconnectPropertyManager(DecoratedDoublePropertyManager *manager)
{
    originalFactory->removePropertyManager(manager);
    disconnect(manager, SIGNAL(prefixChanged(QtProperty *, const QString &)), this, SLOT(slotPrefixChanged(QtProperty *, const QString &)));
    disconnect(manager, SIGNAL(suffixChanged(QtProperty *, const QString &)), this, SLOT(slotSuffixChanged(QtProperty *, const QString &)));
}

void DecoratedDoubleSpinBoxFactory::slotPrefixChanged(QtProperty *property, const QString &prefix)
{
    if (!createdEditors.contains(property))
        return;

    DecoratedDoublePropertyManager *manager = propertyManager(property);
    if (!manager)
        return;

    QList<QDoubleSpinBox *> editors = createdEditors[property];
    QListIterator<QDoubleSpinBox *> itEditor(editors);
    while (itEditor.hasNext()) {
        QDoubleSpinBox *editor = itEditor.next();
        editor->setPrefix(prefix);
    }
}

void DecoratedDoubleSpinBoxFactory::slotSuffixChanged(QtProperty *property, const QString &prefix)
{
    if (!createdEditors.contains(property))
        return;

    DecoratedDoublePropertyManager *manager = propertyManager(property);
    if (!manager)
        return;

    QList<QDoubleSpinBox *> editors = createdEditors[property];
    QListIterator<QDoubleSpinBox *> itEditor(editors);
    while (itEditor.hasNext()) {
        QDoubleSpinBox *editor = itEditor.next();
        editor->setSuffix(prefix);
    }
}

void DecoratedDoubleSpinBoxFactory::slotEditorDestroyed(QObject *object)
{
    QMap<QDoubleSpinBox *, QtProperty *>::ConstIterator itEditor =
                editorToProperty.constBegin();
    while (itEditor != editorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            QDoubleSpinBox *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            editorToProperty.remove(editor);
            createdEditors[property].removeAll(editor);
            if (createdEditors[property].isEmpty())
                createdEditors.remove(property);
            return;
        }
        itEditor++;
    }
}


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QtDoublePropertyManager *undecoratedManager = new QtDoublePropertyManager();
    QtProperty *undecoratedProperty = undecoratedManager->addProperty("Undecorated");
    undecoratedManager->setValue(undecoratedProperty, 123.45);

    DecoratedDoublePropertyManager *decoratedManager = new DecoratedDoublePropertyManager();
    QtProperty *decoratedProperty = decoratedManager->addProperty("Decorated");
    decoratedManager->setPrefix(decoratedProperty, "speed: ");
    decoratedManager->setSuffix(decoratedProperty, " km/h");
    decoratedManager->setValue(decoratedProperty, 123.45);

    QtDoubleSpinBoxFactory *undecoratedFactory = new QtDoubleSpinBoxFactory();
    DecoratedDoubleSpinBoxFactory *decoratedFactory = new DecoratedDoubleSpinBoxFactory();

    QtTreePropertyBrowser *editor = new QtTreePropertyBrowser();
    editor->setFactoryForManager(undecoratedManager, undecoratedFactory);
    editor->setFactoryForManager(decoratedManager, decoratedFactory);
    editor->addProperty(undecoratedProperty);
    editor->addProperty(decoratedProperty);
    editor->show();

    int ret = app.exec();

    delete decoratedFactory;
    delete decoratedManager;
    delete undecoratedFactory;
    delete undecoratedManager;
    delete editor;

    return ret;
}

#include "main.moc"
