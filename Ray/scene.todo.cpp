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
	THROW("method undefined");
	return Point3D();
}

bool Scene::Refract(Point3D v, Point3D n, double ir, Point3D& refract) {
	//////////////////
	// Refract here //
	//////////////////
	THROW("method undefined");
	return false;
}

Point3D Scene::getColor(Ray3D ray, int rDepth, Point3D cLimit) {
	////////////////////////////////////////////////
	// Get the color associated with the ray here //
	////////////////////////////////////////////////
	RayShapeIntersectionInfo iInfo;
	const double d = this->intersect(ray, iInfo);
	if (isinf(d)) return Point3D(0, 0, 0);
	// compute color
	const auto lights = _globalData.lights;
	const Point3D I_e = iInfo.material->emissive;
	const Point3D K_a = iInfo.material->ambient;
	Point3D I_la;
	for (const auto light : lights) {
		I_la = I_la + light->getAmbient(ray, iInfo);
	}
	Point3D summed_lights;
	for (const auto light : lights) {
		const Point3D ambient = K_a * I_la;
		const Point3D diffuse = light->getDiffuse(ray, iInfo);
		const Point3D specular = light->getSpecular(ray, iInfo);
		const Point3D shadow = light->isInShadow(iInfo, this) ? Point3D(0, 0, 0) : Point3D(1, 1, 1);
		summed_lights = summed_lights + ambient + (diffuse + specular) * shadow;
	}
	Point3D I = I_e + summed_lights;
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
