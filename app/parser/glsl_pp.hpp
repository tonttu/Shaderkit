/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLSL_PP_HPP
#define GLSL_PP_HPP

#include <QByteArray>

#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <stack>


union YYSTYPE;
typedef void* yyscan_t;

namespace Shaderkit
{
  class GLSLpp;
}

int ppparse(Shaderkit::GLSLpp&);

namespace Shaderkit
{

  class GLSLpp
  {
  public:
    GLSLpp();
    void reset();
    bool parse(QByteArray data);
    const std::string& out() const { return m_out; }

    /// GLSL version used, default is 0 (no version given)
    /// example: 150
    int version() const { return m_version; }
    /// GLSL version profile, for example "compatibility" or ""
    const std::string& profile() const { return m_profile; }

    enum ExtensionBehavior {
      Require,
      Enable,
      Warn,
      Disable
    };

    /// Extension name (case sensitive) -> behavior
    /// @todo are extensions really case sensitive?
    const std::map<std::string, ExtensionBehavior>& extensions() const { return m_extensions; }

    /// List of pragmas (content as string, line number)
    const std::list<std::pair<std::string, int>>& pragmas() const { return m_pragmas; }

    const std::set<std::string>& possibleUnknownMacros() const { return m_require; }

    struct MacroValue {
      MacroValue(const std::string& name_, const std::string& src_,
                 const std::string& partially_expanded_,
                 const std::vector<std::string>& params_ = std::vector<std::string>())
        : name(name_), src(src_), partially_expanded(partially_expanded_), params(params_) {}
      std::string name;
      std::string src;
      std::string partially_expanded;
      std::string value;
      std::vector<std::string> params;
      std::list<MacroValue> children;
    };

    // Indexing starts from 0
    const std::map<int, std::list<MacroValue>>& macros() const { return m_macros; }

    const std::vector<bool>& lineValues() const { return m_line_values; }

  private:
    std::string m_out;
    yyscan_t yyscanner;

    typedef std::map<std::string, std::string> Objs;
    struct Func {
      std::vector<std::pair<std::string, int>> chunks;
      std::string src;
      std::vector<std::string> params;
    };
    typedef std::map<std::string, Func> Funcs;

    Objs m_objs;
    Funcs m_funcs;
    //std::list<std::string> m_stack;

    int m_version;
    std::string m_profile;

    std::map<std::string, ExtensionBehavior> m_extensions;
    std::list<std::pair<std::string, int>> m_pragmas;
    std::map<int, std::list<MacroValue>> m_macros;

    MacroValue m_current_macro;
    int m_macro_line;
    std::stack<std::pair<MacroValue*, int>> m_macro_stack;

    std::set<std::string> m_undefs;
    std::set<std::string> m_require;

    std::vector<bool> m_line_values;
    bool m_last_status;

    std::string m_func;
    std::vector<std::string> m_args;
    std::string m_txt;
    int m_depth;

    int lex(YYSTYPE* lvalp);
    int lex_debug(YYSTYPE* lvalp);
    void error(GLSLpp& parser, const char* str);
    void pp_return(bool push, bool b);
    void push_string(const char* name, const std::string& src, const std::string& str,
                     const std::vector<std::string>& params = std::vector<std::string>());
    void changeState(bool push, int state);
    void pop();
    void fillLineValues(int new_state = -1);

    // index from 0
    int line() const;
    int column() const;
    void newline();

    friend int ::ppparse(GLSLpp&);
  };

} // namespace Shaderkit

#endif // GLSL_PP_HPP
