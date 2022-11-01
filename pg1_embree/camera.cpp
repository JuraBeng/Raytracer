#include "stdafx.h"
#include "camera.h"

#include <iostream>

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

RTCRay Camera::GenerateRay( const float x_i, const float y_i, bool depth_of_field, float t_focal_rad, float t_aperture_rad) const
{
	Vector3 origin = { view_from_.x , view_from_.y , view_from_.z };
	Vector3 direction = { x_i - float(width_) / 2.0f, (float)height_ / 2.0f - y_i, -f_y_ };
	direction.Normalize();
	direction = M_c_w_ * direction;


	if (t_focal_rad > 0.0f && depth_of_field)
	{
		Vector3 observable_point = origin + direction * t_focal_rad;
		const float lower = 0.0f;
		const float upper = 1.0f;
		std::mt19937 generator(y_i * x_i + t_focal_rad * t_aperture_rad);
		std::uniform_real_distribution<float> unif_rad(lower, upper);
		std::uniform_real_distribution<float> unif_angle(-1000, 1000.f);

		float theta = 2 * M_PI * unif_angle(generator);
		float r = t_aperture_rad * sqrt(unif_rad(generator));
		origin.x = view_from_.x + r * cos(theta);
		origin.y = view_from_.y + r * sin(theta);

		direction = observable_point - origin;
		direction.Normalize();
	}


	// setup a primary ray
	RTCRay ray;
	ray.org_x = origin.x; // ray origin
	ray.org_y = origin.y;
	ray.org_z = origin.z;
	ray.tnear = FLT_MIN; // start of ray segment


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
