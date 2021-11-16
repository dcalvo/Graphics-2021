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
	// generate CCW index list of triangles for each face
	// v1--v3
	// |  / |
	// | /  |
	// v2--v4
	// top face
	const auto v1 = new Vertex();
	const auto v2 = new Vertex();
	const auto v3 = new Vertex();
	const auto v4 = new Vertex();
	v1->position = center + (Point3D(-1., 1., -1.) * length);
	v2->position = center + (Point3D(-1., 1., 1.) * length);
	v3->position = center + (Point3D(1., 1., -1.) * length);
	v4->position = center + (Point3D(1., 1., 1.) * length);
	v1->normal = v2->normal = v3->normal = v4->normal = Point3D(0., 1., 0.);
	v1->texCoordinate = Point2D(0, 0);
	v2->texCoordinate = Point2D(0, 1);
	v3->texCoordinate = Point2D(1, 0);
	v4->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v1, v2, v3);
	mesh.emplace_back(v3, v2, v4);
	// bottom face
	const auto v5 = new Vertex();
	const auto v6 = new Vertex();
	const auto v7 = new Vertex();
	const auto v8 = new Vertex();
	v5->position = center + (Point3D(-1., -1., -1.) * length);
	v6->position = center + (Point3D(-1., -1., 1.) * length);
	v7->position = center + (Point3D(1., -1., -1.) * length);
	v8->position = center + (Point3D(1., -1., 1.) * length);
	v5->normal = v6->normal = v7->normal = v8->normal = Point3D(0., -1., 0.);
	v5->texCoordinate = Point2D(0, 0);
	v6->texCoordinate = Point2D(0, 1);
	v7->texCoordinate = Point2D(1, 0);
	v8->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v5, v7, v6); // invert order on negative faces
	mesh.emplace_back(v7, v8, v6); // invert order on negative faces
	// front face
	const auto v9 = new Vertex();
	const auto v10 = new Vertex();
	const auto v11 = new Vertex();
	const auto v12 = new Vertex();
	v9->position = center + (Point3D(-1., 1., 1.) * length);
	v10->position = center + (Point3D(-1., -1., 1.) * length);
	v11->position = center + (Point3D(1., 1., 1.) * length);
	v12->position = center + (Point3D(1., -1., 1.) * length);
	v9->normal = v10->normal = v11->normal = v12->normal = Point3D(0., 0., 1.);
	v9->texCoordinate = Point2D(0, 0);
	v10->texCoordinate = Point2D(0, 1);
	v11->texCoordinate = Point2D(1, 0);
	v12->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v9, v10, v11);
	mesh.emplace_back(v11, v10, v12);
	// back face
	const auto v13 = new Vertex();
	const auto v14 = new Vertex();
	const auto v15 = new Vertex();
	const auto v16 = new Vertex();
	v13->position = center + (Point3D(-1., 1., -1.) * length);
	v14->position = center + (Point3D(-1., -1., -1.) * length);
	v15->position = center + (Point3D(1., 1., -1.) * length);
	v16->position = center + (Point3D(1., -1., -1.) * length);
	v13->normal = v14->normal = v15->normal = v16->normal = Point3D(0., 0., -1.);
	v13->texCoordinate = Point2D(0, 0);
	v14->texCoordinate = Point2D(0, 1);
	v15->texCoordinate = Point2D(1, 0);
	v16->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v13, v15, v14); // invert order on negative faces
	mesh.emplace_back(v15, v16, v14); // invert order on negative faces
	// right face
	const auto v17 = new Vertex();
	const auto v18 = new Vertex();
	const auto v19 = new Vertex();
	const auto v20 = new Vertex();
	v17->position = center + (Point3D(1., 1., -1.) * length);
	v18->position = center + (Point3D(1., -1., -1.) * length);
	v19->position = center + (Point3D(1., 1., 1.) * length);
	v20->position = center + (Point3D(1., -1., 1.) * length);
	v17->normal = v18->normal = v19->normal = v20->normal = Point3D(1., 0., 0.);
	v17->texCoordinate = Point2D(0, 0);
	v18->texCoordinate = Point2D(0, 1);
	v19->texCoordinate = Point2D(1, 0);
	v20->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v17, v19, v18); // invert order on negative faces (x is flipped)
	mesh.emplace_back(v19, v20, v18); // invert order on negative faces (x is flipped)
	// left face
	const auto v21 = new Vertex();
	const auto v22 = new Vertex();
	const auto v23 = new Vertex();
	const auto v24 = new Vertex();
	v21->position = center + (Point3D(-1., 1., -1.) * length);
	v22->position = center + (Point3D(-1., -1., -1.) * length);
	v23->position = center + (Point3D(-1., 1., 1.) * length);
	v24->position = center + (Point3D(-1., -1., 1.) * length);
	v21->normal = v22->normal = v23->normal = v24->normal = Point3D(-1., 0., 0.);
	v21->texCoordinate = Point2D(0, 0);
	v22->texCoordinate = Point2D(0, 1);
	v23->texCoordinate = Point2D(1, 0);
	v24->texCoordinate = Point2D(1, 1);
	mesh.emplace_back(v21, v22, v23);
	mesh.emplace_back(v23, v22, v24);
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
	_material->drawOpenGL(glslProgram);
	for (const auto& mesh_triangle : mesh) {
		const auto v0 = mesh_triangle[0];
		const auto v1 = mesh_triangle[1];
		const auto v2 = mesh_triangle[2];

		glBegin(GL_TRIANGLES);

		glTexCoord3d(v0->texCoordinate[0], v0->texCoordinate[1], v0->texCoordinate[2]);
		glNormal3d(v0->normal[0], v0->normal[1], v0->normal[2]);
		glVertex3d(v0->position[0], v0->position[1], v0->position[2]);

		glTexCoord3d(v1->texCoordinate[0], v1->texCoordinate[1], v1->texCoordinate[2]);
		glNormal3d(v1->normal[0], v1->normal[1], v1->normal[2]);
		glVertex3d(v1->position[0], v1->position[1], v1->position[2]);

		glTexCoord3d(v2->texCoordinate[0], v2->texCoordinate[1], v2->texCoordinate[2]);
		glNormal3d(v2->normal[0], v2->normal[1], v2->normal[2]);
		glVertex3d(v2->position[0], v2->position[1], v2->position[2]);

		glEnd();
	}
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
