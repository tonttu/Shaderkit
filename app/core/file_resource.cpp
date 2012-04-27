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

#include "core/file_resource.hpp"

#include <QFileInfo>

namespace Shaderkit
{

  FileResource::FileResource()
  {
  }

  FileResource::~FileResource()
  {
  }

  void FileResource::setFilename(const QString& filename)
  {
    m_rawFilename = filename;
    if (filename.isEmpty()) {
      m_filenameAbsolute = filename;
      return;
    }
    QFileInfo fi(filename);
    m_filenameAbsolute = fi.canonicalFilePath();
    if (m_filenameAbsolute.isEmpty()) {
      QFileInfo fi2("$scene/"+filename);
      m_filenameAbsolute = fi2.canonicalFilePath();
    }
    if (m_filenameAbsolute.isEmpty())
      m_filenameAbsolute = fi.absoluteFilePath();
  }

  const QString& FileResource::filename() const
  {
    return m_filenameAbsolute;
  }

  const QString& FileResource::rawFilename() const
  {
    return m_rawFilename;
  }

} // namespace Shaderkit
