#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "forward.hpp"

#include <QString>

class Project {
public:
  ScenePtr load(const QString& filename);
};

#endif // PROJECT_HPP
