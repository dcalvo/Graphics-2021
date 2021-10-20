#include <cmath>
#include <Util/exceptions.h>
#include "triangle.h"

using namespace Ray;
using namespace Util;

//////////////
// Triangle //
//////////////

void Triangle::init(const LocalSceneData& data) {
	// Set the vertex pointers
	for (int i = 0; i < 3; i++) {
		if (_vIndices[i] == -1)
			THROW("negative vertex index: %d", _vIndices[i]);
		else if (_vIndices[i] >= data.vertices.size())
			THROW("vertex index out of bounds: %d <= %d", _vIndices[i], static_cast<int>(data.vertices.size()));
		else _v[i] = &data.vertices[_vIndices[i]];
	}

	const Point3D p1 = _v[0]->position;
	const Point3D p2 = _v[1]->position;
	const Point3D p3 = _v[2]->position;

	// Construct the plane spanned by vertices
	const Plane3D plane(p1, p2, p3);
	_plane = plane;

	// Compute what we can for barycentric coordinates
	const Point3D v0 = _v[1]->position - _v[0]->position;
	const Point3D v1 = _v[2]->position - _v[0]->position;
	_d00 = v0.dot(v0);
	_d01 = v0.dot(v1);
	_d11 = v1.dot(v1);
	_denom = _d00 * _d11 - _d01 * _d01;
}

void Triangle::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	const Point3D p1 = _v[0]->position;
	const Point3D p2 = _v[1]->position;
	const Point3D p3 = _v[2]->position;
	const double min_x = std::min(p1[0], std::min(p2[0], p3[0]));
	const double min_y = std::min(p1[1], std::min(p2[1], p3[1]));
	const double min_z = std::min(p1[2], std::min(p2[2], p3[2]));
	const double max_x = std::max(p1[0], std::max(p2[0], p3[0]));
	const double max_y = std::max(p1[1], std::max(p2[1], p3[1]));
	const double max_z = std::max(p1[2], std::max(p2[2], p3[2]));
	const Point3D min_p(min_x, min_y, min_z), max_p(max_x + Epsilon, max_y + Epsilon, max_z + Epsilon);
	_bBox = ShapeBoundingBox(BoundingBox3D(min_p, max_p));
}

void Triangle::initOpenGL(void) {
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Triangle::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                           std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	if (abs(ray.direction.dot(_plane.normal)) < Epsilon) return Infinity;
	const double t = -(ray.position - _v[0]->position).dot(_plane.normal) / ray.direction.dot(_plane.normal);
	if (!range.isInside(t)) return Infinity;
	const Point3D p = ray(t); // point of intersection with the plane
	const auto [alpha, beta, gamma] = barycentricCoordinates(p);
	if (alpha < 0 || beta < 0 || gamma < 0) return Infinity;
	iInfo.position = p;
	iInfo.normal = (alpha * _v[0]->normal + beta * _v[1]->normal + gamma * _v[2]->normal).unit();
	// TODO: texture?
	return t;
}

std::tuple<double, double, double> Triangle::barycentricCoordinates(Point3D intersection) const {
	// Compute barycentric coordinates using Christer Ericson's Real-Time Collision Detection algorithm
	const Point3D p = intersection - _v[0]->position; // center to origin
	const double d20 = p.dot(_v[1]->position - _v[0]->position);
	const double d21 = p.dot(_v[2]->position - _v[0]->position);
	const double beta = (_d11 * d20 - _d01 * d21) / _denom;
	const double gamma = (_d00 * d21 - _d01 * d20) / _denom;
	const double alpha = 1. - beta - gamma;
	return {alpha, beta, gamma};
}

void Triangle::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
