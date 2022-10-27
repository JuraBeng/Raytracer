#ifndef SPHERICALMAP_H_
#define SPHERICALMAP_H_
#include "texture.h"
#include "vector3.h"

class SphericalMap
{
public:
	SphericalMap(const std::string& t_file_name);
	Vector3 getTexel(Vector3 direction) const;

private:
	Texture * m_texture;
};
#endif


