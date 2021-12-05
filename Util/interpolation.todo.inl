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

#include <iostream>
#include <math.h>
#include <Util/exceptions.h>

namespace Util {
	///////////////////
	// Interpolation //
	///////////////////
	template <typename SampleType>
	SampleType Interpolation::Sample(const std::vector<SampleType>& samples, double t, int interpolationType) {
		switch (interpolationType) {
		case NEAREST:
			{
				t *= samples.size();
				const int it1 = static_cast<int>(floor(t));
				const int it2 = (it1 + 1) % samples.size();
				t -= it1;
				if (t < 0.5) return samples[it1];
				return samples[it2];
			}
		case LINEAR:
			{
				///////////////////////////////////////
				// Perform linear interpolation here //
				///////////////////////////////////////
				t *= samples.size();
				const int it1 = static_cast<int>(floor(t));
				const int it2 = (it1 + 1) % samples.size();
				t -= it1;
				return samples[it1] + (samples[it2] - samples[it1]) * t;
			}
		case CATMULL_ROM:
			{
				////////////////////////////////////////////
				// Perform Catmull-Rom interpolation here //
				////////////////////////////////////////////
				t *= samples.size();
				const int it1 = static_cast<int>(floor(t));
				const int it2 = (it1 + 1) % samples.size(); // choose k = 2
				const int it3 = (it1 + 2) % samples.size();
				const int it4 = (it1 + 3) % samples.size();
				t -= it1;
				// blending functions
				constexpr double s = 0.5; // default from the slides
				const double c_0 = -s * pow(t, 3) + 2 * s * pow(t, 2) - s * t;
				const double c_1 = (2 - s) * pow(t, 3) + (s - 3) * pow(t, 2) + 1;
				const double c_2 = (s - 2) * pow(t, 3) + (3 - 2 * s) * pow(t, 2) + s * t;
				const double c_3 = s * pow(t, 3) - s * pow(t, 2);
				return c_0 * samples[it1] + c_1 * samples[it2] + c_2 * samples[it3] + c_3 * samples[it4];
			}
		case UNIFORM_CUBIC_B_SPLINE:
			{
				///////////////////////////////////////////////////////
				// Perform uniform cubic b-spline interpolation here //
				///////////////////////////////////////////////////////
				t *= samples.size();
				const int it1 = static_cast<int>(floor(t));
				const int it2 = (it1 + 1) % samples.size(); // choose k = 2
				const int it3 = (it1 + 2) % samples.size();
				const int it4 = (it1 + 3) % samples.size();
				t -= it1;
				// blending functions
				constexpr double s = 0.5; // default from the slides
				const double c_0 = (1 / 3. - s) * pow(t, 3) + (2 * s - 1 / 2.) * pow(t, 2) - s * t + 1 / 6.;
				const double c_1 = (1 - s) * pow(t, 3) + (s - 3 / 2.) * pow(t, 2) + 2 / 3.;
				const double c_2 = (s - 1) * pow(t, 3) + (3 / 2. - 2 * s) * pow(t, 2) + s * t + 1 / 6.;
				const double c_3 = (s - 1 / 3.) * pow(t, 3) + (1 / 2. - s) * pow(t, 2);
				return c_0 * samples[it1] + c_1 * samples[it2] + c_2 * samples[it3] + c_3 * samples[it4];
			}
		default:
			ERROR_OUT("unrecognized interpolation type");
			return samples[0];
		}
	}
}
