/*
Copyright (c) 2019, Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/
#include <cmath>
#include <SVD/SVDFit.h>
#include <SVD/MatrixMNTC.h>
#include <Util/exceptions.h>
#include "geometry.h"

namespace Util {
	////////////////////////////
	// EulerRotationParameter //
	////////////////////////////
	Matrix3D EulerRotationParameter::operator()(void) const {
		///////////////////////////////////////////////
		// Transform Euler angles to a rotation here //
		///////////////////////////////////////////////
		Matrix3D x, y, z;
		// from the "Modeling Transformations" lecture
		x(0, 0) = 1;
		x(1, 1) = cos(parameter[0]);
		x(1, 2) = -sin(parameter[0]);
		x(2, 1) = sin(parameter[0]);
		x(2, 2) = cos(parameter[0]);

		y(0, 0) = cos(parameter[1]);
		y(0, 2) = sin(parameter[1]);
		y(1, 1) = 1;
		y(2, 0) = -sin(parameter[1]);
		y(2, 2) = cos(parameter[1]);

		z(0, 0) = cos(parameter[2]);
		z(0, 1) = -sin(parameter[2]);
		z(1, 0) = sin(parameter[2]);
		z(1, 1) = cos(parameter[2]);
		z(2, 2) = 1;
		return z * y * x;
	}

	/////////////////////////////////
	// QuaternionRotationParameter //
	/////////////////////////////////
	Matrix3D QuaternionRotationParameter::operator()(void) const {
		///////////////////////////////////////////////
		// Transform a quaternion to a rotation here //
		///////////////////////////////////////////////
		const auto unit_q = parameter.unit();
		const double a = unit_q.real, b = unit_q.imag[0], c = unit_q.imag[1], d = unit_q.imag[2];
		Matrix3D r;
		r(0, 0) = 1 - 2 * c * c - 2 * d * d;
		r(0, 1) = 2 * b * c - 2 * a * d;
		r(0, 2) = 2 * b * d + 2 * a * c;
		r(1, 0) = 2 * b * c + 2 * a * d;
		r(1, 1) = 1 - 2 * b * b - 2 * d * d;
		r(1, 2) = 2 * c * d - 2 * a * b;
		r(2, 0) = 2 * b * d - 2 * a * c;
		r(2, 1) = 2 * c * d + 2 * a * b;
		r(2, 2) = 1 - 2 * b * b - 2 * c * c;
		return r;
	}
}
