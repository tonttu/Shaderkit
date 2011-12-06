#include "light_list.hpp"
#include "ui_light_list.h"

#include "renderpass.hpp"
#include "mainwindow.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "viewport.hpp"

QString alphaName(const QColor& color) {
  QRgb argb = color.rgba();
  return QString("#%1").arg(((argb & 0xFFFFFF) << 8) | qAlpha(argb), 8, 16, QLatin1Char('0')).toUpper();
}

QColor fromAlphaName(const QString& name) {
  if (name.length() == 9) {
    QColor c(name.left(7));
    c.setAlpha(name.right(2).toInt(0, 16));
    return c;
  } else {
    return QColor(name);
  }
}

void updatePreview(QPushButton& btn, QColor& color) {
  QImage img(16, 16, QImage::Format_RGB32);

  int a = 0x66, b = 0x99;
  QRgb src = color.rgb();
  float mix = color.alphaF();
  QRgb colora = qRgb(a * (1.0f - mix) + mix * qRed(src),
                     a * (1.0f - mix) + mix * qGreen(src),
                     a * (1.0f - mix) + mix * qBlue(src));
  QRgb colorb = qRgb(b * (1.0f - mix) + mix * qRed(src),
                     b * (1.0f - mix) + mix * qGreen(src),
                     b * (1.0f - mix) + mix * qBlue(src));

  for (int x = 0; x < 16; ++x) {
    int i = (x >> 2) % 2;
    for (int y = 0; y < 16; ++y) {
      int j = (y >> 2) % 2;
      img.setPixel(x, y, i == j ? colora : colorb);
    }
  }

  btn.setIcon(QIcon(QPixmap::fromImage(img)));
}

LightList::LightList(QWidget* parent, RenderPassPtr render_pass)
  : QDialog(parent),
    m_ui(new Ui::LightList),
    m_render_pass(render_pass) {
  m_ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);

  Viewport* view = MainWindow::instance().mainViewport();
  if (view) {
    /// @todo should somehow force this to use this render pass
    m_ui->toolbar->addAction(view->actionNewLight());
  }
  m_ui->toolbar->addSeparator();
  m_duplicate = m_ui->toolbar->addAction(QIcon(":/icons/duplicate.png"),
                                         "Duplicate light", this, SLOT(duplicate()));
  m_duplicate->setEnabled(false);
  m_ui->toolbar->addSeparator();
  m_destroy = m_ui->toolbar->addAction(QIcon(":/icons/delete.png"),
                                       "Delete", this, SLOT(remove()));
  m_destroy->setEnabled(false);

  updated(m_render_pass);
  if (m_render_pass) {
    setWindowTitle(QString("Lights for render pass %1").arg(m_render_pass->name()));
    connect(m_render_pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
    connect(m_ui->list, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*)));
  } else {
    setWindowTitle("Light browser");
  }
  connect(m_ui->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

  connect(m_ui->name, SIGNAL(editingFinished()), this, SLOT(nameChanged()));
  connect(m_ui->type, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged()));
  connect(m_ui->ambient, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
  connect(m_ui->diffuse, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
  connect(m_ui->specular, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
  connect(m_ui->locx, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->locy, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->locz, SIGNAL(editingFinished()), this, SLOT(locChanged()));
  connect(m_ui->targetx, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->targety, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->targetx, SIGNAL(editingFinished()), this, SLOT(targetChanged()));
  connect(m_ui->cutoff, SIGNAL(editingFinished()), this, SLOT(cutoffChanged()));
  connect(m_ui->dirx, SIGNAL(editingFinished()), this, SLOT(dirChanged()));
  connect(m_ui->diry, SIGNAL(editingFinished()), this, SLOT(dirChanged()));
  connect(m_ui->dirz, SIGNAL(editingFinished()), this, SLOT(dirChanged()));

  connect(m_ui->ambient_color, SIGNAL(clicked()), this, SLOT(colorDialog()));
  connect(m_ui->diffuse_color, SIGNAL(clicked()), this, SLOT(colorDialog()));
  connect(m_ui->specular_color, SIGNAL(clicked()), this, SLOT(colorDialog()));

  m_ui->locx->setValidator(new QDoubleValidator(this));
  m_ui->locy->setValidator(new QDoubleValidator(this));
  m_ui->locz->setValidator(new QDoubleValidator(this));
  m_ui->targetx->setValidator(new QDoubleValidator(this));
  m_ui->targety->setValidator(new QDoubleValidator(this));
  m_ui->targetz->setValidator(new QDoubleValidator(this));
  m_ui->cutoff->setValidator(new QDoubleValidator(this));
  m_ui->dirx->setValidator(new QDoubleValidator(this));
  m_ui->diry->setValidator(new QDoubleValidator(this));
  m_ui->dirz->setValidator(new QDoubleValidator(this));

  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);
  font.setPointSize(10);
  font.setStyleStrategy(QFont::PreferAntialias);
  m_ui->ambient->setFont(font);
  m_ui->diffuse->setFont(font);
  m_ui->specular->setFont(font);

  updatePanel();
}

LightList::~LightList() {
  delete m_ui;
}

void LightList::duplicate() {
  /// @todo implement
}

void LightList::remove() {
  /// @todo implement
}

void LightList::updated(RenderPassPtr pass) {
  assert(m_render_pass == pass);

  QSet<LightPtr> my_lights;
  if (pass) my_lights = pass->lights();

  if (pass)
    disconnect(pass.get(), SIGNAL(changed(RenderPassPtr)),
              this, SLOT(updated(RenderPassPtr)));

  m_ui->list->clear();
  foreach (LightPtr light, MainWindow::scene()->lights()) {
    /// @todo icon
    QListWidgetItem* item = new QListWidgetItem(light->name(), m_ui->list);
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (pass) flags |= Qt::ItemIsUserCheckable;
    item->setFlags(flags);
    if (pass) {
      if (my_lights.contains(light))
        item->setCheckState(Qt::Checked);
      else
        item->setCheckState(Qt::Unchecked);
    }
  }
  if (pass)
    connect(pass.get(), SIGNAL(changed(RenderPassPtr)),
            this, SLOT(updated(RenderPassPtr)));
}

void LightList::selectionChanged() {
  int c = m_ui->list->selectedItems().size();
  m_destroy->setEnabled(c >= 1);
  m_duplicate->setEnabled(c == 1);

  if (m_ui->list->selectedItems().size() == 1)
    m_light = MainWindow::scene()->light(m_ui->list->selectedItems()[0]->text());
  else
    m_light.reset();
  updatePanel();
}

void LightList::updatePanel() {
  if (m_light) {
    m_ui->name->setText(m_light->name());
    bool spot = m_light->type() == Light::Spot;
    m_ui->type->setCurrentIndex(spot ? 0 : 1);
    m_ui->ambient->setText(alphaName(m_light->ambient()));
    m_ui->diffuse->setText(alphaName(m_light->diffuse()));
    m_ui->specular->setText(alphaName(m_light->specular()));
    if (spot) {
      m_ui->locx->setText(QString::number(m_light->location().x()));
      m_ui->locy->setText(QString::number(m_light->location().y()));
      m_ui->locz->setText(QString::number(m_light->location().z()));
      m_ui->targetx->setText(QString::number(m_light->target().x()));
      m_ui->targety->setText(QString::number(m_light->target().y()));
      m_ui->targetz->setText(QString::number(m_light->target().z()));
      m_ui->cutoff->setText(QString::number(m_light->spotCutoff()));
      m_ui->dir_group->hide();
      m_ui->spot_group->show();
    } else {
      m_ui->dirx->setText(QString::number(m_light->direction().x()));
      m_ui->diry->setText(QString::number(m_light->direction().y()));
      m_ui->dirz->setText(QString::number(m_light->direction().z()));
      m_ui->spot_group->hide();
      m_ui->dir_group->show();
    }
    m_ui->panel->setEnabled(true);
  } else {
    m_ui->panel->setEnabled(false);
    m_ui->name->setText("");
    m_ui->type->setCurrentIndex(0);
    m_ui->ambient->setText("#000000");
    m_ui->diffuse->setText("#000000");
    m_ui->specular->setText("#000000");
    m_ui->locx->setText("0.0");
    m_ui->locy->setText("0.0");
    m_ui->locz->setText("0.0");
    m_ui->targetx->setText("0.0");
    m_ui->targety->setText("0.0");
    m_ui->targetz->setText("0.0");
    m_ui->cutoff->setText("0.0");
    m_ui->spot_group->hide();
    m_ui->dir_group->hide();
  }
}

void LightList::itemChanged(QListWidgetItem* item) {
  LightPtr light = MainWindow::scene()->light(item->text());
  if (light) {
    if (item->checkState() != Qt::Unchecked)
      m_render_pass->add(light);
    else
      m_render_pass->remove(light);
  }
}

void LightList::nameChanged() {
  /// @todo implement
}

void LightList::typeChanged() {
  if (!m_light) return;

  Light::Type t = m_ui->type->currentIndex() == 0 ? Light::Spot : Light::Direction;
  m_light->setType(t);

  updatePanel();
}

void LightList::colorChanged() {
  if (!m_light) return;
  QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
  if (!edit) return;

  QColor color = fromAlphaName(edit->text());
  if (edit == m_ui->ambient) {
    m_light->setAmbient(color);
    updatePreview(*m_ui->ambient_color, color);
  } else if (edit == m_ui->diffuse) {
    m_light->setDiffuse(color);
    updatePreview(*m_ui->diffuse_color, color);
  } else if (edit == m_ui->specular) {
    m_light->setSpecular(color);
    updatePreview(*m_ui->specular_color, color);
  }
}

void LightList::locChanged() {
  if (!m_light) return;
  QVector3D loc(m_ui->locx->text().toFloat(),
                m_ui->locy->text().toFloat(),
                m_ui->locz->text().toFloat());
  m_light->setLocation(loc);

}

void LightList::targetChanged() {
  if (!m_light) return;
  QVector3D target(m_ui->targetx->text().toFloat(),
                   m_ui->targety->text().toFloat(),
                   m_ui->targetz->text().toFloat());
  m_light->setTarget(target);

}

void LightList::cutoffChanged() {
  if (!m_light) return;
  m_light->setSpotCutoff(m_ui->cutoff->text().toFloat());
}

void LightList::dirChanged() {
  if (!m_light) return;
  QVector3D dir(m_ui->dirx->text().toFloat(),
                m_ui->diry->text().toFloat(),
                m_ui->dirz->text().toFloat());
  m_light->setDirection(dir);
}

void LightList::colorDialog() {
  if (!m_light) return;
  QPushButton* btn = dynamic_cast<QPushButton*>(sender());
  if (!btn) return;

  QLineEdit* edit;
  QString name;

  if (btn == m_ui->ambient_color) {
    name = "ambient";
    edit = m_ui->ambient;
  } else if (btn == m_ui->diffuse_color) {
    name = "diffuse";
    edit = m_ui->diffuse;
  } else if (btn == m_ui->specular_color) {
    name = "specular";
    edit = m_ui->specular;
  } else {
    return;
  }

  QString orig = edit->text();

  QColorDialog dialog(this);
  dialog.setWindowTitle("Select " + name + " color for Light " + m_light->name());
  dialog.setOptions(QColorDialog::ShowAlphaChannel);
  dialog.setCurrentColor(fromAlphaName(orig));

  if (btn == m_ui->ambient_color) {
    connect(&dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(setAmbient(QColor)));
  } else if (btn == m_ui->diffuse_color) {
    connect(&dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(setDiffuse(QColor)));
  } else {
    connect(&dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(setSpecular(QColor)));
  }

  dialog.exec();

  if (dialog.result() == QDialog::Rejected) {
    edit->setText(orig);
  }
}

void LightList::setAmbient(QColor color) {
  m_ui->ambient->setText(alphaName(color));
}

void LightList::setDiffuse(QColor color) {
  m_ui->diffuse->setText(alphaName(color));
}

void LightList::setSpecular(QColor color) {
  m_ui->specular->setText(alphaName(color));
}
