#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "cylinder.h"

using namespace Ray;
using namespace Util;

//////////////
// Cylinder //
//////////////

void Cylinder::init(const LocalSceneData& data) {
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

void Cylinder::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	Point3D p(radius, height / 2, radius);
	_bBox = BoundingBox3D(center - p, center + p);
}

void Cylinder::initOpenGL(void) {
	/////////////////////////////////////////
	// Do any necessary OpenGL set-up here //
	/////////////////////////////////////////
	// generate 4 triangles each loop
	// two from the center poles to the edges at theta and phi
	// v1---v3       v2---v4
	//  \   /         \   /
	//   \ /    and    \ /
	//   top           bot
	// two for the quad enclosed by theta and phi
	// v1--v3
	// |  / |
	// | /  |
	// v2--v4
	const int complexity = OpenGLTessellationComplexity;
	const auto top = new Vertex();
	const auto bot = new Vertex();
	top->position = center + Point3D(0., height, 0.);
	top->normal = Point3D(0., 1., 0.);
	bot->position = center;
	bot->normal = Point3D(0., -1., 0.);
	for (int i = 0; i < 2 * complexity; i++) {
		const double sin_theta = sin(Pi / complexity * i);
		const double cos_theta = cos(Pi / complexity * i);
		const double sin_phi = sin(Pi / complexity * (i + 1));
		const double cos_phi = cos(Pi / complexity * (i + 1));
		const auto v1 = new Vertex();
		const auto v2 = new Vertex();
		const auto v3 = new Vertex();
		const auto v4 = new Vertex();
		v1->position = top->position + Point3D(radius * cos_theta, 0, radius * sin_theta);
		v2->position = bot->position + Point3D(radius * cos_theta, 0, radius * sin_theta);
		v3->position = top->position + Point3D(radius * cos_phi, 0, radius * sin_phi);
		v4->position = bot->position + Point3D(radius * cos_phi, 0, radius * sin_phi);
		v1->normal = v3->normal = Point3D(0., 1., 0.);
		v2->normal = v4->normal = Point3D(0., -1., 0.);
		mesh.emplace_back(v1, top, v3);
		mesh.emplace_back(v4, bot, v2);
		const auto v5 = new Vertex();
		const auto v6 = new Vertex();
		const auto v7 = new Vertex();
		const auto v8 = new Vertex();
		v5->position = v1->position;
		v6->position = v2->position;
		v7->position = v3->position;
		v8->position = v4->position;
		v5->normal = (v5->position - top->position).unit();
		v6->normal = (v6->position - bot->position).unit();
		v7->normal = (v7->position - top->position).unit();
		v8->normal = (v8->position - bot->position).unit();
		v5->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity), 0);
		v6->texCoordinate = Point2D(static_cast<double>(i) / (2 * complexity), 1);
		v7->texCoordinate = Point2D(static_cast<double>(i + 1) / (2 * complexity), 0);
		v8->texCoordinate = Point2D(static_cast<double>(i + 1) / (2 * complexity), 1);
		mesh.emplace_back(v5, v7, v6);
		mesh.emplace_back(v6, v7, v8);
	}
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Cylinder::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                           std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////////
	// Compute the intersection of the shape with the ray here //
	/////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

bool Cylinder::isInside(Point3D p) const {
	////////////////////////////////////////////////////////
	// Determine if the point is inside the cylinder here //
	////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void Cylinder::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	_material->drawOpenGL(glslProgram);

	GLUquadric* q = gluNewQuadric();

	glPushMatrix();

	glTranslatef(center[0], center[1] - height / 2, center[2]);
	glRotatef(90, -1, 0, 0);
	gluCylinder(q, radius, radius, height, OpenGLTessellationComplexity, OpenGLTessellationComplexity);

	glPushMatrix();
	glTranslatef(0, 0, height);
	gluDisk(q, 0, radius, OpenGLTessellationComplexity, OpenGLTessellationComplexity);
	glPopMatrix();

	glRotatef(180, 1, 0, 0); // Normals pointing out
	gluDisk(q, 0, radius, OpenGLTessellationComplexity, OpenGLTessellationComplexity);

	glPopMatrix();

	gluDeleteQuadric(q);

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
