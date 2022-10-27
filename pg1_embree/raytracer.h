#ifndef RAYTRACER_H_
#define RAYTRACER_H_
#include "simpleguidx11.h"
#include "surface.h"
#include "camera.h"
#include "Cubemap.h"
#include "mymath.h"
#include "Light.h"
#include "rayops.h"
#include "MirrorShader.h"
#include <random>

/*! \class Raytracer
\brief General ray tracer class.
\author Tomáš Fabián
\version 0.1
\date 2018
*/
class MirrorShader;

class Raytracer : public SimpleGuiDX11
{
public:
	Raytracer(const int width, const int height,
		const float fov_y, const Vector3 view_from, const Vector3 view_at);
	~Raytracer();

	int InitDeviceAndScene(const char* config);

	int ReleaseDeviceAndScene();

	void LoadScene(const std::string file_name, const char* config);

	Color4f get_pixel(const int x, const int y, const int tid, const float t = 0.0f) override;
	Vector3 TraceRay(RTCRay t_ray, int t_depth, int t_max_depth = 5);

	int Ui();

private:
	MirrorShader* m_mirror_shader;
	std::vector<Surface*> surfaces_;
	std::vector<Material*> materials_;
	bool* m_super_sampling;
	RTCDevice device_;
	RTCScene scene_;
	Camera camera_;
	Light m_light;
	CubeMap* m_cube_map;
	std::string options;
	std::string file_name;


	Material* getMaterial(RTCRayHit& t_ray_hit);

	Normal3f getNormalNorm3f(RTCRayHit& t_ray_hit);
	Vector3 getNormalVec3(RTCRayHit& t_ray_hit);

	Vector3 useOpaqueShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& material, int t_depth, int t_max_depth);
	Vector3 useMirrorShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& material, int t_depth, int t_max_depth);
	Vector3 useTransparentShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& material, int t_depth, int t_max_depth);



	float compressGamma(float t_color);
	float expandGamma(float t_color);
	Vector3 compressGamma(Vector3& const t_color);
	Vector3 expandGamma(Vector3& t_color);
	Vector3 mixSRGB(Vector3 &t_color1, Vector3 &t_color2, float &alpha);
	Vector3 mixLinear(Vector3 &t_color1, Vector3 &t_color2, float &alpha);

	int m_samples = 1;
	int m_width;
	int m_height;
	float m_spec_coef = 1.0f;
	float view_from_x, view_from_y, view_from_z;
	float view_at_x, view_at_y, view_at_z;
	float detail_x, detail_y, detail_z = 0;
	float detail_view_x, detail_view_y, detail_view_z = 0;
	bool m_quick_render = false;
	float alpha = 2.255f;
	bool show_material_window = false;
	bool show_view_window = false;
};
#endif
