#ifndef NEW_WIZARD_HPP
#define NEW_WIZARD_HPP

#include <QDialog>

namespace Ui {
class NewWizard;
}

class NewWizard : public QDialog {
  Q_OBJECT

public:
  explicit NewWizard(QWidget* parent = 0);
  ~NewWizard();

private:
  Ui::NewWizard* m_ui;
};

#endif // NEW_WIZARD_H
