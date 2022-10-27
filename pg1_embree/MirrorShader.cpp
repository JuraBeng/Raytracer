#include "stdafx.h"
#include "MirrorShader.h"

MirrorShader::MirrorShader(Raytracer* t_raytracer)
{
	m_raytracer = t_raytracer;
}

Vector3 MirrorShader::useShader(RTCRayHit& t_rayhit, Vector3& t_normal, Vector3& t_hitpoint, Material& t_material,
                                int t_depth, int t_max_depth)
{
	Vector3 view_dir = { t_rayhit.ray.dir_x, t_rayhit.ray.dir_y, t_rayhit.ray.dir_z };
	Vector3 reflect_dir = view_dir.ReflectTo(t_normal);
	RTCRay refl_ray = ray_ops::generateRay(t_hitpoint, reflect_dir, t_rayhit.ray.time, 0.02f);
	return m_raytracer->TraceRay(refl_ray, t_depth + 1, t_max_depth);
}
