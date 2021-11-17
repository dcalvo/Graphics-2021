#ifndef CONE_INCLUDED
#define CONE_INCLUDED
#include <Util/geometry.h>
#include <Util/polynomial.h>
#include "shape.h"
#include "triangle.h"

namespace Ray {
	/** This class represents a cone whose central axis is parallel to the y-axis, and 
	* is defined by the center of the cone, the height from the tip to the base
	* and the base of the cone. */
	class Cone : public Shape {
		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the cone */
		const class Material* _material;
	public:
		/** The center of the cone */
		Util::Point3D center;

		/** The height of the cone */
		double height;

		/** The radius of the cone */
		double radius;

		/** The mesh of the cone */
		std::vector<Triangle> mesh;

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "shape_cone"; }

		/** The default constructor */
		Cone(void);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "cone"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		bool isInside(Util::Point3D p) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		size_t primitiveNum(void) const override;
	};
}
#endif // CONE_INCLUDED
