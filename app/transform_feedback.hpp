#ifndef TRANSFORM_FEEDBACK_HPP
#define TRANSFORM_FEEDBACK_HPP

#include "buffer_object.hpp"

#include <QString>

class TransformFeedback {
public:
  TransformFeedback();
  ~TransformFeedback();

  bool begin();
  bool end(float& out);

//  float readMacro(QString name);

private:
  BufferObject m_buffer;
  unsigned int m_query;
  unsigned int m_id;
};

#endif // TRANSFORM_FEEDBACK_HPP
