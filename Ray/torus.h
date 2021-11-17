#ifndef TORUS_INCLUDED
#define TORUS_INCLUDED
#include <Util/geometry.h>
#include <Util/polynomial.h>
#include "shape.h"
#include "triangle.h"

namespace Ray {
	/** This class describes a torus, and is represented by its center and two radii */
	class Torus : public Shape {
		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the torus */
		const class Material* _material;

	public:
		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "shape_torus"; }

		/** The center of the torus */
		Util::Point3D center;

		/** The inner radius of the torus */
		double iRadius;

		/** The outer radius of the torus */
		double oRadius;

		/** The mesh of the cone */
		std::vector<Triangle> mesh;

		/** The default constructor */
		Torus(void);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "torus"; }
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
#endif // TORUS_INCLUDED
