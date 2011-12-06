#ifndef OBJECT_LIST_HPP
#define OBJECT_LIST_HPP

#include "forward.hpp"

#include <QDialog>

namespace Ui {
class ObjectList;
}

class QListWidgetItem;
class ObjectList : public QDialog {
  Q_OBJECT

public:
  explicit ObjectList(QWidget *parent, RenderPassPtr render_pass);
  ~ObjectList();

private slots:
  void duplicate();
  void remove();
  void selectionChanged();
  void itemChanged(QListWidgetItem*);
  void updated(RenderPassPtr pass);

private:
  Ui::ObjectList* m_ui;
  RenderPassPtr m_render_pass;

  QAction* m_duplicate;
  QAction* m_destroy;
};

#endif // OBJECT_LIST_HPP
