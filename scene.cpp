#include "scene.hpp"
#include "state.hpp"
#include "renderpass.hpp"

#include <QtOpenGL>

Scene::Scene() : m_width(-1), m_height(-1) {}

void Scene::resize(int width, int height) {
  if (width == m_width && height == m_height) return;

  m_width = width;
  m_height = height;
}

void Scene::render() {
  State state;
  for (RenderPasses::iterator it = m_render_passes.begin(); it != m_render_passes.end(); ++it) {
    (*it)->render(state);
  }
}
