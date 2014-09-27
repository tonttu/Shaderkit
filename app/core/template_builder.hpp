#ifndef SHADERKIT_TEMPLATE_BUILDER_HPP
#define SHADERKIT_TEMPLATE_BUILDER_HPP

#include "gl/shader.hpp"

#include <QGLFormat>

namespace Shaderkit
{
  class TemplateBuilder
  {
  public:
    typedef int TemplateId;

    struct Template
    {
      Template() : id(-1), version(0), textures(0) {}
      TemplateId id;
      QString name;
      int version;
      QString profile;
      int textures;
      QStringList files;
    };

    struct Query
    {
      Query() : textures(0) {}

      QGLFormat format;
      int textures;
      QString shading_model;
    };

  public:
    TemplateBuilder();

    // Creates template shader for id
    ProgramPtr create(const TemplateId& id) const;

    QList<Template> findMatchingTemplates(const Query& query) const;
    /// Might return a template that might not work with the given format,
    /// but is the best we can do. If we have multiple matches, takes the
    /// 'best' match.
    Template findBestTemplate(const Query& query) const;

    void addPath(const QString& path);

    void reload();

    void static addDefaultPath(const QString& path);

  private:
    void addDefaultPaths();
    void loadFile(const QString& file);

  private:
    QStringList m_paths;
    TemplateId m_nextId;
    QMap<TemplateId, Template> m_templates;
  };
}

#endif // SHADERKIT_TEMPLATE_BUILDER_HPP
