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

#ifndef LIMBO_DIALOG_HPP
#define LIMBO_DIALOG_HPP

#include <QWidget>
#include <QSet>

#include "forward.hpp"

namespace Ui {
  class LimboDialog;
}

class LimboDialog : public QWidget {
  Q_OBJECT

public:
  LimboDialog(const QString& filename, ScenePtr scene, const MetaInfo& meta, bool& cont);
  ~LimboDialog();

protected:
  void closeEvent(QCloseEvent*);

private slots:
  void open();
  void save();
  void maybeDiscard();
  void notNow();
  void discard();
  void abortDiscard();

private:
  const QString m_filename;
  const MetaInfo& m_meta;
  bool m_done;
  bool& m_cont;

  QSet<QString> m_localFiles;
  ScenePtr m_scene;

  Ui::LimboDialog* m_ui;
};

#endif // LIMBO_DIALOG_HPP
