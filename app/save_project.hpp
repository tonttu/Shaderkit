#ifndef SAVE_PROJECT_HPP
#define SAVE_PROJECT_HPP

#include "forward.hpp"
#include <QDialog>

namespace Ui {
class SaveProject;
}

class SaveProject : public QDialog {
  Q_OBJECT

public:
  explicit SaveProject(QWidget* parent = 0);
  ~SaveProject();

  static bool save(ScenePtr scene);

protected:
  void keyPressEvent(QKeyEvent* e);

private slots:
  void filenameChanged(QString);
  void browse();

private:
  Ui::SaveProject* m_ui;
};

#endif // SAVE_PROJECT_HPP
