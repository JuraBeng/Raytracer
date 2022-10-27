#include "stdafx.h"
#include "Light.h"

Light::Light(Vector3 t_position, Vector3 t_color, Vector3 t_intensity)
{
	m_position = t_position;
	m_color = t_color;
	m_intensity = t_intensity * m_color;
}

Light::Light()
{
}

Light::~Light()
{
}

void Light::setLightColor(Vector3 t_color)
{
	m_color = t_color;
}

void Light::setLightPosition(Vector3 t_position)
{
	m_position = t_position;
}

void Light::setLightIntensity(Vector3 t_intensity)
{
	m_intensity = t_intensity;
}

Vector3 Light::getLightPosition()
{
	return m_position;
}

Vector3 Light::getLightIntensity()
{
	return m_intensity * m_color;
}
