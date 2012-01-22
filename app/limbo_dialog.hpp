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
