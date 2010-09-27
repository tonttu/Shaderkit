/**
 * Copyright 2010 Riku Palomäki.
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

#ifndef LAB_TR1_HPP
#define LAB_TR1_HPP

#include <cstddef>

#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
#include <memory>
#else

#if defined(__GNUC__) && __GNUC__ >= 4
#include <tr1/memory>
#else
#include <boost/tr1/memory.hpp>
#endif

namespace std {
  using tr1::shared_ptr;
  using tr1::enable_shared_from_this;
}
#endif

#endif
