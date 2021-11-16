#include <cmath>
#include  <Util/exceptions.h>
#include "scene.h"
#include "box.h"

using namespace Ray;
using namespace Util;

/////////
// Box //
/////////

void Box::init(const LocalSceneData& data) {
	// Set the material pointer
	if (_materialIndex < 0)
		THROW("negative material index: %d", _materialIndex);
	else if (_materialIndex >= data.materials.size())
		THROW("material index out of bounds: %d <= %d", _materialIndex, static_cast<int>(data.materials.size()));
	else _material = &data.materials[_materialIndex];

	//////////////////////////////////
	// Do any necessary set-up here //
	//////////////////////////////////
	WARN_ONCE("method undefined");
}

void Box::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	_bBox = BoundingBox3D(center - (length / 2), center + (length / 2));
}

void Box::initOpenGL(void) {
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	WARN_ONCE("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Box::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                      std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

bool Box::isInside(Point3D p) const {
	///////////////////////////////////////////////////
	// Determine if the point is inside the box here //
	///////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void Box::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
