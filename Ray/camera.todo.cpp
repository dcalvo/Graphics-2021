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
	const double ar = static_cast<double>(height) / width;
	const double vfov = heightAngle;
	const double hfov = 2 * atan(tan(vfov / 2) / ar);
	const Point3D tl = position + forward + up * tan(vfov / 2) - right * tan(hfov / 2);
	const Point3D tr = position + forward + up * tan(vfov / 2) + right * tan(hfov / 2);
	const Point3D bl = position + forward - up * tan(vfov / 2) - right * tan(hfov / 2);
	const Point3D u = tl + (tr - tl) * (i + 0.5) / width;
	const Point3D v = bl + (tl - bl) * (j + 0.5) / height;
	const Point3D origin(position);
	const Point3D direction = (Point3D(u[0], v[1], v[2]) - position).unit();
	return Ray3D(origin, direction);
}

void Camera::drawOpenGL(void) const {
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Point3D center(position + forward);
	gluLookAt(position[0], position[1], position[2],
	          center[0], center[1], center[2],
	          up[0], up[1], up[2]);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

// Helper function to rotate vector an angle about an axis
Point3D rotateVector(const Point3D& vector, const Point3D& axis, const float angle) {
	// adapted from https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	const float sin_theta = sin(angle);
	const float cos_theta = cos(angle);
	Point3D rotated_vector = vector * cos_theta + Point3D::CrossProduct(axis, vector) * sin_theta + axis.dot(vector) *
		axis * (1 - cos_theta);
	return rotated_vector;
}

void Camera::rotateUp(Point3D center, float angle) {
	///////////////////////////////////////////////////
	// Rotate the camera about the up direction here //
	///////////////////////////////////////////////////
	position = rotateVector(position, up, angle);
	forward = rotateVector(forward, up, angle);
	right = rotateVector(right, up, angle);
}

void Camera::rotateRight(Point3D center, float angle) {
	//////////////////////////////////////////////////////
	// Rotate the camera about the right direction here //
	//////////////////////////////////////////////////////
	position = rotateVector(position, right, angle);
	forward = rotateVector(forward, right, angle);
	up = rotateVector(up, right, angle);
}

void Camera::moveForward(float dist) {
	//////////////////////////////////
	// Move the camera forward here //
	//////////////////////////////////
	position += forward * dist;
}

void Camera::moveRight(float dist) {
	///////////////////////////////////////
	// Move the camera to the right here //
	///////////////////////////////////////
	position += right * dist;
}

void Camera::moveUp(float dist) {
	/////////////////////////////
	// Move the camera up here //
	/////////////////////////////
	position += up * dist;
}
