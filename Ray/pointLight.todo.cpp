#include <cmath>
#include <Util/exceptions.h>
#include "pointLight.h"
#include "scene.h"
#include "Image/bmp.h"

using namespace Ray;
using namespace Util;

////////////////
// PointLight //
////////////////

Point3D PointLight::getAmbient(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	return getIntensity(_ambient, iInfo);
}

Point3D PointLight::getDiffuse(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
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

Point3D PointLight::getSpecular(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
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

Point3D PointLight::getIntensity(Point3D light, const RayShapeIntersectionInfo& iInfo) const {
	const double delta = (_location - iInfo.position).length();
	const double denom = _constAtten + _linearAtten * delta + _quadAtten * (delta * delta);
	return light / denom;
}

bool PointLight::isInShadow(const RayShapeIntersectionInfo& iInfo, const Shape* shape) const {
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	RayShapeIntersectionInfo occlusionInfo;
	const BoundingBox1D range(Point1D(Epsilon), Point1D((_location - iInfo.position).length()));
	return !isinf(shape->intersect(ray, occlusionInfo, range));
}

Point3D PointLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit,
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

void PointLight::drawOpenGL(int index, GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
