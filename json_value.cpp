#include "json_value.hpp"

#include <string>
#include <ostream>

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

Value::Value(Type t) : type(t) {
  if (type == String) data.s = new QString;
  else if (type == Object) data.o = new Map;
  else if (type == Array) data.a = new Vector;
  else if (type == Double) data.d = 0.0;
  else if (type == Integer) data.i = 0;
}

Value::Value(double d) : type(Double) {
  data.d = d;
}

Value::Value(const QString& str) : type(String) {
  data.s = new QString(str);
}

Value::Value(const std::string& str) : type(String) {
  data.s = new QString(str.c_str());
}

Value::~Value() {
  if (type == String) delete data.s;
  else if (type == Object) delete data.o;
  else if (type == Array) delete data.a;
}

/// @todo actually use level for something
void Value::save(std::ostream& os, int /* level */) const {
  if (type == String) save_str(os, *data.s);
  else if (type == Double) os << data.d;
  else if (type == Integer) os << data.i;
  else if (type == Object) save_object(os, *data.o);
  else if (type == Array) save_array(os, *data.a);
  else if (type == True) os << "true";
  else if (type == False) os << "false";
  else if (type == Null) os << "null";
}
