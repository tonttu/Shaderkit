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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QMap>
#include "qtcanvas.h"

class QtProperty;

class CanvasView : public QtCanvasView
{
    Q_OBJECT
public:
    CanvasView(QWidget *parent = 0)
        : QtCanvasView(parent), moving(0) { }
    CanvasView(QtCanvas *canvas, QWidget *parent = 0)
        : QtCanvasView(canvas, parent), moving(0) { }
signals:
    void itemClicked(QtCanvasItem *item);
    void itemMoved(QtCanvasItem *item);
protected:
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseDoubleClickEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent* event);
private:
    void handleMouseClickEvent(QMouseEvent *event);
    QPoint moving_start;
    QtCanvasItem *moving;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

private slots:
    void newRectangle();
    void newEllipse();
    void newLine();
    void newText();
    void deleteObject();
    void clearAll();
    void fillView();

    void itemClicked(QtCanvasItem *item);
    void itemMoved(QtCanvasItem *item);
    void valueChanged(QtProperty *property, double value);
    void valueChanged(QtProperty *property, const QString &value);
    void valueChanged(QtProperty *property, const QColor &value);
    void valueChanged(QtProperty *property, const QFont &value);
    void valueChanged(QtProperty *property, const QPoint &value);
    void valueChanged(QtProperty *property, const QSize &value);
private:

    QtCanvasItem *addRectangle();
    QtCanvasItem *addEllipse();
    QtCanvasItem *addLine();
    QtCanvasItem *addText();
    void addProperty(QtProperty *property, const QString &id);
    void updateExpandState();

    QAction *deleteAction;

    class QtDoublePropertyManager *doubleManager;
    class QtStringPropertyManager *stringManager;
    class QtColorPropertyManager *colorManager;
    class QtFontPropertyManager *fontManager;
    class QtPointPropertyManager *pointManager;
    class QtSizePropertyManager *sizeManager;

    class QtTreePropertyBrowser *propertyEditor;
    CanvasView *canvasView;
    QtCanvas *canvas;
    QtCanvasItem *currentItem;
    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;
};

#endif
