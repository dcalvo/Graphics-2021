#include <cmath>
#include <Util/exceptions.h>
#include "pointLight.h"
#include "scene.h"

using namespace Ray;
using namespace Util;

////////////////
// PointLight //
////////////////

Point3D PointLight::getAmbient(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	return getIntensity(_ambient, iInfo.position);
}

Point3D PointLight::getDiffuse(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	const Point3D intensity = getIntensity(_diffuse, iInfo.position);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Point3D diffuse = iInfo.material->diffuse * iInfo.normal.dot(dirTowardsLight) * intensity;
	return diffuse;
}

Point3D PointLight::getSpecular(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	const Point3D intensity = getIntensity(_specular, iInfo.position);
	const Point3D dirTowardsLight = (_location - iInfo.position).unit();
	const Point3D r = (dirTowardsLight - 2 * (iInfo.normal.dot(dirTowardsLight) * iInfo.normal)).unit();
	const double vr = std::clamp(ray.direction.dot(r), 0., 1.);
	const Point3D specular = iInfo.material->specular * pow(vr, iInfo.material->specularFallOff) * intensity;
	return specular;
}

Point3D PointLight::getIntensity(Point3D light, Point3D intersection) const {
	const double delta = (_location - intersection).length();
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
	return !isinf(shape->intersect(ray, occlusionInfo));
}

Point3D PointLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit) const {
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	THROW("method undefined");
	return Point3D(1., 1., 1.);
}

void PointLight::drawOpenGL(int index, GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
