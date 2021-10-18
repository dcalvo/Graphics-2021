#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////
// Scene //
///////////
Point3D Scene::Reflect(Point3D v, Point3D n) {
	//////////////////
	// Reflect here //
	//////////////////
	return (v - 2 * v.dot(n) * n).unit();
}

bool Scene::Refract(Point3D v, Point3D n, double ir, Point3D& refract) {
	//////////////////
	// Refract here //
	//////////////////
	// Snell's works from the intersection, so flip v direction
	const double t_i = -v.dot(n) / v.length() * n.length();
	const double t_r = t_i;
	if (abs(1 * sin(t_i)) > 1) return false;
	refract = (1 * cos(t_i) - cos(t_r)) * n - 1 * -v;
	return true;
}

Point3D Scene::getColor(Ray3D ray, int rDepth, Point3D cLimit) {
	////////////////////////////////////////////////
	// Get the color associated with the ray here //
	////////////////////////////////////////////////
	Point3D I;
	if (!rDepth || (cLimit[0] > 1 && cLimit[1] > 1 && cLimit[2] > 1)) return I;
	RayShapeIntersectionInfo iInfo;
	const double d = this->intersect(ray, iInfo);
	if (isinf(d)) return I;

	// compute color
	const auto lights = _globalData.lights;
	Point3D ambient_sum;
	Point3D light_contrib;
	for (const auto light : lights) {
		ambient_sum = ambient_sum + light->getAmbient(ray, iInfo);
	}
	for (const auto light : lights) {
		const Point3D ambient = iInfo.material->ambient * ambient_sum;
		const Point3D diffuse = light->getDiffuse(ray, iInfo);
		const Point3D specular = light->getSpecular(ray, iInfo);
		const Point3D shadow = light->isInShadow(iInfo, this) ? Point3D(0., 0., 0.) : Point3D(1., 1., 1.);
		light_contrib = light_contrib + ambient + (diffuse + specular) * shadow;
	}

	Point3D reflect_contrib;
	if (ray.direction.dot(iInfo.normal) < 0) {
		Ray3D reflect;
		reflect.direction = Reflect(ray.direction, iInfo.normal);
		reflect.position = iInfo.position + reflect.direction * Epsilon;
		const Point3D specularity = iInfo.material->specular;
		reflect_contrib = getColor(reflect, rDepth - 1, cLimit / specularity) * specularity;
	}

	Point3D refract_contrib;
	Ray3D refract;
	if (Refract(ray.direction, iInfo.normal, iInfo.material->ir, refract.direction)) {
		refract.position = iInfo.position + refract.direction * Epsilon;
		const Point3D transparency = iInfo.material->transparent;
		refract_contrib = getColor(refract, rDepth - 1, cLimit / transparency) * transparency;
	}

	I = iInfo.material->emissive + light_contrib + reflect_contrib + refract_contrib;
	I[0] = std::clamp(I[0], 0., 1.);
	I[1] = std::clamp(I[1], 0., 1.);
	I[2] = std::clamp(I[2], 0., 1.);
	return I;
}

//////////////
// Material //
//////////////
void Material::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

/////////////
// Texture //
/////////////
void Texture::initOpenGL(void) {
	///////////////////////////////////
	// Do OpenGL texture set-up here //
	///////////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
