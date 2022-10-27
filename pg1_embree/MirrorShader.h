#ifndef MIRRORSHADER_H_
#define MIRRORSHADER_H_
#include "vector3.h"
#include "material.h"
#include "rayops.h"
#include "Shader.h"
#include "raytracer.h"

class Raytracer;
class MirrorShader : public Shader
{
private:
	Raytracer* m_raytracer;
public:
	MirrorShader(Raytracer *t_raytracer);
	Vector3 useShader(RTCRayHit& t_rayhit, Vector3& t_normal, Vector3& t_hitpoint, Material& t_material, int t_depth, int t_max_depth) override;
};
#endif
