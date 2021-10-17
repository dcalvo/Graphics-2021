#include <cmath>
#include <Util/exceptions.h>
#include "camera.h"
#include "shape.h"

using namespace Ray;
using namespace Util;

////////////
// Camera //
////////////

Ray3D Camera::getRay(int i, int j, int width, int height) const {
	const double ar = height / width;
	const double vfov = heightAngle;
	const double hfov = 2 * atan(tan(vfov / 2) / ar);
	const Point3D W = -forward;
	const Point3D U = Point3D::CrossProduct(W, up);
	const Point3D V = Point3D::CrossProduct(W, U);
	const double d = 1000;
	const double l = -tan(hfov / 2) * width;
	const double r = tan(hfov / 2) * width;
	const double b = -tan(vfov / 2) * height;
	const double t = tan(vfov / 2) * height;
	const double u = l + (r - l) * (i + 0.5) / width;
	const double v = b + (t - b) * (j + 0.5) / height;
	const Point3D direction(-d * W + u * U + v * V);
	const Point3D origin(position);
	// debug
	if ((i == 0 && j == 0) || (i == 0 && j == 499) || (i == 499 && j == 0) || (i == 499 && j == 499) || i == 249 && j ==
		249)
		std::cout << Ray3D(origin, direction) << '\n';
	return Ray3D(origin, direction);
}

void Camera::drawOpenGL(void) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	THROW("method undefined");

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

void Camera::rotateUp(Point3D center, float angle) {
	///////////////////////////////////////////////////
	// Rotate the camera about the up direction here //
	///////////////////////////////////////////////////
	THROW("method undefined");
}

void Camera::rotateRight(Point3D center, float angle) {
	//////////////////////////////////////////////////////
	// Rotate the camera about the right direction here //
	//////////////////////////////////////////////////////
	THROW("method undefined");
}

void Camera::moveForward(float dist) {
	//////////////////////////////////
	// Move the camera forward here //
	//////////////////////////////////
	THROW("method undefined");
}

void Camera::moveRight(float dist) {
	///////////////////////////////////////
	// Move the camera to the right here //
	///////////////////////////////////////
	THROW("method undefined");
}

void Camera::moveUp(float dist) {
	/////////////////////////////
	// Move the camera up here //
	/////////////////////////////
	THROW("method undefined");
}
