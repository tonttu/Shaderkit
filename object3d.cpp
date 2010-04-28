#include "object3d.hpp"
#include "state.hpp"
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

/// @todo remove this, only for testing
static float f = 0.0f;

void Teapot::render(State&) {
  f += 0.02f;
  glRotatef(f, 0, 1, 0);

  teapot(10, 3.7f, GL_FILL);
}

void Box::render(State&) {
  glRotatef(f, 0, 1, 0);

  /// @todo remove translatef once there's a transform support in JSON format
  glTranslatef(0, -3.1f, 0);
  drawBox(3.5f, 0.4f, 3.5f);
}
