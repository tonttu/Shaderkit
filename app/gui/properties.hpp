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

#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include "forward.hpp"
#include "gl/uniform.hpp"

#include <QMap>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QComboBox>
#include <QStackedWidget>

class QStackedWidget;

namespace Shaderkit
{

  /**
   * This line editor updates its size hints automatically based on the content.
   */
  class LineEdit : public QLineEdit
  {
    Q_OBJECT

  public:
    LineEdit(QWidget* parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

  private slots:
    void updateSizes();
  };

  class MenuComboBox : public QComboBox
  {
    Q_OBJECT

  signals:
    void showPopup(QPoint);

  public:
    MenuComboBox(QWidget* parent = 0) : QComboBox(parent) {}
    virtual void showPopup();
  };

  class HeaderWidget;
  class MaterialProperties;
  class UniformEditor;
  class AttributeEditor;

  struct MaterialItem {
    MaterialPtr material;
    HeaderWidget* header;

    QPersistentModelIndex header_index;
    QPersistentModelIndex last_index;

    QMap<QString, UniformEditor*> uniform_editors;
    QMap<QString, AttributeEditor*> attribute_mappers;
  };

  class VarEditor : public QObject
  {
  public:
    VarEditor(MaterialProperties& prop, int row, MaterialPtr mat, const QString& name);
    virtual ~VarEditor() {}

    QLabel* label() const { return m_label; }
    QPersistentModelIndex index() const { return m_index; }

    MaterialPtr material() const { return m_mat; }
    const QString& name() const { return m_name; }

  protected:
    MaterialPtr m_mat;
    QString m_name;

    QLabel* m_label;

    QPersistentModelIndex m_index;
  };

  class AttributeEditor : public VarEditor
  {
    Q_OBJECT

  public:
    AttributeEditor(MaterialProperties& prop, int row, MaterialPtr mat, AttributeVar& var);
    virtual void updateUI(AttributeVar& var);

  private slots:
    void editingFinished();

  private:
    void clear();

  private:
    QComboBox* m_combo;
  };

  class UniformEditor : public VarEditor
  {
  public:
    UniformEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
    virtual void updateUI(UniformVar& var) = 0;

    virtual bool isInteractiveEditor() const;

  protected:
    UniformVar* getVar();
  };

  class FloatEditor;
  class UniformMapEditor : public QStackedWidget
  {
    Q_OBJECT

  public:
    UniformMapEditor(MaterialProperties& prop, UniformEditor& editor, bool only_menu);
    virtual void updateUI(UniformVar& var);

    /// @todo QSlider should be a template parameter
    QSlider& slider() const { return *m_slider; }

    bool editorVisible() const;

  private slots:
    void editingFinished();
    void menu(QPoint point);

  private:
    void clear();

  private:
    UniformEditor& m_editor;
    MenuComboBox* m_combo;
    QSlider* m_slider;
    bool m_only_menu;
  };

  class FloatEditor : public UniformEditor
  {
    Q_OBJECT

  public:
    FloatEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
    virtual ~FloatEditor();

    virtual bool isInteractiveEditor() const;

    void updateUI(UniformVar& var);

  private slots:
    void editingFinished();
    void valueChanged(int);
    void reset();

  private:
    LineEdit* m_edit;
    UniformMapEditor* m_ui;
    float m_min, m_max;

    QAction* m_reset_action;
  };

  class GenericEditor : public UniformEditor
  {
    Q_OBJECT

  public:
    GenericEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);

    void updateUI(UniformVar& var);

  private:
    UniformMapEditor* m_ui;
  };

  class TextureEditor : public UniformEditor
  {
    Q_OBJECT

  public:
    TextureEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
    virtual ~TextureEditor();

    void updateUI(UniformVar& var);

  private slots:
    void showPreview();
    void browse();

  private:
    QLabel* m_texname;
    TextureWidgetGL* m_icon;
  };

  class PropertyLayout;
  class PropertyItem;
  struct PropertyLayoutData {
    PropertyLayoutData(int columns, int stretch_column);

    QMargins& margins(int i) { return padding[i]; }

  private:
    int columns, stretch_column;
    typedef QList<QVector<PropertyItem*> > Items;
    Items items;
    QVector<QMargins> padding;
    QSize min_size;

    friend class MaterialProperties;
    friend class PropertyLayout;
  };

  class PropertyLayout : public QLayout
  {
  public:
    PropertyLayout(std::shared_ptr<PropertyLayoutData> data);
    ~PropertyLayout();

    virtual QSize minimumSize() const;
    virtual QSize sizeHint() const;
    virtual void addItem(QLayoutItem*);
    virtual QLayoutItem* itemAt(int index) const;
    virtual QLayoutItem* takeAt(int index);
    virtual int count() const;

    void setWidget(int column, int colspan, QWidget* widget);

    void setGeometry(const QRect& r);

  private:
    std::shared_ptr<PropertyLayoutData> m_data;
    QVector<PropertyItem*>& m_row;
  };

  class HeaderWidget : public QWidget
  {
  public:
    HeaderWidget(QIcon active, QIcon inactive);

    void setText(QString text) { m_text = text; }
    QString text() const { return m_text; }

    QIcon icon() const { return *m_icon; }

    void setSelected(bool selected);

  protected:
    void paintEvent(QPaintEvent* ev);

    QString m_text;
    bool m_selected;
    QIcon* m_icon;
    QIcon m_active;
    QIcon m_inactive;
  };

  /**
   * Real-time property editor for shader properties, like uniform variables.
   *
   * This is a singleton class.
   */
  class MaterialProperties : public QTableWidget
  {
    Q_OBJECT

  public:
    static MaterialProperties& instance();
    MaterialProperties(QWidget* parent = 0);
    virtual ~MaterialProperties();

    void init();

    std::shared_ptr<PropertyLayoutData> layoutData() { return m_data; }

  protected:
    /// @see RenderPassProperties::paintEvent
    void paintEvent(QPaintEvent* e);

  public slots:
    /// This shader program in given render pass has changed (usually just relinked)
    void update(MaterialPtr mat);
    void setActiveMaterials(QSet<MaterialPtr> materials);
    void updateMaterialList(ScenePtr scene);
    void selectionChanged();
    void itemSelected(int row, int column);

  protected slots:
    void create();
    void load();
    void duplicate();
    void edit();
    void remove();
    void toggleMode();

    void materialChanged(MaterialPtr);

  private:
    void updateUniforms(MaterialItem& item, MaterialPtr mat);
    void updateAttributes(MaterialItem& item, MaterialPtr mat);

  protected:
    QMap<MaterialPtr, MaterialItem> m_materials;
    UniformEditor* createEditor(MaterialPtr mat, UniformVar& var,
                                const ShaderTypeInfo& type, int row);
    virtual void startDrag(Qt::DropActions supportedActions);
    virtual void contextMenuEvent(QContextMenuEvent* e);

    MaterialPtr get(QModelIndex index) const;
    MaterialPtr getSelected() const;
    // bool viewportEvent(QEvent* event);

    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index,
        const QEvent* event) const;
    QAction *m_only_editors, *m_create, *m_open, *m_duplicate, *m_edit, *m_destroy;
    // int m_hover_row;

    std::shared_ptr<PropertyLayoutData> m_data;

    MaterialPtr m_selected;

    bool m_firstRender;

    static MaterialProperties* s_instance;
  };

  /**
   * List of all available editable files.
   *
   * This is a singleton class.
   */
  /*
  class FileList : public QTreeWidget {
    Q_OBJECT

  public:
    static FileList& instance();
    FileList(QWidget* parent = 0);
    virtual ~FileList();

    void init();

  signals:
    void openFile(ShaderPtr);

  public slots:
    void update(ShaderPtr shader);
    void activateFile(QTreeWidgetItem* item, int column);
    void remove(ShaderPtr shader);
    void selectionChanged();

  protected:
    QMap<QString, QTreeWidgetItem*> m_files;
    QMap<QTreeWidgetItem*, ShaderPtr> m_items;
    QTreeWidgetItem* m_src;

    QAction *m_create, *m_open, *m_duplicate, *m_edit, *m_destroy;

    static FileList* s_instance;
  };
  */

} // namespace Shaderkit

#endif // PROPERTIES_HPP
