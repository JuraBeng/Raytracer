#include "stdafx.h"
#include "raytracer.h"

#include <iostream>

#include "objloader.h"
#include "tutorials.h"

Raytracer::Raytracer(const int width, const int height,
	const float fov_y, const Vector3 view_from, const Vector3 view_at) : SimpleGuiDX11(width, height)
{
	m_cube_map = new CubeMap(
		"../../../data/posx.jpg", 
		"../../../data/negx.jpg", 
		"../../../data/posy.jpg", 
		"../../../data/negy.jpg",
		"../../../data/posz.jpg",
		"../../../data/negz.jpg"
	);
	m_mirror_shader = new MirrorShader(this);
	m_light = Light(Vector3{ 313, - 1200, 1200 }, Vector3{ 1.0f, 1.0f, 1.0f }, Vector3{1.0f, 1.0f, 1.0f});
	m_super_sampling = new bool;
	m_super_sampling = false;
	m_samples = 1;
	m_width = width;
	m_height = height;
	camera_ = Camera(width, height, fov_y, view_from, view_at);
	view_from_x = view_from.x;
	view_from_y = view_from.y;
	view_from_z = view_from.z;
	view_at_x = view_at.x;
	view_at_y = view_at.y;
	view_at_z = view_at.z;
}

Raytracer::~Raytracer()
{
	ReleaseDeviceAndScene();
}

int Raytracer::InitDeviceAndScene(const char* config)
{
	device_ = rtcNewDevice(config);
	error_handler(nullptr, rtcGetDeviceError(device_), "Unable to create a new device.\n");
	rtcSetDeviceErrorFunction(device_, error_handler, nullptr);

	ssize_t triangle_supported = rtcGetDeviceProperty(device_, RTC_DEVICE_PROPERTY_TRIANGLE_GEOMETRY_SUPPORTED);

	// create a new scene bound to the specified device
	scene_ = rtcNewScene(device_);

	return S_OK;
}

int Raytracer::ReleaseDeviceAndScene()
{
	rtcReleaseScene(scene_);
	rtcReleaseDevice(device_);

	return S_OK;
}

void Raytracer::LoadScene(const std::string file_name, const char* config = "threads=0,verbose=3")
{
	options = std::string(config);
	this->file_name = file_name;
	InitDeviceAndScene(options.c_str());
	const int no_surfaces = LoadOBJ(this->file_name.c_str(), surfaces_, materials_);
	const int no_surfaces1 = LoadOBJ(this->file_name.c_str(), surfaces2_, materials_);
	// surfaces loop
	for (auto surface : surfaces_)
	{
		RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

		Vertex3f* vertices = (Vertex3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
			sizeof(Vertex3f), 3 * surface->no_triangles());

		Triangle3ui* triangles = (Triangle3ui*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
			sizeof(Triangle3ui), surface->no_triangles());

		rtcSetGeometryUserData(mesh, (void*)(surface->get_material()));

		rtcSetGeometryVertexAttributeCount(mesh, 2);

		Normal3f* normals = (Normal3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3,
			sizeof(Normal3f), 3 * surface->no_triangles());

		Coord2f* tex_coords = (Coord2f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2,
			sizeof(Coord2f), 3 * surface->no_triangles());

		// triangles loop
		for (int i = 0, k = 0; i < surface->no_triangles(); ++i)
		{
			Triangle& triangle = surface->get_triangle(i);

			// vertices loop
			for (int j = 0; j < 3; ++j, ++k)
			{
				const Vertex& vertex = triangle.vertex(j);

				vertices[k].x = vertex.position.x;
				vertices[k].y = vertex.position.y;
				vertices[k].z = vertex.position.z;

				normals[k].x = vertex.normal.x;
				normals[k].y = vertex.normal.y;
				normals[k].z = vertex.normal.z;

				tex_coords[k].u = vertex.texture_coords[0].u;
				tex_coords[k].v = vertex.texture_coords[0].v;
			} // end of vertices loop

			triangles[i].v0 = k - 3;
			triangles[i].v1 = k - 2;
			triangles[i].v2 = k - 1;
		} // end of triangles loop

		rtcCommitGeometry(mesh);
		unsigned int geom_id = rtcAttachGeometry(scene_, mesh);
		rtcReleaseGeometry(mesh);
	} // end of surfaces loop

	for (auto surface : surfaces2_)
	{
		RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

		Vertex3f* vertices = (Vertex3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
			sizeof(Vertex3f), 3 * surface->no_triangles());

		Triangle3ui* triangles = (Triangle3ui*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
			sizeof(Triangle3ui), surface->no_triangles());

		rtcSetGeometryUserData(mesh, (void*)(surface->get_material()));

		rtcSetGeometryVertexAttributeCount(mesh, 2);

		Normal3f* normals = (Normal3f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3,
			sizeof(Normal3f), 3 * surface->no_triangles());

		Coord2f* tex_coords = (Coord2f*)rtcSetNewGeometryBuffer(
			mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2,
			sizeof(Coord2f), 3 * surface->no_triangles());

		// triangles loop
		for (int i = 0, k = 0; i < surface->no_triangles(); ++i)
		{
			Triangle& triangle = surface->get_triangle(i);

			// vertices loop
			for (int j = 0; j < 3; ++j, ++k)
			{
				const Vertex& vertex = triangle.vertex(j);

				vertices[k].x = vertex.position.x - 150.0f;
				vertices[k].y = vertex.position.y + 100.0f;
				vertices[k].z = vertex.position.z + 50.0f;

				normals[k].x = vertex.normal.x;
				normals[k].y = vertex.normal.y;
				normals[k].z = vertex.normal.z;

				tex_coords[k].u = vertex.texture_coords[0].u;
				tex_coords[k].v = vertex.texture_coords[0].v;
			} // end of vertices loop

			triangles[i].v0 = k - 3;
			triangles[i].v1 = k - 2;
			triangles[i].v2 = k - 1;
		} // end of triangles loop

		rtcCommitGeometry(mesh);
		unsigned int geom_id = rtcAttachGeometry(scene_, mesh);
		rtcReleaseGeometry(mesh);
	} // end of surfaces loop

	rtcCommitScene(scene_);
}

Color4f Raytracer::get_pixel(const int x, const int y, const int tid,  const float t)
{
	Vector3 color = { 0.0f, 0.0f, 0.0f };
	if (m_samples > 1)
	{
		const float lower = 0.0f;
		const float upper = 1.0f;
		std::mt19937 generator(tid + m_samples * 7);
		std::uniform_real_distribution<float> unif_dist(lower, upper);

		for (int i = 0; i < m_samples; i++)
		{
			float x_offset = unif_dist(generator);
			float y_offset = unif_dist(generator);
			RTCRay ray = camera_.GenerateRay(x + x_offset, y + y_offset, depth_of_field,  m_rad_size, m_aperture_size);
			color += TraceRay(ray, 0, 6);
		}
		color /= m_samples;
	}
	else
	{
		RTCRay ray = camera_.GenerateRay(x, y, depth_of_field, m_rad_size, m_aperture_size);
		color = TraceRay(ray, 0, 6);
	}
	return Color4f{ color, 1.0f };
}

int Raytracer::Ui()
{
	Vector3 view_from = { view_from_x + detail_x, view_from_y + detail_y, view_from_z + detail_z };
	Vector3 view_at = { view_at_x + detail_view_x, view_at_y + detail_view_y, view_at_z + detail_view_z };
	ImGui::Begin("Switch window"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::Text("Switch visibility of windows below");
	if (ImGui::Button("Open view window"))
		show_view_window = !show_view_window;
	if (ImGui::Button("Open material window"))
		show_material_window = !show_material_window;
	if (ImGui::Button("Open graphics window"))
		show_graphics_window = !show_graphics_window;
	ImGui::End();

	if (show_view_window)
	{
		ImGui::Begin("View window", &show_view_window);
		Vector3 m_pos = m_light.getLightPosition();
		ImGui::SliderFloat("Light x", &m_pos.x, -1200.0f, 1200.0f);
		ImGui::SliderFloat("Light y", &m_pos.y, -1200.0f, 1200.0f);
		ImGui::SliderFloat("Light z", &m_pos.z, -1200.0f, 1200.0f);
		m_light.setLightPosition(m_pos);
		ImGui::Separator();

		ImGui::SliderFloat("View From X", &view_from_x, -300.0f, 300.0f);
		ImGui::SliderFloat("Detail X", &detail_x, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::SliderFloat("View From Y", &view_from_y, -300.0f, 300.0f);
		ImGui::SliderFloat("Detail Y", &detail_y, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::SliderFloat("View From Z", &view_from_z, -300.0f, 300.0f);
		ImGui::SliderFloat("Detail Z", &detail_z, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::SliderFloat("View At X", &view_at_x, -300.0f, 300.0f);
		ImGui::SliderFloat("View Detail X", &detail_view_x, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::SliderFloat("View At Y", &view_at_y, -300.0f, 300.0f);
		ImGui::SliderFloat("View Detail Y", &detail_view_y, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::SliderFloat("View At Z", &view_at_z, -300.0f, 300.0f);
		ImGui::SliderFloat("View Detail Z", &detail_view_z, -3.0f, 3.0f);
		ImGui::Separator();

		ImGui::End();
	}
	if (show_material_window)
	{
		ImGui::Begin("Material window");
		for (auto& mat : materials_)
		{
			std::string name = mat->get_name();
			ImGui::Text(("Material " + name).c_str());

			ImGui::BeginChild(name.c_str(), ImVec2{ 200, 200 }, true);

				ImGui::SliderFloat("IOR", &mat->ior, 0.2f, 4.0f);
				float cols[3] = { mat->diffuse.x, mat->diffuse.y, mat->diffuse.z };
				ImGui::ColorEdit3("DIFFUSE", cols);
				mat->diffuse = { cols[0], cols[1], cols[2] };

			ImGui::EndChild();
		}

		ImGui::End();
	}

	if (show_graphics_window)
	{
		ImGui::Begin("Graphics settings window");
		ImGui::Checkbox("SSAA", &m_super_sampling);
		if (m_super_sampling)
		{
			if (m_samples == 1)
				m_samples = 4;
			ImGui::SameLine();
			if (ImGui::Button("x2"))
			{
				m_samples = 4;
			}
			ImGui::SameLine();
			if (ImGui::Button("x4"))
			{
				m_samples = 16;
			}
			ImGui::SameLine();
			if (ImGui::Button("x8"))
			{
				m_samples = 64;
			}
			ImGui::SameLine();
			if (ImGui::Button("x16"))
			{
				m_samples = 128;
			}
		}
		else
		{
			m_samples = 1;
		}
		ImGui::Separator();
		ImGui::Checkbox("Depth of field", &depth_of_field);
		if(depth_of_field)
		{
			ImGui::InputFloat("Radius", &m_rad_size);
			ImGui::InputFloat("Aperture", &m_aperture_size);
		}
		ImGui::Separator();
		ImGui::End();
	}


	// Use a Begin/End pair to created a named window
	ImGui::Begin("Ray Tracer Params");

	ImGui::Text("Surfaces = %d", surfaces_.size());
	ImGui::Text("Materials = %d", materials_.size());
	ImGui::Separator();
	ImGui::Checkbox("Vsync", &vsync_);
	ImGui::Checkbox("Fast render", &m_quick_render);
	if(m_quick_render)
	{
		ImGui::SameLine();
		ImGui::Checkbox("Faster render", &quickest);
	}

	if(depth_of_field)
		ImGui::Text("Using Depth of field with rad %.2f and aperture %.2f", m_rad_size, m_aperture_size);
	if(m_super_sampling)
		ImGui::Text("Using SSAA with %d samples per pixel", m_samples);
	ImGui::Text("Rendering for %dX%d size.", m_width, m_height);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	camera_ = Camera(m_width, m_height, deg2rad(45.0), view_from, view_at);
	return 0;
}


Material* Raytracer::getMaterial(RTCRayHit& t_ray_hit)
{
	RTCGeometry geometry = rtcGetGeometry(scene_, t_ray_hit.hit.geomID);
	Material* material = (Material*)rtcGetGeometryUserData(geometry);
	return material;
}

Normal3f Raytracer::getNormalNorm3f(RTCRayHit& t_ray_hit)
{
	RTCGeometry geometry = rtcGetGeometry(scene_, t_ray_hit.hit.geomID);
	Normal3f normal;
	rtcInterpolate0(geometry, t_ray_hit.hit.primID, t_ray_hit.hit.u, t_ray_hit.hit.v,
		RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
	return normal;
}

Vector3 Raytracer::getNormalVec3(RTCRayHit& t_ray_hit)
{
	Normal3f normal;
	RTCGeometry geometry = rtcGetGeometry(scene_, t_ray_hit.hit.geomID);
	rtcInterpolate0(geometry, t_ray_hit.hit.primID, t_ray_hit.hit.u, t_ray_hit.hit.v,
		RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
	Vector3 view_dir = { t_ray_hit.ray.dir_x, t_ray_hit.ray.dir_y, t_ray_hit.ray.dir_z };
	Vector3 vec3_normal = { normal.x, normal.y, normal.z };
	return view_dir.DotProduct(vec3_normal) > 0 ? vec3_normal * -1 : vec3_normal;
}


Vector3 Raytracer::TraceRay(RTCRay t_ray, int t_depth, int t_max_depth)
{
	Vector3 output_color;
	Vector3 view_dir = { t_ray.dir_x, t_ray.dir_y, t_ray.dir_z };
	Vector3 background_color = m_cube_map->getTexel(view_dir);
	output_color = background_color;
	if(t_depth <= t_max_depth)
	{
		RTCRayHit ray_hit = ray_ops::generateRayHit(t_ray);
		if(ray_ops::isRayIntersected(ray_hit, &scene_))
		{
			Material* material = getMaterial(ray_hit);
			Vector3 normal = getNormalVec3(ray_hit);
			Vector3 hit_point = ray_ops::getRayHitPoint(ray_hit);
			if (m_quick_render)
			{
				Vector3 out = { 1.0f, 1.0f, 1.0f };
				if(!quickest)
				{
					Vector3 dir_to_light = hit_point.FromDirectionTo(m_light.getLightPosition());
					RTCRayHit shadow_ray = ray_ops::generateRayHit(hit_point, dir_to_light);
					out = Vector3{ 1.0f, 1.0f, 1.0f } *max(0.0f, normal.DotProduct(dir_to_light)) *
						!ray_ops::isRayIntersected(shadow_ray, &scene_);
				}
				return out;
			}

			

			if(material->shader == 1)
				output_color = useOpaqueShader(ray_hit, normal, hit_point, *material, t_depth, t_max_depth);
			if (material->shader == 2)
				output_color = m_mirror_shader->useShader(ray_hit, normal, hit_point, *material, t_depth, t_max_depth);
			//useMirrorShader(ray_hit, normal, hit_point, *material, t_depth, t_max_depth);
			if (material->shader == 3)
				output_color = useTransparentShader(ray_hit, normal, hit_point, *material, t_depth, t_max_depth);
		}

	}
	return output_color;
}

Vector3 Raytracer::useMirrorShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& t_material, int t_depth, int t_max_depth)
{
	Vector3 view_dir = { t_rayhit.ray.dir_x, t_rayhit.ray.dir_y, t_rayhit.ray.dir_z };
	Vector3 reflect_dir = view_dir.ReflectTo(t_normal);
	RTCRay refl_ray = ray_ops::generateRay(t_hitpoint, reflect_dir, t_rayhit.ray.time, 0.02f);
	return TraceRay(refl_ray, t_depth + 1, t_max_depth);
}

Vector3 Raytracer::useTransparentShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& t_material, int t_depth, int t_max_depth)
{
	float refl_coef, refr_coef;

	Vector3 attenuation = {
	exp(-t_material.attenuation.x * t_rayhit.ray.tfar),
	exp(-t_material.attenuation.y * t_rayhit.ray.tfar),
	exp(-t_material.attenuation.z * t_rayhit.ray.tfar)
	};

	Vector3 view_dir = { t_rayhit.ray.dir_x, t_rayhit.ray.dir_y, t_rayhit.ray.dir_z };

	float incident_ior = t_rayhit.ray.time;
	float outgoing_ior = t_material.ior;
	if (incident_ior == outgoing_ior)
	{
		outgoing_ior = 1.0f;
	}

	Vector3 reflect_dir = view_dir.ReflectTo(t_normal);
	RTCRay refl_ray = ray_ops::generateRay(t_hitpoint, reflect_dir, t_rayhit.ray.time, 0.02f);
	Vector3 refracted_dir = ray_ops::getRefractedRayDir(incident_ior, outgoing_ior, t_normal, view_dir);
	RTCRay refr_ray = ray_ops::generateRay(t_hitpoint, refracted_dir, outgoing_ior, 0.02f);
	ray_ops::fresnel(incident_ior, outgoing_ior, t_normal, view_dir, &refl_coef, &refr_coef);

	Vector3 specular = TraceRay(refl_ray, t_depth + 1, t_max_depth) * refl_coef;
	Vector3 refracted = TraceRay(refr_ray, t_depth + 1, t_max_depth) * refr_coef * attenuation;

	return refracted + specular;
}

Vector3 Raytracer::useOpaqueShader(RTCRayHit &t_rayhit, Vector3 &t_normal, Vector3 &t_hitpoint, Material& t_material, int t_depth, int t_max_depth)
{
	Vector3 ambient, diffuse, specular, reflected;
	float refl_coef, refr_coef;

	Vector3 dir_to_light = t_hitpoint.FromDirectionTo(m_light.getLightPosition());
	Vector3 view_dir = { t_rayhit.ray.dir_x, t_rayhit.ray.dir_y, t_rayhit.ray.dir_z };
	Vector3 reflect_dir = view_dir.ReflectTo(t_normal);

	RTCRay refl_ray = ray_ops::generateRay(t_hitpoint, reflect_dir, t_rayhit.ray.time, 0.02f);
	ray_ops::fresnel(t_rayhit.ray.time, t_material.ior, t_normal, view_dir, &refl_coef, &refr_coef);
	float spec_shine = powf(max(0.0f, reflect_dir.DotProduct(dir_to_light)), t_material.shininess);

	RTCRayHit shadow_ray = ray_ops::generateRayHit(t_hitpoint, dir_to_light);
	ambient = t_material.ambient;
	diffuse = t_material.diffuse * max(0.0f, t_normal.DotProduct(dir_to_light));
	specular = spec_shine * t_material.specular;
	reflected = TraceRay(refl_ray, t_depth + 1, t_max_depth) * refl_coef;

	return {
		m_light.getLightIntensity() *
		( ambient + (!ray_ops::isRayIntersected(shadow_ray, &scene_) + 0.2) * (diffuse + specular)) + reflected
		//(mixSRGB(diffuse, specular, alpha)))
	};
}

float Raytracer::compressGamma(float t_color)
{
	if (t_color <= 0.0f) return 0.0f;
	if (t_color >= 1.0f) return 1.0f;
	if (t_color <= 0.0031308f)
	{
		return 12.92 * t_color;
	}
	else
	{
		const float a = 0.055f;
		return (1.0f + a) * powf(t_color, 1.0f / 2.4f) - a;
	}
}

float Raytracer::expandGamma(float t_color)
{
	if (t_color <= 0.0f) return 0.0f;
	if (t_color >= 1.0f) return 1.0f;
	if (t_color <= 0.04045f)
	{
		return t_color / 12.92;
	}
	else
	{
		const float a = 0.055f;
		return powf(((t_color + a) / 1.055f), 2.4f);
	}
}

Vector3 Raytracer::compressGamma(Vector3& t_color)
{
	Vector3 out =
	{
		compressGamma(t_color.x),
		compressGamma(t_color.y),
		compressGamma(t_color.z)
	};
	return out;
}

Vector3 Raytracer::expandGamma(Vector3& t_color)
{
	Vector3 out =
	{
		expandGamma(t_color.x),
		expandGamma(t_color.y),
		expandGamma(t_color.z)
	};
	return out;
}

Vector3 Raytracer::mixLinear(Vector3& t_color1, Vector3& t_color2, float& alpha)
{
	Vector3 out;
	out.x = alpha * t_color1.x + (1.0f - alpha) * t_color2.x;
	out.y = alpha * t_color1.y + (1.0f - alpha) * t_color2.y;
	out.z = alpha * t_color1.z + (1.0f - alpha) * t_color2.z;

	return out;
}

Vector3 Raytracer::mixSRGB(Vector3& t_color1, Vector3& t_color2, float& alpha)
{
	Vector3 out1 = expandGamma(t_color1);
	Vector3 out2 = expandGamma(t_color2);
	Vector3 mixed = mixLinear(out1, out2, alpha);
	return compressGamma(mixed);
}
