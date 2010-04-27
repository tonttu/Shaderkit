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
struct Value {
  /// Map type is used with "Object" Values.
  typedef std::map<QString, Value> Map;
  /// Vector type is used with "Array" Values.
  typedef std::vector<Value> Vector;

  /// The type of this Value
  enum Type {String, Double, Integer, Object, Array, Null, True, False, None} type;

  Value(Type t = None);
  Value(double d);
  Value(const QString& str);
  Value(const std::string& str);
  ~Value();

  /// Save the Value recursively to the output stream
  void save(std::ostream& os, int level = 0) const;

  /// The actual value stored as a union
  /// @see Type
  union {
    QString* s;
    int i;
    double d;
    Map* o;
    Vector* a;
    bool b;
  } data;
};

#endif // JSON_VALUE_HPP
