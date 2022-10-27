#include "stdafx.h"
#include "camera.h"

# define M_PI           3.14159265358979323846  /* pi */

Camera::Camera( const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at )
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	f_y_ = height_ / (2.0 * tan((fov_y_ * (180.0/M_PI)) / 2.0));
	Vector3 z_c = view_from_ - view_at_;
	Vector3 x_c = up_.CrossProduct(z_c);
	Vector3 y_c = z_c.CrossProduct(x_c);

	x_c.Normalize();
	z_c.Normalize();
	y_c.Normalize();

	M_c_w_ = Matrix3x3( x_c, y_c, z_c );
}

RTCRay Camera::GenerateRay( const float x_i, const float y_i) const
{
	// setup a primary ray
	RTCRay ray;
	ray.org_x = view_from_.x; // ray origin
	ray.org_y = view_from_.y;
	ray.org_z = view_from_.z;
	ray.tnear = FLT_MIN; // start of ray segment

	Vector3 direction;
	direction.x = x_i - float(width_) / 2.0;
	direction.y = (float)height_ / 2.0 - y_i;
	direction.z = -f_y_;
	direction.Normalize();

	direction = M_c_w_ * direction;


	ray.dir_x = direction.x; // ray direction
	ray.dir_y = direction.y;
	ray.dir_z = direction.z;
	ray.time = 1.0f; // time of this ray for motion blur

	ray.tfar = FLT_MAX; // end of ray segment (set to hit distance)

	ray.mask = 0; // can be used to mask out some geometries for some rays
	ray.id = 0; // identify a ray inside a callback function
	ray.flags = 0; // reserved
	
	return ray;
}

Vector3 Camera::getViewFrom()
{
	return view_from_;
}
