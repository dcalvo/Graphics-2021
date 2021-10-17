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
	//////////////////////////////////////////////
	// Generate a ray from the camera's position //
	//////////////////////////////////////////////
	const double ar = height / width;
	const double vfov = heightAngle;
	const double hfov = 2 * atan(tan(vfov / 2) / ar);
	const Point3D tr = position + forward - up * tan(vfov / 2) - right * tan(hfov / 2);
	const Point3D tl = position + forward - up * tan(vfov / 2) + right * tan(hfov / 2);
	const Point3D bl = position + forward + up * tan(vfov / 2) + right * tan(hfov / 2);
	const Point3D u = tl + (tr - tl) * (i + 0.5) / width;
	const Point3D v = tl + (bl - tl) * (j + 0.5) / height;
	const Point3D origin(position);
	const Point3D direction = (Point3D(u[0], v[1], v[2]) - position).unit();
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
