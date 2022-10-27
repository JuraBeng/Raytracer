#include "stdafx.h"
#include "rayops.h"

RTCRay ray_ops::generateRay(Vector3& t_origin, Vector3& t_direction, float t_ior, float t_tnear, float t_tfar)
{
	RTCRay ray;
	ray.org_x = t_origin.x; // ray origin
	ray.org_y = t_origin.y;
	ray.org_z = t_origin.z;
	ray.tnear = t_tnear; // start of ray segment

	ray.dir_x = t_direction.x; // ray direction
	ray.dir_y = t_direction.y;
	ray.dir_z = t_direction.z;
	ray.time = t_ior; // time of this ray for motion blur

	ray.tfar = t_tfar; // end of ray segment (set to hit distance)

	ray.mask = 0; // can be used to mask out some geometries for some rays
	ray.id = 0; // identify a ray inside a callback function
	ray.flags = 0; // reserved

	return ray;
}

RTCRayHit ray_ops::generateRayHit(RTCRay& t_ray)
{
		RTCHit hit;
	hit.geomID = RTC_INVALID_GEOMETRY_ID;
	hit.primID = RTC_INVALID_GEOMETRY_ID;
	hit.Ng_x = 0.0f; // geometry normal
	hit.Ng_y = 0.0f;
	hit.Ng_z = 0.0f;

	// merge ray and hit structures
	RTCRayHit ray_hit;
	ray_hit.ray = t_ray;
	ray_hit.hit = hit;

	return ray_hit;
}

RTCRayHit ray_ops::generateRayHit(Vector3& t_origin, Vector3& t_direction, float t_ior, float t_tnear, float t_tfar)
{
	RTCRay ray = generateRay(t_origin, t_direction, t_ior, t_tnear, t_tfar);
	return generateRayHit(ray);
}

Vector3 ray_ops::getRefractedRayDir(float& t_n1, float& t_n2, Vector3& t_normal, Vector3& t_view)
{
	float etai_over_etat = t_n1 / t_n2;
	float cos_theta1 = min(1.0f, t_normal.DotProduct(-t_view));

	float cos_theta2 =
	{
		1.0f - ((t_n1 / t_n2) * (t_n1 / t_n2)) * (max(0.0f, 1.0f - (cos_theta1 * cos_theta1)))
	};

	if (cos_theta2 < 0)
		return Vector3{ 0.0f, 0.0f, 0.0f };

	Vector3 refr_dir = {
		(t_n1 / t_n2) * t_view + ((t_n1 / t_n2) * cos_theta1 - sqrtf(cos_theta2)) * t_normal
	};
	return refr_dir;
}

bool ray_ops::isRayIntersected(RTCRayHit& t_ray_hit, RTCScene *t_scene)
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	rtcIntersect1(*t_scene, &context, &t_ray_hit);
	return t_ray_hit.hit.geomID != RTC_INVALID_GEOMETRY_ID;
}

Vector3 ray_ops::getRayHitPoint(RTCRayHit& t_ray_hit)
{
	Vector3 origin = Vector3(t_ray_hit.ray.org_x, t_ray_hit.ray.org_y, t_ray_hit.ray.org_z);
	Vector3 direction = Vector3(t_ray_hit.ray.dir_x, t_ray_hit.ray.dir_y, t_ray_hit.ray.dir_z);
	return origin + direction * t_ray_hit.ray.tfar;
}

void ray_ops::fresnel(float& t_n1, float& t_n2, Vector3& t_normal, Vector3& t_view, float* ptr_refl_coef,
	float* ptr_refr_coef)
{
	float n1_div_n2 = t_n1 / t_n2;
	float cos_theta1 = t_normal.DotProduct(-t_view);

	float tmp = 1.0f - (n1_div_n2 * n1_div_n2) * (max(0.0f, 1.0f - (cos_theta1 * cos_theta1)));
	if (tmp < 0)
	{
		*ptr_refl_coef = 1.0f;
		*ptr_refr_coef = 0.0f;
		return;
	}
	float cos_theta2 =
	{
		sqrtf(tmp)
	};

	float r_s =
	{
		((t_n2 * cos_theta2 - t_n1 * cos_theta1) / (t_n2 * cos_theta2 + t_n1 * cos_theta1)) *
		((t_n2 * cos_theta2 - t_n1 * cos_theta1) / (t_n2 * cos_theta2 + t_n1 * cos_theta1))
	};

	float r_p =
	{
		((t_n2 * cos_theta1 - t_n1 * cos_theta2) / (t_n2 * cos_theta1 + t_n1 * cos_theta2)) *
		((t_n2 * cos_theta1 - t_n1 * cos_theta2) / (t_n2 * cos_theta1 + t_n1 * cos_theta2))
	};
	*ptr_refl_coef = (r_s + r_p) / 2.0f;
	*ptr_refr_coef = 1.0f - (*ptr_refl_coef);
	return;
}
