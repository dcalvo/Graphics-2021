#include <cmath>
#include <random>
#include <Util/exceptions.h>
#include "scene.h"
#include "sphereLight.h"

using namespace Ray;
using namespace Util;

/////////////////
// SphereLight //
/////////////////
Point3D SphereLight::transparency(const RayShapeIntersectionInfo& iInfo, const Shape& shape, Point3D cLimit,
                                  unsigned int samples) const {
	//////////////////////////////////////////////////////////
	// Compute the transparency along the path to the light //
	//////////////////////////////////////////////////////////
	// Normally distributed numbers will end up uniformly distributed once the vector is normalized
	// See http://corysimon.github.io/articles/uniformdistn-on-sphere/
	std::default_random_engine gen(std::random_device{}());
	std::normal_distribution dist(0.0, 1.0);
	Point3D shadow_sum;
	for (unsigned int i = 0; i < samples; i++) {
		Point3D sample_location = _location + Point3D(dist(gen), dist(gen), dist(gen)).unit() * _radius;
		const Point3D dirTowardsLight = (sample_location - iInfo.position).unit();
		Ray3D ray(iInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
		RayShapeIntersectionInfo occlusionInfo;
		const BoundingBox1D range(Point1D(Epsilon), Point1D((sample_location - iInfo.position).length()));
		Point3D shadow_sample(1., 1., 1.);
		while (!isinf(shape.intersect(ray, occlusionInfo, range)) &&
			(shadow_sample[0] > cLimit[0] && shadow_sample[1] > cLimit[1] && shadow_sample[2] > cLimit[2])) {
			shadow_sample *= occlusionInfo.material->transparent;
			ray = Ray3D(occlusionInfo.position + dirTowardsLight * Epsilon, dirTowardsLight);
		}
		shadow_sum = shadow_sum + shadow_sample;
	}
	return shadow_sum / samples;
}
