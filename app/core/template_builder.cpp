#include "template_builder.hpp"

#include "parser.h"

#include "gl/program.hpp"

#include <QDirIterator>

namespace Shaderkit
{
  static QStringList s_defaultPaths;

  TemplateBuilder::TemplateBuilder()
  {
    addDefaultPaths();
    reload();
  }

  ProgramPtr TemplateBuilder::create(const TemplateBuilder::TemplateId& id) const
  {
    auto tpl = m_templates.value(id);
    ProgramPtr prog = std::make_shared<GLProgram>(tpl.name);
    for (auto& path: tpl.files)
      prog->addShader(path);
    return prog;
  }

  QList<TemplateBuilder::Template> TemplateBuilder::findMatchingTemplates(const Query& query) const
  {
    // ordered by -score
    QMultiMap<int, const TemplateBuilder::Template*> tmp;

    for (auto& tpl: m_templates)
      tmp.insert(0, &tpl);

    QList<TemplateBuilder::Template> result;
    for (auto ptr: tmp)
      result << *ptr;
    return result;
  }

  TemplateBuilder::Template TemplateBuilder::findBestTemplate(const Query& query) const
  {
    auto lst = findMatchingTemplates(query);
    if (lst.size() > 0)
      return lst[0];
    return TemplateBuilder::Template();
  }

  void TemplateBuilder::addPath(const QString& path)
  {
    m_paths.insert(0, path);
  }

  void TemplateBuilder::addDefaultPaths()
  {
    for (auto & path: s_defaultPaths)
      addPath(path);
  }

  void TemplateBuilder::loadFile(const QString& filename)
  {
    static TemplateId s_id = 0;

    QJson::Parser parser;
    bool ok = false;
    QFile file(filename);
    QVariant data = parser.parse(&file, &ok);
    if (!ok) {
      Log::error("Failed to load %s", filename.toUtf8().data());
      return;
    }
    QVariantMap map = data.toMap();

    Template tpl;

    QString cwd = QDir::currentPath();
    QDir::setCurrent(QFileInfo(filename).absolutePath());
    for (auto s: map["shaders"].toList()) {
      const QString shader = s.toMap()["file"].toString();
      if (!shader.isEmpty()) {
        tpl.files << QFileInfo(shader).absoluteFilePath();
      } else {
        Log::error("TemplateBuilder::loadFile # %s: failed to recognize shader", filename.toUtf8().data());
      }
    }
    QDir::setCurrent(cwd);

    if (tpl.files.isEmpty()) {
      Log::error("TemplateBuilder::loadFile # %s: No shaders detected", filename.toUtf8().data());
      return;
    }

    tpl.id = s_id++;
    tpl.name = map["name"].toString();
    tpl.textures = map["textures"].toInt();
    auto lst = map["version"].toList();
    if (lst.size() > 0)
      tpl.version = lst[0].toInt();
    if (lst.size() > 1)
      tpl.profile = lst[1].toString();

    m_templates[tpl.id] = tpl;
  }

  void TemplateBuilder::reload()
  {
    m_templates.clear();
    for (auto& root: m_paths) {
      QDirIterator it(root, QStringList() << "*.tpl", QDir::Files,
                      QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
      while (it.hasNext())
        loadFile(it.next());
    }
  }

  void TemplateBuilder::addDefaultPath(const QString& path)
  {
    s_defaultPaths << path;
  }
}
