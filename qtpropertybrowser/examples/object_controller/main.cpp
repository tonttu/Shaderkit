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
#include <QtGui/QSpinBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLineEdit>
#include <QtGui/QDialog>
#include <QtGui/QComboBox>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QBoxLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QAction>
#include <QtGui/QDesktopWidget>
#include <QtGui/QTextDocument>
#include <QtGui/QCalendarWidget>
#include <QtCore/QTimeLine>
#include "objectcontroller.h"

class MyController : public QDialog
{
    Q_OBJECT
public:
    MyController(QWidget *parent = 0);
    ~MyController();
private slots:
    void createAndControl();
private:
    QComboBox *theClassCombo;
    ObjectController *theController;
    QStringList theClassNames;
    QObject *theControlledObject;
};

MyController::MyController(QWidget *parent)
    : QDialog(parent), theControlledObject(0)
{
    theClassCombo = new QComboBox(this);
    QToolButton *button = new QToolButton(this);
    theController = new ObjectController(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);

    connect(button, SIGNAL(clicked()), this, SLOT(createAndControl()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    button->setText(tr("Create And Control"));
    buttonBox->setStandardButtons(QDialogButtonBox::Close);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *internalLayout = new QHBoxLayout();
    internalLayout->addWidget(theClassCombo);
    internalLayout->addWidget(button);
    layout->addLayout(internalLayout);
    layout->addWidget(theController);
    layout->addWidget(buttonBox);

    theClassNames.append(QLatin1String("QWidget"));
    theClassNames.append(QLatin1String("QPushButton"));
    theClassNames.append(QLatin1String("QDialogButtonBox"));
    theClassNames.append(QLatin1String("QTreeWidget"));
    theClassNames.append(QLatin1String("QCalendarWidget"));
    theClassNames.append(QLatin1String("QAction"));
    theClassNames.append(QLatin1String("QTimeLine"));
    theClassNames.append(QLatin1String("QTextDocument"));

    theClassCombo->addItems(theClassNames);
}

MyController::~MyController()
{
    if (theControlledObject)
        delete theControlledObject;
}

void MyController::createAndControl()
{
    QObject *newObject = 0;
    QString className = theClassNames.at(theClassCombo->currentIndex());
    if (className == QLatin1String("QWidget"))
        newObject = new QWidget();
    else if (className == QLatin1String("QPushButton"))
        newObject = new QPushButton();
    else if (className == QLatin1String("QDialogButtonBox"))
        newObject = new QDialogButtonBox();
    else if (className == QLatin1String("QTreeWidget"))
        newObject = new QTreeWidget();
    else if (className == QLatin1String("QCalendarWidget"))
        newObject = new QCalendarWidget();
    else if (className == QLatin1String("QAction"))
        newObject = new QAction(0);
    else if (className == QLatin1String("QTimeLine"))
        newObject = new QTimeLine();
    else if (className == QLatin1String("QTextDocument"))
        newObject = new QTextDocument();

    if (!newObject)
        return;

    QWidget *newWidget = qobject_cast<QWidget *>(newObject);
    if (newWidget) {
        QRect r = newWidget->geometry();
        r.setSize(newWidget->sizeHint());
        r.setWidth(qMax(r.width(), 150));
        r.setHeight(qMax(r.height(), 50));
        r.moveCenter(QApplication::desktop()->geometry().center());
        newWidget->setGeometry(r);
        newWidget->setWindowTitle(tr("Controlled Object: %1").arg(className));
        newWidget->show();
    }

    if (theControlledObject)
        delete theControlledObject;

    theControlledObject = newObject;
    theController->setObject(theControlledObject);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MyController *controller = new MyController();
    controller->show();

    int ret = app.exec();

    return ret;
}

#include "main.moc"
