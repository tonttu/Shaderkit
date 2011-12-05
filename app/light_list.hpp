#ifndef LIGHT_LIST_HPP
#define LIGHT_LIST_HPP

#include "forward.hpp"
#include <QDialog>

namespace Ui {
class LightList;
}

class QListWidgetItem;
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

private:
  Ui::LightList* m_ui;
  RenderPassPtr m_render_pass;

  QAction* m_duplicate;
  QAction* m_destroy;
};

#endif // LIGHT_LIST_HPP
