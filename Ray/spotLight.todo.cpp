#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "spotLight.h"

using namespace Ray;
using namespace Util;

///////////////
// SpotLight //
///////////////
Point3D SpotLight::getAmbient(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	return getIntensity(_ambient, iInfo);
}

Point3D SpotLight::getDiffuse(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	Point3D diffuse;
	const Point3D intensity = getIntensity(_diffuse, iInfo);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	if (iInfo.normal.dot(dirTowardsLight) < 0) return diffuse;
	diffuse = iInfo.material->diffuse * iInfo.normal.dot(dirTowardsLight) * intensity;
	return diffuse;
}

Point3D SpotLight::getSpecular(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	Point3D specular;
	const Point3D intensity = getIntensity(_specular, iInfo);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	if (iInfo.normal.dot(dirTowardsLight) < 0) return specular;
	const Point3D r = (dirTowardsLight - 2 * (iInfo.normal.dot(dirTowardsLight) * iInfo.normal)).unit();
	const double vr = ray.direction.dot(r);
	if (vr < 0) return specular;
	specular = iInfo.material->specular * pow(vr, iInfo.material->specularFallOff) * intensity;
	return specular;
}

Point3D SpotLight::getIntensity(Point3D light, const RayShapeIntersectionInfo& iInfo) const {
	const Point3D dirTowardsShape = (iInfo.position - _location).unit();
	const double dv = _direction.dot(dirTowardsShape);
	const Point3D numer = light * pow(dv, _dropOffRate);
	const double delta = (_location - iInfo.position).length();
	const double denom = _constAtten + _linearAtten * delta + _quadAtten * (delta * delta);
	return dv > cos(_cutOffAngle * 2) ? numer / denom : Point3D(0., 0., 0.);
}

bool SpotLight::isInShadow(const RayShapeIntersectionInfo& iInfo, const Shape* shape) const {
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	RayShapeIntersectionInfo occlusionInfo;
	const BoundingBox1D range(Point1D(Epsilon), Point1D((_location - iInfo.position).length()));
	return !isinf(shape->intersect(ray, occlusionInfo, range));
}

Point3D SpotLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit,
                                unsigned int samples) const {
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	Point3D shadow(1., 1., 1.);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	RayShapeIntersectionInfo occlusionInfo;
	const BoundingBox1D range(Point1D(Epsilon), Point1D((_location - iInfo.position).length()));
	while (!isinf(shape.intersect(ray, occlusionInfo, range)) &&
		(shadow[0] > cLimit[0] && shadow[1] > cLimit[1] && shadow[2] > cLimit[2])) {
		shadow *= occlusionInfo.material->transparent;
		ray = Ray3D(occlusionInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	}
	return shadow;
}

void SpotLight::drawOpenGL(int index, GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	const float ambient_param[4] = {_ambient[0], _ambient[1], _ambient[2], 1.};
	const float diffuse_param[4] = {_diffuse[0], _diffuse[1], _diffuse[2], 1.};
	const float specular_param[4] = {_specular[0], _specular[1], _specular[2], 1.};
	const float position_param[4] = {_location[0], _location[1], _location[2], 1.};

	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, ambient_param);
	glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, diffuse_param);
	glLightfv(GL_LIGHT0 + index, GL_SPECULAR, specular_param);
	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, position_param);
	glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, _constAtten);
	glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, _linearAtten);
	glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, _quadAtten);
	glLightf(GL_LIGHT0 + index, GL_SPOT_CUTOFF, _cutOffAngle / (Pi / 2) * 90);
	glLightf(GL_LIGHT0 + index, GL_SPOT_EXPONENT, _dropOffRate);

	glEnable(GL_LIGHT0 + index);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
