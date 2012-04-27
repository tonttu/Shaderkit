/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIGHT_LIST_HPP
#define LIGHT_LIST_HPP

#include "forward.hpp"
#include <QDialog>

class QListWidgetItem;

namespace Shaderkit {

namespace Ui {
class LightList;
}

class LightList : public QDialog {
  Q_OBJECT

public:
  explicit LightList(QWidget *parent, RenderPassPtr render_pass);
  ~LightList();

private slots:
  void duplicate();
  void remove();
  void selectionChanged();
  void itemChanged(QListWidgetItem*);
  void updated(RenderPassPtr pass);

  void nameChanged();
  void typeChanged();
  void colorChanged();
  void locChanged();
  void targetChanged();
  void cutoffChanged();
  void dirChanged();

  void colorDialog();
  void setAmbient(QColor);
  void setDiffuse(QColor);
  void setSpecular(QColor);

private:
  void updatePanel();

  Ui::LightList* m_ui;
  RenderPassPtr m_render_pass;

  LightPtr m_light;

  QAction* m_duplicate;
  QAction* m_destroy;
};

} // namespace Shaderkit

#endif // LIGHT_LIST_HPP
