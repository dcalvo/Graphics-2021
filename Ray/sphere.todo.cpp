#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphere.h"
#include "triangle.h"

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

	// Calculate intersection polynomial
	Polynomial3D<2> P;
	P.coefficient(2u, 0u, 0u) = P.coefficient(0u, 2u, 0u) = P.coefficient(0u, 0u, 2u) = 1;
	P.coefficient(0u, 0u, 0u) = -(radius * radius);
	_P = P;
}

void Sphere::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	Point3D p(radius, radius, radius);
	_bBox = BoundingBox3D(center - p, center + p);
}

void Sphere::initOpenGL(void) {
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	// adapted from http://www.songho.ca/opengl/gl_sphere.html
	const int stack_count = OpenGLTessellationComplexity;
	const int sector_count = 2 * stack_count;
	const double stack_step = Pi / stack_count;
	const double sector_step = 2 * Pi / sector_count;
	const double inverse_radius = 1. / radius;
	// generate vertices
	std::vector<Vertex*> vertices;
	for (int i = 0; i <= stack_count; i++) {
		const double stack_angle = Pi / 2 - i * stack_step;
		const double xy = radius * cos(stack_angle);
		const double z = sin(stack_angle);

		// generate a vertical band of vertices around the sphere
		for (int j = 0; j <= sector_count; j++) {
			const double sector_angle = j * sector_step;
			// find vertex position by implicit formula
			const double x = xy * cos(sector_angle);
			const double y = xy * sin(sector_angle);
			// normalize vertex normals
			const double n_x = x * inverse_radius;
			const double n_y = y * inverse_radius;
			const double n_z = z * inverse_radius;
			// map texture coordinate between [0, 1]
			const double u = static_cast<double>(j) / sector_count;
			const double v = static_cast<double>(i) / sector_count;
			// add vertex
			const Point3D position(x, y, z);
			const Point3D normal(n_x, n_y, n_z);
			const Point2D texCoordinate(u, v);
			auto vertex = new Vertex();
			vertex->position = position;
			vertex->normal = normal;
			vertex->texCoordinate = texCoordinate;
			vertices.push_back(vertex);
		}
	}
	// generate CCW index list of triangles for each quad we formed with stacks and sectors
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1
	// we visit indices k1 -> k2 -> k1+1
	// then indices k1+1 -> k2 -> k2+1
	for (int i = 0; i < stack_count; i++) {
		int k1 = i * (sector_count + 1);
		int k2 = k1 + sector_count + 1;
		for (int j = 0; j < sector_count; j++, k1++, k2++) {
			// visit k1 -> k2 -> k1+1
			if (i) mesh.emplace_back(vertices[k1], vertices[k2], vertices[k1 + 1]);
			// visit k1+1 -> k2 -> k2+1
			if (i != (stack_count - 1)) mesh.emplace_back(vertices[k1 + 1], vertices[k2], vertices[k2 + 1]);
		}
	}
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

double Sphere::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                         std::function<bool (double)> validityLambda) const {
	RayTracingStats::IncrementRayPrimitiveIntersectionNum();

	/////////////////////////////////////////////////////////
	// Compute the intersection of the sphere with the ray //
	/////////////////////////////////////////////////////////
	const Polynomial1D<2> p = _P(ray);
	double roots[2];
	const unsigned int root_num = p.roots(roots);
	if (!root_num) return Infinity;
	// get smallest positive root
	double root;
	if (root_num == 2) {
		if (roots[0] > 0 && roots[1] > 0) root = std::min(roots[0], roots[1]);
		else root = roots[(roots[0] > 0 ? 0 : 1)];
	}
	else root = roots[0];
	if (!range.isInside(root)) return Infinity;
	iInfo.position = ray(root);
	iInfo.normal = (iInfo.position - center).unit();
	iInfo.material = _material;
	// TODO: set iInfo texture
	return root;
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
	_material->drawOpenGL(glslProgram);
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	GLUquadric* q = gluNewQuadric();
	gluSphere(q, radius, 2 * OpenGLTessellationComplexity, OpenGLTessellationComplexity);
	gluDeleteQuadric(q);
	glPopMatrix();

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
