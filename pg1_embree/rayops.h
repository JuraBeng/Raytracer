#pragma once
#include "stdafx.h"
#include "vector3.h"

namespace ray_ops
{
	RTCRay generateRay(Vector3 &t_origin, Vector3 &t_direction, float t_ior = 1.0f, float t_tnear = 0.02f, float t_tfar = FLT_MAX);
	RTCRayHit generateRayHit(RTCRay& t_ray);
	RTCRayHit generateRayHit(Vector3& t_origin, Vector3& t_direction, float t_ior = 1.0f, float t_tnear = 0.02f, float t_tfar = FLT_MAX);
	Vector3 getRefractedRayDir(float &t_n1, float &t_n2, Vector3 &t_normal, Vector3 &t_view);
	Vector3 getRayHitPoint(RTCRayHit& t_ray_hit);
	void fresnel(float &t_n1, float &t_n2, Vector3 &t_normal, Vector3 &t_view, float *ptr_refl_coef, float *ptr_refr_coef);
	bool isRayIntersected(RTCRayHit& t_ray_hit, RTCScene* t_scene);

};
