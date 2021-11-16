#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED
#include <Util/geometry.h>
#include <Util/polynomial.h>
#include "shape.h"

namespace Ray {
	/** This class describes a sphere, and is represented by its center and radius. */
	class Sphere : public Shape {
		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the sphere */
		const class Material* _material;

		/** The polynomial used to calculate intersection with a ray */
		Util::Polynomial3D<2> _P;

	public:
		/** The center of the sphere */
		Util::Point3D center;
		/** The radius of the sphere */
		double radius;
		/** The mesh of the sphere */
		std::vector<MeshTriangle> mesh;

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "shape_sphere"; }

		/** The default constructor */
		Sphere(void);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "sphere"; }
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
#endif // SPHERE_INCLUDED
