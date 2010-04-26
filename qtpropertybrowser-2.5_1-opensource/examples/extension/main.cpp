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
#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

class VariantManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    VariantManager(QObject *parent = 0);
    ~VariantManager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

public slots:
    virtual void setValue(QtProperty *property, const QVariant &val);
protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
private slots:
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);
private:
    struct Data {
        QVariant value;
        QtVariantProperty *x;
        QtVariantProperty *y;
    };
    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> xToProperty;
    QMap<const QtProperty *, QtProperty *> yToProperty;
};

VariantManager::VariantManager(QObject *parent)
    : QtVariantPropertyManager(parent)
{
    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
                this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

VariantManager::~VariantManager()
{

}

void VariantManager::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (xToProperty.contains(property)) {
        QtProperty *pointProperty = xToProperty[property];
        QVariant v = this->value(pointProperty);
        QPointF p = qVariantValue<QPointF>(v);
        p.setX(qVariantValue<double>(value));
        setValue(pointProperty, p);
    } else if (yToProperty.contains(property)) {
        QtProperty *pointProperty = yToProperty[property];
        QVariant v = this->value(pointProperty);
        QPointF p = qVariantValue<QPointF>(v);
        p.setY(qVariantValue<double>(value));
        setValue(pointProperty, p);
    }
}

void VariantManager::slotPropertyDestroyed(QtProperty *property)
{
    if (xToProperty.contains(property)) {
        QtProperty *pointProperty = xToProperty[property];
        propertyToData[pointProperty].x = 0;
        xToProperty.remove(property);
    } else if (yToProperty.contains(property)) {
        QtProperty *pointProperty = yToProperty[property];
        propertyToData[pointProperty].y = 0;
        yToProperty.remove(property);
    }
}

bool VariantManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == QVariant::PointF)
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

int VariantManager::valueType(int propertyType) const
{
    if (propertyType == QVariant::PointF)
        return QVariant::PointF;
    return QtVariantPropertyManager::valueType(propertyType);
}

QVariant VariantManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property))
        return propertyToData[property].value;
    return QtVariantPropertyManager::value(property);
}

QString VariantManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        QVariant v = propertyToData[property].value;
        QPointF p = qVariantValue<QPointF>(v);
        return QString(tr("(%1, %2)").arg(QString::number(p.x()))
                                 .arg(QString::number(p.y())));
    }
    return QtVariantPropertyManager::valueText(property);
}

void VariantManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.type() != QVariant::PointF && !val.canConvert(QVariant::PointF))
            return;
        QPointF p = qVariantValue<QPointF>(val);
        Data d = propertyToData[property];
        d.value = p;
        if (d.x)
            d.x->setValue(p.x());
        if (d.y)
            d.y->setValue(p.y());
        propertyToData[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, p);
        return;
    }
    QtVariantPropertyManager::setValue(property, val);
}

void VariantManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == QVariant::PointF) {
        Data d;

        d.value = QPointF(0, 0);

        VariantManager *that = (VariantManager *)this;

        d.x = that->addProperty(QVariant::Double);
        d.x->setPropertyName(tr("Position X"));
        property->addSubProperty(d.x);
        xToProperty[d.x] = property;

        d.y = that->addProperty(QVariant::Double);
        d.y->setPropertyName(tr("Position Y"));
        property->addSubProperty(d.y);
        yToProperty[d.y] = property;

        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void VariantManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        Data d = propertyToData[property];
        if (d.x)
            xToProperty.remove(d.x);
        if (d.y)
            yToProperty.remove(d.y);
        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    VariantManager *variantManager = new VariantManager();

    QtVariantProperty *item = variantManager->addProperty(QVariant::PointF,
                "PointF Property");
    item->setValue(QPointF(2.5, 13.13));

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory();

    QtTreePropertyBrowser ed1;
    QtVariantPropertyManager *varMan = variantManager;
    ed1.setFactoryForManager(varMan, variantFactory);
    ed1.addProperty(item);


    ed1.show();

    int ret = app.exec();

    delete variantFactory;
    delete variantManager;

    return ret;
}

#include "main.moc"
