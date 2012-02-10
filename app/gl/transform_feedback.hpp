/**
 * Copyright 2010,2011 Riku Palomäki.
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

#ifndef TRANSFORM_FEEDBACK_HPP
#define TRANSFORM_FEEDBACK_HPP

#include "gl/buffer_object.hpp"

#include <QString>

class TransformFeedback {
public:
  TransformFeedback();
  ~TransformFeedback();

  bool begin(int primitive, int size);
  const float* map(int& size);
  void unmap();
  bool end(float* out, int size);

//  float readMacro(QString name);

private:
  BufferObject m_buffer;
  unsigned int m_query;
  unsigned int m_id;
};

#endif // TRANSFORM_FEEDBACK_HPP
