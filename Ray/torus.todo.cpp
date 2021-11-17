#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "torus.h"

using namespace Ray;
using namespace Util;

///////////
// Torus //
///////////

void Torus::init(const LocalSceneData& data) {
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

void Torus::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	Point3D p(iRadius + oRadius, oRadius, iRadius + oRadius);
	_bBox = BoundingBox3D(center - p, center + p);
}

void Torus::initOpenGL(void) {
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	// implemented from https://gamedev.stackexchange.com/questions/16845/how-do-i-generate-a-torus-mesh
	// the outer loop iterates through the major radius of the torus and finds vertical slices to triangulate
	// the inner loop then subdivides the vertical slice into horizontal sectors, which results in a quad
	// v1--v3
	// |  / |
	// | /  |
	// v2--v4
	const int complexity = OpenGLTessellationComplexity;
	for (int i = 0; i < 2 * complexity; i++) {
		for (int j = 0; j < 2 * complexity; j++) {
			const double sin_theta = sin(Pi / complexity * i);
			const double cos_theta = cos(Pi / complexity * i);
			const double sin_theta_step = sin(Pi / complexity * (i + 1));
			const double cos_theta_step = cos(Pi / complexity * (i + 1));
			const double sin_phi = sin(Pi / complexity * j);
			const double cos_phi = cos(Pi / complexity * j);
			const double sin_phi_step = sin(Pi / complexity * (j + 1));
			const double cos_phi_step = cos(Pi / complexity * (j + 1));
			const double d = (oRadius + iRadius * sin_phi);
			const double d_step = (oRadius + iRadius * sin_phi_step);
			const auto v1 = new Vertex();
			const auto v2 = new Vertex();
			const auto v3 = new Vertex();
			const auto v4 = new Vertex();
			v1->position = center + Point3D(d * cos_theta, iRadius * cos_phi, d * sin_theta);
			v2->position = center + Point3D(d * cos_theta_step, iRadius * cos_phi, d * sin_theta_step);
			v3->position = center + Point3D(d_step * cos_theta, iRadius * cos_phi_step, d_step * sin_theta);
			v4->position = center + Point3D(d_step * cos_theta_step, iRadius * cos_phi_step, d_step * sin_theta_step);
			v1->normal = Point3D(sin_phi * cos_theta, cos_phi, sin_phi * sin_theta);
			v2->normal = Point3D(sin_phi * cos_theta_step, cos_phi, sin_phi * sin_theta_step);
			v3->normal = Point3D(cos_theta * sin_phi_step, cos_phi_step, sin_theta * sin_phi_step);
			v4->normal = Point3D(cos_theta_step * sin_phi_step, cos_phi_step, sin_theta_step * sin_phi_step);
			v1->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity),
			                            static_cast<double>(j) / (2 * complexity));
			v2->texCoordinate = Point2D(static_cast<double>(i + 1) / (2 * complexity),
			                            static_cast<double>(j) / (2 * complexity));
			v3->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity),
			                            static_cast<double>(j + 1) / (2 * complexity));
			v4->texCoordinate = Point2D(static_cast<double>(i + 1) / (2 * complexity),
			                            static_cast<double>(j + 1) / (2 * complexity));
			mesh.emplace_back(v1, v2, v3);
			mesh.emplace_back(v2, v4, v3);
		}
	}
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Torus::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                        std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

bool Torus::isInside(Point3D p) const {
	////////////////////////////////////////////////////////
	// Determine if the point is inside the cylinder here //
	////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void Torus::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	_material->drawOpenGL(glslProgram);
	for (const auto& triangle : mesh) triangle.drawOpenGL(glslProgram);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
