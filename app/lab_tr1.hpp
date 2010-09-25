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
