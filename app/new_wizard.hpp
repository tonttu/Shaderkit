#ifndef NEW_WIZARD_HPP
#define NEW_WIZARD_HPP

#include <QDialog>
#include <QSet>
#include <QCheckBox>

namespace Ui {
class NewWizard;
}

class ToggleButton : public QCheckBox {
public:
  ToggleButton(QWidget* parent = 0);
  void paintEvent(QPaintEvent*);
};

class NewWizard : public QDialog {
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
  QSet<QString> m_project_names;
  QButtonGroup* m_group;

  QString getUniqName(QString str) const;
};

#endif // NEW_WIZARD_H
