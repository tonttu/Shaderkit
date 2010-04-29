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

#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include "forward.hpp"

#include <QString>

#include <map>
#include <vector>

/**
 * JSON Value object.
 *
 * Different value types specified by JSON are string, number, object ("map"),
 * array, true, false or null.
 *
 * In addition to those, we added our own type "None", that means that this
 * value is invalid, something that for example search functions might return.
 * We also gave different types for floating point numbers and integers.
 */
class Value {
public:

  /// Map type is used with "Object" Values.
  typedef std::map<QString, Value> Map;
  /// Vector type is used with "Array" Values.
  typedef std::vector<Value> Vector;

  /// The type of this Value
  enum Type {String, Double, Integer, Object, Array, Null, True, False, None};

  Value(Type t = None);
  Value(double d);
  Value(const QString& str);
  Value(const std::string& str);
  ~Value();

  /// Save the Value recursively to the output stream
  void save(std::ostream& os, int level = 0) const;

  /// Returns the value as a string at position path
  /// @param path same kind of query as the path parameter in find
  /// @param def value returned if correct Value was not found or isn't a String
  /// @param ok is valid pointer, set to true/false depending if there was a search match
  QString str(const QString& path = "", const QString& def = "", bool* ok = 0) const;

  /**
   * Search for the Value tree with a path (similar to xpath or css selectors)
   *
   * Path is a dot-separated list of object names. For example find("foo.bar")
   * assumes that the current Value is an "Object" that has a "foo" named
   * child object, that has a Value with name "bar". Either the result is
   * returned, or an invalid Value.
   *
   * Array items are indexed with number starting from zero. For example
   * find("foo.5") will search the sixth value of array "foo".
   *
   * If path is empty, returns this object.
   */
  Value find(const QString& path) const;

  /**
   * Searches with path and returns true if the object was found.
   */
  bool have(const QString& path) const;

  /// Converts find(path) to color, returns if the conversion was successful
  bool to(QColor& color, const QString& path = "") const;
  /// Converts find(path) to vector, returns if the conversion was successful
  bool to(QVector3D& vector, const QString& path = "") const;
  /// Converts find(path) to vector, returns if the conversion was successful
  bool to(QVector4D& vector, const QString& path = "") const;
  /// Converts find(path) to float, returns if the conversion was successful
  bool to(float& f, const QString& path = "") const;
  /// Converts find(path) to double, returns if the conversion was successful
  bool to(double& f, const QString& path = "") const;
  /// Converts find(path) to string, returns if the conversion was successful
  bool to(QString& str, const QString& path = "") const;

  /// Returns if this a valid Value (m_type != None)
  bool valid() const;

  /// If this is an "Object", return a child Value with name key
  Value operator[](const QString& key) const;
  /// If this is an "Array", return the Value with at the position index
  Value operator[](size_t index) const;

  /// Loads the file, parses it and stores it into this
  /// Implementation in json_parser.cpp
  bool load(const QString& filename);

  /// Returns the reference to the Map. Precondition: m_type == Object
  /// Use map() instead of this unless you know what you are doing.
  Value::Map& getMap();
  /// Returns the reference to the Vector. Precondition: m_type == Array
  /// Use array() instead of this unless you know what you are doing.
  Value::Vector& getArray();

  /// If find(path) is a map, returns copy of the actual Map,
  /// otherwise return an empty map.
  Value::Map map(const QString& path) const;

  /// If find(path) is an array, returns copy of the actual vector,
  /// otherwise return an empty vector.
  Value::Vector array(const QString& path) const;

  /// Typecast to QString, same as str()
  operator QString() const;

  /// Same as str() == value
  bool operator==(const QString& value) const;

protected:
  Type m_type;

  /// The actual value stored as a union
  /// @see Type
  union {
    QString* s;
    int i;
    double d;
    Map* o;
    Vector* a;
    bool b;
  } m_data;
};

#endif // JSON_VALUE_HPP
