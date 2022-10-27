#include "stdafx.h"
#include "SphericalMap.h"
#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif


SphericalMap::SphericalMap(const std::string& t_file_name)
{
	m_texture = new Texture(t_file_name.c_str());
}

Vector3 SphericalMap::getTexel(Vector3 direction) const
{
	float polar_angle = acos(direction.z);
	float azimut_angle = atan2f(direction.y, direction.z);
	if(direction.y < 0)
	{
		azimut_angle += 2.0f * M_PI;
	}
	float v = polar_angle / M_PI;
	float u = azimut_angle / (2.0f * M_PI);
	Color3f color = m_texture->get_texel(u, v);
	return Vector3{ color.r, color.g, color.b };
}
