#include <Util/exceptions.h>
#include "shapeList.h"
#include "triangle.h"

using namespace Ray;
using namespace Util;

////////////////
// Difference //
////////////////
void Difference::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW("method undefined");
}

double Difference::intersect(Ray3D ray, class RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                             std::function<bool (double)> validityLambda) const {
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the ray here //
	//////////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

bool Difference::isInside(Point3D p) const {
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the difference here //
	//////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

///////////////
// ShapeList //
///////////////
double ShapeList::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                            std::function<bool (double)> validityLambda) const {
	//////////////////////////////////////////////////////////////////
	// Compute the intersection of the shape list with the ray here //
	//////////////////////////////////////////////////////////////////
	double closest_d = Infinity;
	for (const auto shape : shapes) {
		RayShapeIntersectionInfo thisInfo;
		const double d = shape->intersect(ray, thisInfo, range, validityLambda);
		if (isinf(d) || d < 0 || !range.isInside(d)) continue;
		if (d < closest_d) {
			closest_d = d;
			iInfo = thisInfo;
		}
	}
	return closest_d;
}

bool ShapeList::isInside(Point3D p) const {
	//////////////////////////////////////////////////////////
	// Determine if the point is inside the shape list here //
	//////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

void ShapeList::init(const LocalSceneData& data) {
	// Initialize the children
	for (int i = 0; i < shapes.size(); i++) shapes[i]->init(data);

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE("method undefined");
}

void ShapeList::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	WARN_ONCE("method undefined");
}

void ShapeList::initOpenGL(void) {
	// Initialize the children
	for (int i = 0; i < shapes.size(); i++) shapes[i]->initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE("remainder of method undefined");
}

void ShapeList::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

/////////////////
// AffineShape //
/////////////////
double AffineShape::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                              std::function<bool (double)> validityLambda) const {
	//////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the affinely deformed shape here //
	//////////////////////////////////////////////////////////////////////////////////////
	const Matrix4D globalToLocal = getInverseMatrix();
	const Matrix3D globalToLocalLinear(globalToLocal);
	const Matrix4D localToGlobal = getMatrix();
	const Matrix3D localToGlobalNormal = getNormalMatrix();
	// transform ray G2L
	Ray3D local_ray;
	local_ray.position = globalToLocal * ray.position;
	local_ray.direction = (globalToLocalLinear * ray.direction).unit();
	// intersect in L space
	const double local_d = _shape->intersect(local_ray, iInfo, range, validityLambda);
	if (isinf(local_d)) return Infinity;
	// transform hit info L2G
	iInfo.position = localToGlobal * iInfo.position;
	iInfo.normal = localToGlobalNormal * iInfo.normal;
	const double d = (iInfo.position - ray.position).length() / ray.direction.length();
	return d;
}

bool AffineShape::isInside(Point3D p) const {
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the affinely deformed shape here //
	///////////////////////////////////////////////////////////////////////
	THROW("method undefined");
	return _shape->isInside(p);
}

void AffineShape::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW("method undefined");
	_shape->updateBoundingBox();
}

void AffineShape::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	WARN_ONCE("method undefined");
	_shape->drawOpenGL(glslProgram);

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

//////////////////
// TriangleList //
//////////////////
double TriangleList::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                               std::function<bool (double)> validityLambda) const {
	////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the triangle list here //
	////////////////////////////////////////////////////////////////////////////
	double d = Infinity;
	d = _shapeList.intersect(ray, iInfo, range, validityLambda);
	if (isinf(d)) return Infinity;
	iInfo.material = _material;
	return d;
}

void TriangleList::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

void TriangleList::init(const LocalSceneData& data) {
	// Set the vertex and material pointers
	_vertices = &data.vertices[0];
	_vNum = static_cast<unsigned>(data.vertices.size());
	if (_materialIndex >= data.materials.size())
		THROW("shape specifies a material that is out of bounds: %d <= %d", _materialIndex,
	      static_cast<int>(data.materials.size()));
	else if (_materialIndex < 0)
		THROW("negative material index: %d", _materialIndex);
	else _material = &data.materials[_materialIndex];

	_shapeList.init(data);

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////

	WARN_ONCE("method undefined");
}

void TriangleList::initOpenGL(void) {
	_shapeList.initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
	WARN_ONCE("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

///////////
// Union //
///////////
double Union::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                        std::function<bool (double)> validityLambda) const {
	/////////////////////////////////////////////////////////////
	// Compute the intersection of the union with the ray here //
	/////////////////////////////////////////////////////////////
	THROW("method undefined");
	return Infinity;
}

void Union::init(const LocalSceneData& data) {
	_shapeList.init(data);

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE("method undefined");
}

void Union::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW("method undefined");
}

bool Union::isInside(Point3D p) const {
	/////////////////////////////////////////////////////
	// Determine if the point is inside the union here //
	/////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}

//////////////////
// Intersection //
//////////////////
double Intersection::intersect(Ray3D ray, RayShapeIntersectionInfo& iInfo, BoundingBox1D range,
                               std::function<bool (double)> validityLambda) const {
	/////////////////////////////////////////////////////////////////////////////////////
	// Compute the intersection of the difference with the intersection of shapes here //
	/////////////////////////////////////////////////////////////////////////////////////
	THROW("method undefined");
}

void Intersection::init(const LocalSceneData& data) {
	_shapeList.init(data);

	///////////////////////////////////
	// Do any additional set-up here //
	///////////////////////////////////
	WARN_ONCE("method undefined");
}

void Intersection::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	THROW("method undefined");
}

bool Intersection::isInside(Point3D p) const {
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the instersection of shapes here //
	///////////////////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}
