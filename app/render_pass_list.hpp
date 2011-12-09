#ifndef RENDER_PASS_LIST_HPP
#define RENDER_PASS_LIST_HPP

#include "forward.hpp"

#include <QDialog>

namespace Ui {
class RenderPassList;
}

class RenderPassList : public QDialog {
  Q_OBJECT

public:
  RenderPassList(QList<RenderPassPtr> list, QWidget* parent = 0);
  ~RenderPassList();

  QList<RenderPassPtr> orderedList() const;

private:
  QList<RenderPassPtr> m_list;
  Ui::RenderPassList* m_ui;

  void updateList();
};

#endif // RENDER_PASS_LIST_HPP
