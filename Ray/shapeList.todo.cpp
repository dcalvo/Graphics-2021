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
	_shape0->updateBoundingBox();
	_bBox = _shape0->boundingBox();
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
	std::vector<ShapeBoundingBoxHit> hits;
	for (const auto shape : shapes) {
		ShapeBoundingBoxHit hit{};
		const BoundingBox1D r = shape->boundingBox().intersect(ray);
		if (r.isEmpty() || r[0][0] > range[1][0]) continue;
		hit.t = r[0][0] > 0 ? r[0][0] : r[1][0];
		hit.shape = shape;
		hits.push_back(hit);
	}
	std::sort(hits.begin(), hits.end(), ShapeBoundingBoxHit::Compare);
	for (const auto hit : hits) {
		RayShapeIntersectionInfo thisInfo;
		const double d = hit.shape->intersect(ray, thisInfo, range, validityLambda);
		if (isinf(d) || !range.isInside(d)) continue;
		iInfo = thisInfo;
		return d;
	}
	return Infinity;
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
}

void ShapeList::updateBoundingBox(void) {
	///////////////////////////////
	// Set the _bBox object here //
	///////////////////////////////
	auto pList = new Point3D[shapes.size() * 2];
	for (int i = 0; i < shapes.size(); i++) {
		shapes[i]->updateBoundingBox();
		_bBox = shapes[i]->boundingBox();
		pList[2 * i] = _bBox[0];
		pList[2 * i + 1] = _bBox[1];
	}
	_bBox = BoundingBox3D(pList, static_cast<int>(shapes.size()) * 2);
	delete[] pList;
}

void ShapeList::initOpenGL(void) {
	// Initialize the children
	for (int i = 0; i < shapes.size(); i++) shapes[i]->initOpenGL();

	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////
}

void ShapeList::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	for (const auto shape : shapes) {
		shape->drawOpenGL(glslProgram);
	}

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
	iInfo.normal = (localToGlobalNormal * iInfo.normal).unit();
	const double d = (iInfo.position - ray.position).length();
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
	_shape->updateBoundingBox();
	_bBox = getMatrix() * _shape->boundingBox();
}

void AffineShape::drawOpenGL(GLSLProgram* glslProgram) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	const Matrix4D localToGlobal = getMatrix();
	std::vector<double> m;
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) m.push_back(localToGlobal(j, i));
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(m.data());
	_shape->drawOpenGL(glslProgram);
	glPopMatrix();
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
	_material->drawOpenGL(glslProgram);
#ifdef NEW_SHADER_CODE
	if (glslProgram) {
		static bool firstTime = true;
		GLdouble modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glslProgram->setUniformMatrix<4>("modelview", modelview, firstTime);
		Matrix3D m;
		for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) m(j, i) = modelview[i * 4 + j];
		m = m.inverse().transpose();
		for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) modelview[i * 3 + j] = m(j, i);
		glslProgram->setUniformMatrix<3>("modelview_inverse_transpose", modelview, firstTime);
		firstTime = false;

		glBindVertexArray(_vertexArrayID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glDrawElements(GL_TRIANGLES, 3 * _tNum, GL_UNSIGNED_INT, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
#endif // NEW_SHADER_CODE
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	for (const auto shape : _shapeList.shapes) {
		shape->drawOpenGL(glslProgram);
	}

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
}

void TriangleList::initOpenGL(void) {
	_shapeList.initOpenGL();
#ifdef NEW_SHADER_CODE

	std::vector<TriangleIndex> triangles;
	_shapeList.addTrianglesOpenGL(triangles);
	_tNum = static_cast<unsigned>(triangles.size());

	auto vertexData = new GLfloat[_vNum * (3 + 3 + 2)];
	GLfloat* v = vertexData + _vNum * 0;
	GLfloat* n = vertexData + _vNum * 3;
	GLfloat* t = vertexData + _vNum * 6;

	for (int i = 0; i < static_cast<int>(_vNum); i++) {
		for (int j = 0; j < 3; j++) v[3 * i + j] = static_cast<GLfloat>(_vertices[i].position[j]);
		for (int j = 0; j < 3; j++) n[3 * i + j] = static_cast<GLfloat>(_vertices[i].normal[j]);
		for (int j = 0; j < 2; j++) t[2 * i + j] = static_cast<GLfloat>(_vertices[i].texCoordinate[j]);
	}
	glGenBuffers(1, &_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, 8 * _vNum * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] vertexData;

	glGenBuffers(1, &_elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint) * 3, &triangles[0][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &_vertexArrayID);
	glBindVertexArray(_vertexArrayID);

	glEnableVertexAttribArray(0);
	glBindVertexBuffer(0, _vertexBufferID, 0, sizeof(GLfloat) * 3);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);

	glEnableVertexAttribArray(1);
	glBindVertexBuffer(1, _vertexBufferID, sizeof(GLfloat) * 3 * _vNum, sizeof(GLfloat) * 3);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);

	glBindVertexArray(0);
#endif // NEW_SHADER_CODE
	///////////////////////////
	// Do OpenGL set-up here //
	///////////////////////////

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
	_shapeList.updateBoundingBox();
	_bBox = _shapeList.shapes[0]->boundingBox();
	for (int i = 1; i < _shapeList.shapes.size(); i++) _bBox += _shapeList.shapes[i]->boundingBox();
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
	_shapeList.updateBoundingBox();
	_bBox = _shapeList.shapes[0]->boundingBox();
	for (int i = 1; i < _shapeList.shapes.size(); i++) _bBox ^= _shapeList.shapes[i]->boundingBox();
}

bool Intersection::isInside(Point3D p) const {
	///////////////////////////////////////////////////////////////////////
	// Determine if the point is inside the instersection of shapes here //
	///////////////////////////////////////////////////////////////////////
	THROW("method undefined");
	return false;
}
