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

  FileResource::FileResource(const FileResource& fr)
  {
    setFilename(fr.rawFilename());
  }

  FileResource::~FileResource()
  {
  }

  void FileResource::setFilename(const QString& filename)
  {
    QString abs;
    if (filename.isEmpty()) {
      abs = filename;
    } else {
      QFileInfo fi(filename);
      abs = fi.canonicalFilePath();
      if (abs.isEmpty()) {
        QFileInfo fi2("$scene/"+filename);
        abs = fi2.canonicalFilePath();
      }
      if (abs.isEmpty())
        abs = fi.absoluteFilePath();
    }

    if (m_rawFilename == filename && m_filenameAbsolute == abs)
      return;

    m_rawFilename = filename;
    m_filenameAbsolute = abs;
    filenameChanged();
  }

  const QString& FileResource::filename() const
  {
    return m_filenameAbsolute;
  }

  const QString& FileResource::rawFilename() const
  {
    return m_rawFilename;
  }

  FileResource& FileResource::operator=(const FileResource& fr)
  {
    setFilename(fr.rawFilename());
    return *this;
  }

} // namespace Shaderkit
