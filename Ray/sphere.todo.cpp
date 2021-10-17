#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphere.h"

using namespace Ray;
using namespace Util;

////////////
// Sphere //
////////////

void Sphere::init(const LocalSceneData& data) {
	// Set the material pointer
	if (_materialIndex < 0)
		THROW("negative material index: %d", _materialIndex);
	else if (_materialIndex >= data.materials.size())
		THROW("material index out of bounds: %d <= %d", _materialIndex, static_cast<int>(data.materials.size()));
	else _material = &data.materials[_materialIndex];

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE("method undefined");
}

void Sphere::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW("method undefined");
}

void Sphere::initOpenGL(void) {
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Sphere::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                         std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	//////////////////////////////////////////////////////////////
	// Compute the intersection of the sphere with the ray here //
	//////////////////////////////////////////////////////////////
	Polynomial3D<2> P;
	P.coefficient(2u, 0u, 0u) = P.coefficient(0u, 2u, 0u) = P.coefficient(0u, 0u, 2u) = 1;
	P.coefficient(0u, 0u, 0u) = -(this->radius * this->radius);
	Polynomial1D<2> p = P(ray);
	double roots[2];
	unsigned int rootNum = p.roots(roots);
	if (rootNum) return rootNum == 1 ? roots[0] : roots[0] < roots[1] ? roots[0] : roots[1];
	return Infinity;
}

bool Sphere::isInside(Point3D p) const {
	//////////////////////////////////////////////////////
	// Determine if the point is inside the sphere here //
	//////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void Sphere::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
