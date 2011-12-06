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

#endif // LIGHT_LIST_HPP
