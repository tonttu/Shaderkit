/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "editor.hpp"
#include "shader/shader.hpp"
#include "project.hpp"

#include <QKeyEvent>
#include <QFile>
#include <QFileInfo>
#include <QLabel>

IconBtn::~IconBtn() {}

void IconBtn::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.drawPixmap(0, 0, icon().pixmap(22, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent), m_ui(new Ui::MainWindow) {
  m_ui->setupUi(this);
  connect(m_ui->error_list, SIGNAL(itemActivated(QTableWidgetItem*)),
          this, SLOT(errorItemActivated(QTableWidgetItem*)));
  connect(m_ui->action_save, SIGNAL(triggered()), this, SLOT(save()));
}

MainWindow::~MainWindow() {}

void MainWindow::setProject(ProjectPtr p) {
  m_project = p;
  connect(p.get(), SIGNAL(sceneChanged(ScenePtr)),
          m_ui->gl_widget, SLOT(sceneChange(ScenePtr)));
}

Editor* MainWindow::createEditor(ShaderPtr shader) {
  QWidget* widget = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
  QStatusBar* statusbar = new QStatusBar(widget);

  Editor* editor = new Editor(widget, shader);
  connect(editor, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));

  QPushButton* btn = new IconBtn();
  QIcon icon;
  icon.addFile("gfx/nosync.png", QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile("gfx/sync.png", QSize(), QIcon::Normal, QIcon::On);

  btn->setIcon(icon);
  btn->setCheckable(true);
  btn->setChecked(true);
  btn->setToolTip("Auto-compile");
  statusbar->addWidget(btn);
  connect(btn, SIGNAL(toggled(bool)), editor, SLOT(syncToggled(bool)));

  layout->addWidget(editor);
  layout->addWidget(statusbar);

  editor->readFile(shader->filename());

  m_editors.push_back(editor);

  QFileInfo fi(shader->filename());
  m_ui->editor_tabs->addTab(widget, fi.fileName());
  m_ui->editor_tabs->setCurrentIndex(m_ui->editor_tabs->count()-1);
  return editor;
}

void MainWindow::shaderCompiled(ShaderPtr shader, ShaderError::List errors) {
  bool changed = false;
  for (int i = 0; i < m_ui->error_list->rowCount(); ++i) {
    QTableWidgetItem* item = m_ui->error_list->item(i, 0);
    if (m_error_list_items[item].shader() == shader) {
      m_error_list_items.remove(item);
      m_ui->error_list->removeRow(i--);
      changed = true;
    }
  }

  for (ShaderError::List::iterator e = errors.begin(); e != errors.end(); ++e) {
    int r = m_ui->error_list->rowCount();
    m_ui->error_list->setRowCount(r+1);

    /// @todo change order, put this on top
    QTableWidgetItem* msg = new QTableWidgetItem(e->msg());
    msg->setIcon(QIcon(QPixmap(e->type() == "warning" ? "gfx/warning.png" : "gfx/error.png")));
    m_ui->error_list->setItem(r, 0, msg);
    m_ui->error_list->setItem(r, 1, new QTableWidgetItem(shader->filename()));
    m_ui->error_list->setItem(r, 2, new QTableWidgetItem(QString::number(e->line()+1)));
    m_error_list_items[msg] = *e;
    changed = true;
  }

  if (changed) {
    m_ui->error_list->resizeColumnsToContents();
    m_ui->error_list->repaint();
  }
}

void MainWindow::errorItemActivated(QTableWidgetItem* item) {
  ShaderError err = m_error_list_items[m_ui->error_list->item(item->row(), 0)];

  for (int idx = 0; idx < m_editors.size(); ++idx) {
    if (m_editors[idx]->shader() == err.shader()) {
      m_editors[idx]->focusOnError(err);
      m_ui->editor_tabs->setCurrentIndex(idx);
      break;
    }
  }
}

void MainWindow::modificationChanged(bool b) {
  Editor* editor = dynamic_cast<Editor*>(sender());
  if (editor) {
    QFileInfo fi(editor->filename());
    m_ui->editor_tabs->setTabText(m_editors.indexOf(editor),
                                  fi.fileName() + (b ? "*" : ""));
  }
}

void MainWindow::save() {
  Editor* editor = m_editors[m_ui->editor_tabs->currentIndex()];
  QFile file(editor->filename());
  if (file.open(QIODevice::WriteOnly)) {
    file.write(editor->toPlainText().toUtf8());
    editor->document()->setModified(false);
  }
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Escape)
    close();
  else
    QWidget::keyPressEvent(e);
}

void MainWindow::changeEvent(QEvent* e) {
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    m_ui->retranslateUi(this);
    break;
  default:
    break;
  }
}
