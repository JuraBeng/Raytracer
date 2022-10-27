#ifndef LIGHT_H_
#define LIGHT_H_
#include "vector3.h"
class Light
{
public:
	Light(Vector3 t_position, Vector3 t_color, Vector3 t_intensity);
	Light();
	~Light();
	void setLightColor(Vector3 t_color);
	void setLightPosition(Vector3 t_position);
	void setLightIntensity(Vector3 t_intensity);
	Vector3 getLightColor();
	Vector3 getLightPosition();
	Vector3 getLightIntensity();
private:
	Vector3 m_color;
	Vector3 m_position;
	Vector3 m_intensity;
};

#endif
