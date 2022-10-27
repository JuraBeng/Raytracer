#include "stdafx.h"
#include "Cubemap.h"

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif


CubeMap::CubeMap(const std::string& t_pos_x, const std::string& t_neg_x, const std::string& t_pos_y,
	const std::string& t_neg_y, const std::string& t_pos_z, const std::string& t_neg_z)
{
	m_textures.push_back(new Texture(t_pos_x.c_str()));
	m_textures.push_back(new Texture(t_neg_x.c_str()));
	m_textures.push_back(new Texture(t_pos_y.c_str()));
	m_textures.push_back(new Texture(t_neg_y.c_str()));
	m_textures.push_back(new Texture(t_pos_z.c_str()));
	m_textures.push_back(new Texture(t_neg_z.c_str()));
}

Vector3 CubeMap::getTexel(Vector3 & direction) const
{
	char largest_component = direction.LargestComponent(true);
	float u, v;
	int idx;
	if (largest_component == 0)
	{
		idx = direction.x < 0 ? 1 : 0;
		const float tmp = 1.0f / abs(direction.x);
		u = idx == 1 ? (direction.y * tmp + 1.0f) * 0.5f : 1.0f - (direction.y * tmp + 1.0f) * 0.5f;
		v = 1.0f - (direction.z * tmp + 1.0f) * 0.5f;
	}
	if (largest_component == 1)
	{
		idx = direction.y < 0 ? 5 : 4;
		const float tmp = 1.0f / abs(direction.y);
		//u = idx == 5 ? (direction.z * tmp + 1.0f) * 0.5f : 1.0f - (direction.z * tmp + 1.0f) * 0.5f;
		u = idx == 4 ? (direction.x * tmp + 1.0f) * 0.5f : 1.0f - (direction.x * tmp + 1.0f) * 0.5f;
		v = 1.0f - (direction.z * tmp + 1.0f) * 0.5f;
	}
	if (largest_component == 2)
	{
		idx = direction.z < 0 ? 3 : 2;
		const float tmp = 1.0f / abs(direction.z);
		u = (direction.x * tmp + 1.0f) * 0.5f;
		v = idx == 2 ? (direction.y * tmp + 1.0f) * 0.5f : 1.0f - (direction.y * tmp + 1.0f) * 0.5f;
	}
	Color3f output_color = m_textures[idx]->get_texel(u, v);
	return Vector3{ output_color.r, output_color.g, output_color.b };
}