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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "forward.hpp"
#include "gl/scene_object.hpp"

#include "core/attribute.hpp"

#include "Eigen/Geometry"

#include <QVariant>

#ifdef _MSC_VER
#undef near
#undef far
#endif

namespace Shaderkit
{
  /**
   * OpenGL Camera class, handles perspective and ortho projections.
   */
  class Camera : public QObject, public std::enable_shared_from_this<Camera>,
      public SceneObject
  {
    Q_OBJECT

  public:
    enum Type { Perspective, Ortho, Rect };

    Camera(const QString& name);

    /// Set the camera with screen size width x height
    void prepare(int width, int height);

    QVariantMap toMap() const;

    /// Load the camera from map
    void load(QVariantMap map);

    CameraPtr clone() const;

    QIcon icon();

    float width() const { return m_width; }
    float height() const { return m_height; }

    /// Sets this camera to use orthographic matrix that fills the screen
    void setRect(float near = -1.0f, float far = 1.0f);

    void setNear(float near);
    float near() const { return m_near.value(); }

    void setFar(float far);
    float far() const { return m_far.value(); }

    void setFov(float fov);
    float fov() const { return m_fov.value(); }

    void rotate(const Eigen::Vector2f& diff);
    void translate(const Eigen::Vector2f& diff);
    void zoom(float diff);

    void setTarget(const Eigen::Vector3f& target);
    const Eigen::Vector3f& target() const;

    void setLocation(const Eigen::Vector3f& location);
    const Eigen::Vector3f location() const;

    const Eigen::Vector3f up() const;
    const Eigen::Vector3f right() const;
    const Eigen::Vector3f front() const;

    Type type() const { return m_type.value(); }
    void setType(Type type);

    float dist() const;

    //void setPickDisplay(float x, float y);

    const Eigen::Projective3f& projection() const { return m_projection; }

    const Eigen::Affine3f& view() const { return m_view; }

    /// @param swap_y enable if you want to project to Qt coordinates
    Eigen::Projective3f normToWindow(bool swap_y = false) const;

    /// Matrix from view coordinates to window coordinates
    /// @param swap_y enable if you want to project to Qt coordinates
    Eigen::Projective3f transform(bool swap_y = false) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    virtual void attributeChanged();

  signals:
    void changed(CameraPtr);

  protected:
    explicit Camera(const Camera& c);
    void updateVectors();

  protected:
    Attribute<Type> m_type;

    /// The point the camera is looking at (Perspective)
    Attribute<Eigen::Vector3f> m_target;

    /// Camera up vector (Perspective)
    /// There are cached values
    Eigen::Vector3f m_up;
    Eigen::Vector3f m_right;
    Eigen::Vector3f m_front;

    Attribute<float> m_dx, m_dy, m_dist;

    Eigen::Projective3f m_projection;
    Eigen::Affine3f m_view;

    float m_width, m_height;

    /// Field of view, in degrees, in the y direction (Perspective)
    Attribute<float> m_fov;
    /// Near clipping plane distance
    Attribute<float> m_near;
    /// Far clipping plane distance
    Attribute<float> m_far;
  };

} // namespace Shaderkit

#endif // CAMERA_HPP
