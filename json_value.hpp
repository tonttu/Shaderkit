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
   * If path is empty, returns this object.
   */
  Value find(const QString &path) const;

  /// Returns if this a valid Value (m_type != None)
  bool valid() const;

  /// If this is an "Object", return a child Value with name key
  Value operator[](const QString& key) const;
  /// If this is an "Array", return the Value with at the position index
  Value operator[](size_t index) const;

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
