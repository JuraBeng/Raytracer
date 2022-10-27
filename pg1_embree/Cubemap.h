#ifndef CUBEMAP_H_
#define CUBEMAP_H_
#include "texture.h"
#include "vector3.h"
#include <iostream>

class CubeMap
{
public:
	CubeMap(const std::string& t_pos_x, const std::string& t_neg_x, const std::string& t_pos_y,
		const std::string& t_neg_y, const std::string& t_pos_z, const std::string& t_neg_z);
	Vector3 getTexel(Vector3 & direction) const;

private:
	std::vector<Texture*> m_textures;
	enum COORDS
	{
		POSX, NEGX, POSY, NEGY, POSZ, NEGZ
	};
};
#endif


