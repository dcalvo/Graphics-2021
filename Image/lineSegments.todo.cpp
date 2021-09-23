#include "lineSegments.h"
#include <math.h>
#include <Util/exceptions.h>

using namespace Util;
using namespace Image;

////////////////////////////
// Image processing stuff //
////////////////////////////
double OrientedLineSegment::length( void ) const
{
	const Point2D p = this->endPoints[0];
	const Point2D q = this->endPoints[1];
	return Point2D().Distance(p, q); 
}
double OrientedLineSegment::distance( Point2D x ) const
{
	const Point2D p = this->endPoints[0];
	const Point2D q = this->endPoints[1];
	if (this->length() == 0) {
		return Point2D().Distance(p, x);
	}
	const double v = (x - p).dot(this->perpendicular()) / this->length();
	const double u = (x - p).dot(q - p) / (q - p).squareNorm();
	if (u < 0) {
		return Point2D().Distance(p, x);
	}
	if (u > 1) {
		return Point2D().Distance(q, x);
	}
	return abs(v);
}
Point2D OrientedLineSegment::perpendicular( void ) const
{
	const Point2D p = this->endPoints[0];
	const Point2D q = this->endPoints[1];
	const int dx = q[0] - p[0];
	const int dy = q[1] - p[1];
	return Point2D(-dy, dx);
}

Point2D OrientedLineSegment::GetSourcePosition( const OrientedLineSegment& source , const OrientedLineSegment& destination , Point2D target )
{
	const Point2D p = destination.endPoints[0];
	const Point2D q = destination.endPoints[1];
	const Point2D p_prime = source.endPoints[0];
	const Point2D q_prime = source.endPoints[1];
	const double u = (target - p).dot(q - p) / (q - p).squareNorm();
	const double v = (target - p).dot(destination.perpendicular()) / destination.length();
	const Point2D src_point = p_prime + (u * (q_prime - p_prime)) + ((v * source.perpendicular()) / (q_prime - p_prime).length());
	return src_point + target;
}