#ifndef SHADER_H_
#define SHADER_H_
#include "material.h"
#include "vector3.h"

class Shader
{
public:
	virtual Vector3 useShader(RTCRayHit& t_rayhit, Vector3& t_normal, Vector3& t_hitpoint, Material& t_material, int t_depth, int t_max_depth) = 0;
};

#endif
