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
	const Point3D intensity = getIntensity(_diffuse, iInfo);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Point3D diffuse = iInfo.material->diffuse * iInfo.normal.dot(dirTowardsLight) * intensity;
	return diffuse;
}

Point3D SpotLight::getSpecular(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	const Point3D intensity = getIntensity(_specular, iInfo);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Point3D r = (dirTowardsLight - 2 * (iInfo.normal.dot(dirTowardsLight) * iInfo.normal)).unit();
	const double vr = std::clamp(ray.direction.dot(r), 0., 1.);
	const Point3D specular = iInfo.material->specular * pow(vr, iInfo.material->specularFallOff) * intensity;
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

Point3D SpotLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit) const {
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	THROW("method undefined");
	return Point3D(1., 1., 1.);
}

void SpotLight::drawOpenGL(int index, GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
