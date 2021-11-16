#include <cmath>
#include <Util/exceptions.h>
#include "directionalLight.h"
#include "scene.h"
#include "Image/bmp.h"

using namespace Ray;
using namespace Util;

//////////////////////
// DirectionalLight //
//////////////////////

Point3D DirectionalLight::getAmbient(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the ambient contribution of the light here //
	////////////////////////////////////////////////////
	const Point3D intensity = _ambient;
	return intensity;
}

Point3D DirectionalLight::getDiffuse(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	////////////////////////////////////////////////////
	// Get the diffuse contribution of the light here //
	////////////////////////////////////////////////////
	Point3D diffuse;
	const Point3D intensity = _diffuse;
	const Point3D dirTowardsLight = -_direction;
	if (iInfo.normal.dot(dirTowardsLight) < 0) return diffuse;
	diffuse = iInfo.material->diffuse * iInfo.normal.dot(dirTowardsLight) * intensity;
	return diffuse;
}

Point3D DirectionalLight::getSpecular(Ray3D ray, const RayShapeIntersectionInfo& iInfo) const {
	/////////////////////////////////////////////////////
	// Get the specular contribution of the light here //
	/////////////////////////////////////////////////////
	Point3D specular;
	const Point3D intensity = _specular;
	const Point3D dirTowardsLight = -_direction;
	if (iInfo.normal.dot(dirTowardsLight) < 0) return specular;
	const Point3D r = (dirTowardsLight - 2 * (iInfo.normal.dot(dirTowardsLight) * iInfo.normal)).unit();
	const double vr = ray.direction.dot(r);
	if (vr < 0) return specular;
	specular = iInfo.material->specular * pow(vr, iInfo.material->specularFallOff) * intensity;
	return specular;
}

bool DirectionalLight::isInShadow(const RayShapeIntersectionInfo& iInfo, const Shape* shape) const {
	//////////////////////////////////////////////
	// Determine if the light is in shadow here //
	//////////////////////////////////////////////
	const Point3D dirTowardsLight = -_direction;
	const Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	RayShapeIntersectionInfo occlusionInfo;
	return !isinf(shape->intersect(ray, occlusionInfo));
}

Point3D DirectionalLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit,
                                       unsigned samples) const {
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	Point3D shadow(1., 1., 1.);
	const Point3D dirTowardsLight = -_direction;
	Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	RayShapeIntersectionInfo occlusionInfo;
	while (!isinf(shape.intersect(ray, occlusionInfo)) &&
		(shadow[0] > cLimit[0] && shadow[1] > cLimit[1] && shadow[2] > cLimit[2])) {
		shadow *= occlusionInfo.material->transparent;
		ray = Ray3D(occlusionInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
	}
	return shadow;
}

void DirectionalLight::drawOpenGL(int index, GLSLProgram* glslProgram) const {
#ifdef NEW_SHADER_CODE
	if (glslProgram) {
		static bool firstTime = true;
		glslProgram->setUniform<3>("light_ambient", &_ambient[0], firstTime);
		glslProgram->setUniform<3>("light_diffuse", &_diffuse[0], firstTime);
		glslProgram->setUniform<3>("light_specular", &_specular[0], firstTime);
		glslProgram->setUniform<3>("light_direction", &_direction[0], firstTime);
		firstTime = false;
	}
#endif // NEW_SHADER_CODE
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	const float ambient_param[4] = {_ambient[0], _ambient[1], _ambient[2], 1.};
	const float diffuse_param[4] = {_diffuse[0], _diffuse[1], _diffuse[2], 1.};
	const float specular_param[4] = {_specular[0], _specular[1], _specular[2], 1.};
	// a 0 as the fourth parameter indicates that this is a directional light source
	const float position_param[4] = {_direction[0], _direction[1], _direction[2], 0.};

	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, ambient_param);
	glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, diffuse_param);
	glLightfv(GL_LIGHT0 + index, GL_SPECULAR, specular_param);
	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, position_param);

	glEnable(GL_LIGHT0 + index);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
