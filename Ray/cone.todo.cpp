#include <cmath>
#include <Util/exceptions.h>
#include <Util/polynomial.h>
#include "scene.h"
#include "cone.h"

using namespace Ray;
using namespace Util;

//////////
// Cone //
//////////

void Cone::init(const LocalSceneData& data) {
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

void Cone::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	Point3D p(radius, height / 2, radius);
	_bBox = BoundingBox3D(center - p, center + p);
}

void Cone::initOpenGL(void) {
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	// generate 2 triangles each loop
	// 1 from the center to the edges at theta and phi
	// v1---v2 
	//  \   / 
	//   \ / 
	//   bot
	// 1 for the triangle enclosed by theta, phi, and the tip
	//    top
	//    / |
	//   /  |
	// v1--v2
	const int complexity = OpenGLTessellationComplexity;
	const auto bot = new Vertex();
	bot->position = center;
	bot->normal = Point3D(0., -1., 0.);
	for (int i = 0; i < 2 * complexity; i++) {
		const double sin_theta = sin(Pi / complexity * i);
		const double cos_theta = cos(Pi / complexity * i);
		const double sin_phi = sin(Pi / complexity * (i + 1));
		const double cos_phi = cos(Pi / complexity * (i + 1));
		const auto v1 = new Vertex();
		const auto v2 = new Vertex();
		v1->position = bot->position + Point3D(radius * cos_theta, 0, radius * sin_theta);
		v2->position = bot->position + Point3D(radius * cos_phi, 0, radius * sin_phi);
		v1->normal = v2->normal = Point3D(0., -1., 0.);
		mesh.emplace_back(v2, bot, v1);
		const auto v3 = new Vertex();
		const auto v4 = new Vertex();
		v3->position = v1->position;
		v4->position = v2->position;
		v3->normal = (v3->position - bot->position).unit();
		v3->normal = Point3D(v3->normal[0] * height / radius, radius / height, v3->normal[2] * height / radius).unit();
		v4->normal = (v4->position - bot->position).unit();
		v4->normal = Point3D(v4->normal[0] * height / radius, radius / height, v4->normal[2] * height / radius).unit();
		v3->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity), 0);
		v4->texCoordinate = Point2D(static_cast<double>(i + 1) / (2 * complexity), 0);
		// each triangle gets a new top so we can wrap a texture properly
		const auto top = new Vertex();
		top->position = center + Point3D(0., height, 0.);
		top->normal = Point3D(0., 1., 0.);
		top->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity), 1);
		mesh.emplace_back(v4, v3, top);
	}
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Cone::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                       std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

bool Cone::isInside(Point3D p) const {
	///////////////////////////////////////////////////
	// Determine if the point is inside the box here //
	///////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void Cone::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	_material->drawOpenGL(glslProgram);
	for (const auto& triangle : mesh) triangle.drawOpenGL(glslProgram);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
