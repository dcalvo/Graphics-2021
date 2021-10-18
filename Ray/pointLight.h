#ifndef POINT_LIGHT_INCLUDED
#define POINT_LIGHT_INCLUDED
#include "light.h"

namespace Ray {
	/** This class describes a point-light light-source. This light has a fixed position and the light attenuates as function of the distance from the light-source. */
	class PointLight : public Light {
		/** The position of the spot-light */
		Util::Point3D _location;

		/** The constant term of the attenuation equation */
		double _constAtten;

		/** The linear term of the attenuation equation */
		double _linearAtten;

		/** The quadratic term of the attenuation equation */
		double _quadAtten;

	public:
		/** This static method returns the directive describing the Light. */
		static std::string Directive(void) { return "light_point"; }

		///////////////////
		// Light methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "point light"; }
		Util::Point3D getAmbient(Util::Ray3D ray, const class RayShapeIntersectionInfo& iInfo) const override;
		Util::Point3D getDiffuse(Util::Ray3D ray, const class RayShapeIntersectionInfo& iInfo) const override;
		Util::Point3D getSpecular(Util::Ray3D ray, const class RayShapeIntersectionInfo& iInfo) const override;
		Util::Point3D getIntensity(Util::Point3D light, const RayShapeIntersectionInfo& iInfo) const;
		bool isInShadow(const class RayShapeIntersectionInfo& iInfo, const class Shape* shape) const override;
		Util::Point3D transparency(const class RayShapeIntersectionInfo& iInfo, const class Shape& shape,
		                           Util::Point3D cLimit) const override;
		void drawOpenGL(int index, GLSLProgram* glslProgram) const override;
	};
}
#endif // POINT_LIGHT_INCLUDED
