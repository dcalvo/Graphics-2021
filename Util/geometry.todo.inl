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

namespace Util {
	////////////
	// Matrix //
	////////////
	template <unsigned int Dim>
	Matrix<Dim> Matrix<Dim>::Exp(const Matrix& m, int terms) {
		//////////////////////////////////////
		// Compute the matrix exponent here //
		//////////////////////////////////////
		// 0th term
		Matrix<Dim> exponent = Matrix<Dim>::Identity();
		// 1st and beyond terms
		for (int i = 1; i < terms; i++) {
			Matrix<Dim> product = Matrix<Dim>::Identity();
			double n_factorial = 1;
			for (int j = 1; j <= i; j++) product = product * m;
			for (int j = 2; j <= i; j++) n_factorial *= j;
			product = product * (1 / n_factorial);
			exponent = exponent + product;
		}
		return exponent;
	}

	template <unsigned int Dim>
	Matrix<Dim> Matrix<Dim>::closestRotation(void) const {
		///////////////////////////////////////
		// Compute the closest rotation here //
		///////////////////////////////////////
		Matrix<Dim> r1, r2, diagonal;
		SVD(r1, diagonal, r2);
		for (int i = 0; i < Dim; i++) for (int j = 0; j < Dim; j++) if (i == j) diagonal(i, j) = 1.;
		diagonal(Dim, Dim) = (r1 * r2).determinant();
		return r1 * diagonal * r2;
	}

	/////////////////
	// BoundingBox //
	/////////////////
	template <unsigned int Dim>
	BoundingBox<1> BoundingBox<Dim>::intersect(const Ray<Dim>& ray) const {
		///////////////////////////////////////////////////////////////
		// Compute the intersection of a BoundingBox with a Ray here //
		///////////////////////////////////////////////////////////////
		const Point<Dim> bmin = _p[0], bmax = _p[1];
		Point<Dim> inv_dir;
		for (int i = 0; i < Dim; i++)
			inv_dir[i] = 1 / ray.direction[i];
		// adapted from https://tavianator.com/2011/ray_box.html
		double tmin = -Infinity;
		double tmax = Infinity;
		for (int i = 0; i < Dim; i++) {
			const double t1 = (bmin[i] - ray.position[i]) * inv_dir[i];
			const double t2 = (bmax[i] - ray.position[i]) * inv_dir[i];
			tmin = std::max(tmin, std::min(t1, t2));
			tmax = std::min(tmax, std::max(t1, t2));
		}
		auto miss = BoundingBox1D();
		miss[0] = Infinity, miss[1] = -Infinity; // BoundingBox constructor would order these, so do it manually
		return tmax >= tmin && tmax > 0 ? BoundingBox1D(tmin, tmax) : miss;
	}
}
