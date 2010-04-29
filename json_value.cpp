/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "json_value.hpp"

#include <QStringList>
#include <QColor>
#include <QVector3D>
#include <QVector4D>

#include <string>
#include <sstream>

/// Encodes string like specified in the JSON specification
/// @todo actually fails if there is a unicode character, that has one
///       byte in utf8 representation any of ' \n \f \n \r of \t chars
/// @todo change std::ostream to something more Qt
static void save_str(std::ostream& os, const QString& str) {
  const std::string s = str.toStdString();
  std::string::const_iterator it = s.begin(), end = s.end();
  os << '"';
  while (it != end) {
    if (*it == '"') os << "\\\"";
    else if (*it == '\'') os << "\\'";
    else if (*it == '\b') os << "\\b";
    else if (*it == '\f') os << "\\f";
    else if (*it == '\n') os << "\\n";
    else if (*it == '\r') os << "\\r";
    else if (*it == '\t') os << "\\t";
    else os << *it;
    ++it;
  }
  os << '"';
}

static void save_object(std::ostream& os, const Value::Map& map) {
  os << "{\n";
  Value::Map::const_iterator begin = map.begin(), end = map.end();
  for (Value::Map::const_iterator it = begin; it != end; ++it) {
    if (it != begin) os << ",\n";
    save_str(os, it->first);
    os << " : ";
    it->second.save(os);
  }
  os << "\n}";
}

static void save_array(std::ostream& os, const Value::Vector& array) {
  os << '[';
  Value::Vector::const_iterator begin = array.begin(), end = array.end();
  for (Value::Vector::const_iterator it = begin; it != end; ++it) {
    if (it != begin) os << ", ";
    it->save(os);
  }
  os << ']';
}

template <typename T> static T convert(const QString& str) {
  std::istringstream ss(str.toStdString());
  T t;
  ss >> t;
  return t;
}
template <typename T> static QString convert(const T& t) {
  std::ostringstream ss;
  ss << t;
  return QString::fromStdString(ss.str());
}


Value::Value(Type t) : m_type(t) {
  if (t == String) m_data.s = new QString;
  else if (t == Object) m_data.o = new Map;
  else if (t == Array) m_data.a = new Vector;
  else if (t == Double) m_data.d = 0.0;
  else if (t == Integer) m_data.i = 0;
}

Value::Value(double d) : m_type(Double) {
  m_data.d = d;
}

Value::Value(const QString& str) : m_type(String) {
  m_data.s = new QString(str);
}

Value::Value(const std::string& str) : m_type(String) {
  m_data.s = new QString(str.c_str());
}

Value::~Value() {
  if (m_type == String) delete m_data.s;
  else if (m_type == Object) delete m_data.o;
  else if (m_type == Array) delete m_data.a;
}

/// @todo actually use level for something
void Value::save(std::ostream& os, int /* level */) const {
  if (m_type == String) save_str(os, *m_data.s);
  else if (m_type == Double) os << m_data.d;
  else if (m_type == Integer) os << m_data.i;
  else if (m_type == Object) save_object(os, *m_data.o);
  else if (m_type == Array) save_array(os, *m_data.a);
  else if (m_type == True) os << "true";
  else if (m_type == False) os << "false";
  else if (m_type == Null) os << "null";
}

QString Value::str(const QString& path, const QString& def, bool* ok) const {
  QString out = def;
  if (ok) *ok = to(out, path); else to(out, path);
  return out;
}

Value Value::find(const QString& path) const {
  QStringList pieces = path.split(".", QString::SkipEmptyParts);
  QStringList::iterator it = pieces.begin();

  if (it == pieces.end()) return *this;

  Value out = operator[](*it++);
  while (it != pieces.end()) out = out[*it++];
  return out;
}

bool Value::have(const QString& path) const {
  return find(path).valid();
}

bool Value::to(QColor& color, const QString& path) const {
  Value v = find(path);
  if (v.m_type == Array && v.m_data.a->size() == 4) {
    Vector& array = *v.m_data.a;
    double f[4];
    for (int i = 0; i < 4; i++) if (!array[i].to(f[i])) return false;
    color.setRgbF(f[0], f[1], f[2], f[3]);
    return true;
  }
  return false;
}

bool Value::to(QVector3D& vector, const QString& path) const {
  Value v = find(path);
  if (v.m_type == Array && v.m_data.a->size() == 3) {
    Vector& array = *v.m_data.a;
    double f[3];
    for (int i = 0; i < 3; i++) if (!array[i].to(f[i])) return false;
    vector = QVector3D(f[0], f[1], f[2]);
    return true;
  }
  return false;
}

bool Value::to(QVector4D& vector, const QString& path) const {
  Value v = find(path);
  if (v.m_type == Array && v.m_data.a->size() == 4) {
    Vector& array = *v.m_data.a;
    double f[4];
    for (int i = 0; i < 4; i++) if (!array[i].to(f[i])) return false;
    vector = QVector4D(f[0], f[1], f[2], f[3]);
    return true;
  }
  return false;
}

bool Value::to(double& f, const QString& path) const {
  Value v = find(path);
  if (v.m_type == String) { f = convert<double>(*v.m_data.s); return true; }
  if (v.m_type == Double) { f = v.m_data.d; return true; }
  if (v.m_type == Integer) { f = static_cast<double>(v.m_data.i); return true; }
  if (v.m_type == True) { f = 1.0; return true; }
  if (v.m_type == False) { f = 0.0; return true; }
  return false;
}

bool Value::to(float& f, const QString& path) const {
  Value v = find(path);
  if (v.m_type == String) { f = convert<float>(*v.m_data.s); return true; }
  if (v.m_type == Double) { f = static_cast<float>(v.m_data.d); return true; }
  if (v.m_type == Integer) { f = static_cast<float>(v.m_data.i); return true; }
  if (v.m_type == True) { f = 1.0f; return true; }
  if (v.m_type == False) { f = 0.0f; return true; }
  return false;
}

bool Value::to(QString& str, const QString& path) const {
  Value v = find(path);
  if (v.m_type == String) { str = *v.m_data.s; return true; }
  if (v.m_type == Double) { str = convert(v.m_data.d); return true; }
  if (v.m_type == Integer) { str = convert(v.m_data.i); return true; }
  if (v.m_type == True) { str = "1"; return true; }
  if (v.m_type == False) { str = "0"; return true; }
  return false;
}

bool Value::valid() const {
  return m_type != None;
}

Value Value::operator[](const QString& key) const {
  if (m_type == Object) {
    Value::Map& map = *m_data.o;
    Value::Map::const_iterator it = map.find(key);
    if (it != map.end()) return it->second;
  } else if (m_type == Array) {
    bool ok;
    size_t index = key.toULong(&ok);
    if (ok) return operator[](index);
  }
  return Value();
}

Value Value::operator[](size_t index) const {
  if (m_type == Array) {
    Value::Vector& array = *m_data.a;
    if (index < array.size()) return array[index];
  }
  return Value();
}

Value::Map& Value::getMap() {
  assert(m_type == Object);
  return *m_data.o;
}

Value::Vector& Value::getArray() {
  assert(m_type == Array);
  return *m_data.a;
}

Value::Vector Value::array(const QString& path) const {
  Value v = find(path);
  if (v.m_type == Array) return v.getArray();
  return Vector();
}

Value::operator QString() const {
  return str();
}

bool Value::operator==(const QString& value) const {
  return str() == value;
}
