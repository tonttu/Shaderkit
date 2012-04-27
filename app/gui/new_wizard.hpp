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

#ifndef NEW_WIZARD_HPP
#define NEW_WIZARD_HPP

#include <QDialog>
#include <QSet>
#include <QCheckBox>

namespace Shaderkit
{

  namespace Ui
  {
    class NewWizard;
  }

  class ToggleButton : public QCheckBox
  {
  public:
    ToggleButton(QWidget* parent = 0);
    void paintEvent(QPaintEvent*);
  };

  class NewWizard : public QDialog
  {
    Q_OBJECT

  public:
    explicit NewWizard(QWidget* parent = 0);
    ~NewWizard();

  protected:
    void keyPressEvent(QKeyEvent* e);

  private slots:
    void nameEditingFinished();
    void nameEdited(QString);
    void preview(QString);
    void create();

  private:
    Ui::NewWizard* m_ui;
    QSet<QString> m_scene_names;
    QButtonGroup* m_group;

    QString getUniqName(QString str) const;
  };

} // namespace Shaderkit

#endif // NEW_WIZARD_H
