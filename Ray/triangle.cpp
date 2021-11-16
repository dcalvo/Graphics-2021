#include <cmath>
#include <Util/exceptions.h>
#include "triangle.h"

using namespace Ray;
using namespace Util;

//////////////
// Triangle //
//////////////
Triangle::Triangle(void) { _v[0] = _v[1] = _v[2] = nullptr; }

Triangle::Triangle(Vertex* v0, Vertex* v1, Vertex* v2) { _v[0] = v0, _v[1] = v1, _v[2] = v2; }

void Triangle::_read(std::istream& stream) {
	for (int i = 0; i < 3; i++)
		if (!(stream >> _vIndices[i]))
			THROW("failed to read index for %s", Directive().c_str());
}

void Triangle::_write(std::ostream& stream) const {
	WriteInset(stream);
	stream << "#" << Directive() << "  " << _vIndices[0] << " " << _vIndices[1] << " " << _vIndices[2];
}

bool Triangle::isInside(Point3D p) const { return false; }

void Triangle::addTrianglesOpenGL(std::vector<TriangleIndex>& triangles) {
	triangles.push_back(TriangleIndex(static_cast<int>(_vIndices[0]), static_cast<int>(_vIndices[1]),
	                                  static_cast<int>(_vIndices[2])));
}

size_t Triangle::primitiveNum(void) const { return 1; }
