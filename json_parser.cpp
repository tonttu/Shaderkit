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

#include <QDebug>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
#include <boost/lambda/lambda.hpp>

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>


namespace qi = boost::spirit::qi;
namespace cs = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/**
 * JSON Boost Spirit Parser.
 *
 * @todo in very small scale tests this was dramatically slower to compile and
 *       to run than a simple Bison parser. Find out, and fix or replace with
 *       Bison if this is too slow. Also Bison version would be so much easier
 *       to debug and develop. This kind of C++ abuse should be illegal.
 */
template <typename Iterator>
struct Parser : qi::grammar<Iterator, cs::space_type, Value()> {

  struct ToValue {
    template <typename> struct result { typedef Value type; };

    template <typename T> Value operator()(T t) const {
      return Value(t);
    }
  };
  phoenix::function<ToValue> tovalue;

  static Value::Map& getMap(Value v) { return v.getMap(); }
  static Value::Vector& getArray(Value v) { return v.getArray(); }
  static Value create(Value::Type type) { return Value(type); }
  static void add(Value::Map& map, const std::string& key, Value v) { map[QString::fromStdString(key)] = v; }

  Parser() : Parser::base_type(object, "json") {
    using boost::spirit::qi::double_;
    using qi::int_;
    using qi::eps;
    using cs::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::debug;
    using qi::on_error;
    using qi::fail;
    using phoenix::val;
    using namespace qi::labels;
    using phoenix::construct;
    using phoenix::bind;
    using phoenix::push_back;
    namespace lambda = boost::lambda;

    object = lit('{')[_val = bind(&create, Value::Object)] >> -objlist(bind(&getMap, _val)) >> '}';
    objlist = ((string[_a = _1] >> ':' >> value[bind(&add, _r1, _a, _1)]) % ',');

    array = lit('[')[_val = bind(&create, Value::Array)] >> -arraylist(bind(&getArray, _val)) >> ']';
    arraylist = value[push_back(_r1, _1)] % ',';

    string = lexeme['"' >> *(char_ - '"') >> '"'];
    value = //int_[_val = tovalue(_1)]
        double_[_val = tovalue(_1)]
        | string[_val = tovalue(_1)]
        | lit("true")[_val = bind(&create, Value::True)]
        | lit("false")[_val = bind(&create, Value::False)]
        | lit("null")[_val = bind(&create, Value::Null)]
        | object[_val = _1]
        | array[_val = _1];

    object.name("object");
    array.name("array");
    value.name("value");
    objlist.name("objlist");
    arraylist.name("arraylist");
    string.name("string");

    on_error<fail>
        (object, std::cout << val("Error! Expecting ") << _4 << val(" here: \"")
         << construct<std::string>(_3, _2) << val("\"") << std::endl);
  }

  qi::rule<Iterator, cs::space_type, Value()> object, array;
  qi::rule<Iterator, cs::space_type, Value()> value;
  qi::rule<Iterator, cs::space_type, void(Value::Map&), qi::locals<std::string> > objlist;
  qi::rule<Iterator, cs::space_type, void(Value::Vector&)> arraylist;
  qi::rule<Iterator, cs::space_type, std::string()> string;
};

bool Value::load(const QString& filename) {
  std::ifstream file(filename.toStdString().c_str());
  if (!file) {
    qDebug() << "Error, couldn't open the project file" << filename;
    return false;
  }

  std::string data;
  file.unsetf(std::ios::skipws);
  std::copy(std::istream_iterator<char>(file),
            std::istream_iterator<char>(),
            std::back_inserter(data));

  typedef std::string::const_iterator It;
  It b = data.begin(), e = data.end();
  Parser<It> parser;

  Value tmp(Object);
  if (qi::phrase_parse(b, e, parser, cs::space, tmp) && b == e) {
    *this = tmp;
    return true;
  } else {
    qDebug() << "Parsing error with file" << filename;
  }
  return false;
}
