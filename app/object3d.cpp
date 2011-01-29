/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "object3d.hpp"
#include "state.hpp"
#include "model.hpp"
#include "ext/glut_teapot.hpp"

/// Renders a rectangular box.
void drawBox(float x, float y, float z) {
  glBegin(GL_QUADS);
  glNormal3f(1,0,0);
  glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
  glNormal3f(0,0,-1);
  glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
  glNormal3f(-1,0,0);
  glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
  glNormal3f(0,0,1);
  glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
  glNormal3f(0,1,0);
  glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z); glVertex3f(-x,y,-z);
  glNormal3f(0,-1,0);
  glVertex3f(-x,-y,z); glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z);
  glEnd();
}

Object3D::Object3D() {}
Object3D::~Object3D() {}

QVariantMap Object3D::save() const {
  QVariantMap map;
  QVariantList lst;
  if (builtin()) {
    lst << "built-in" << name();
  } else {
    lst << "file" << name();
  }
  map["model"] = lst;

  return map;
}

void Teapot::render(State&) {
  teapot(10, 3.7f, GL_FILL);
}

void Box::render(State&) {
  /// @todo remove translatef once there's a transform support in JSON format
  glTranslatef(0, -3.1f, 0);
  drawBox(3.5f, 0.4f, 3.5f);
}
