#ifndef BOX_INCLUDED
#define BOX_INCLUDED
#include <Util/geometry.h>
#include "shape.h"
#include "triangle.h"

namespace Ray {
	/** This class represents a box and is defined by its center and the length of the sides. */
	class Box : public Shape {
		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated with the box */
		const class Material* _material;
	public:
		/** The center of the box */
		Util::Point3D center;

		/** The lengths of the sides of the box */
		Util::Point3D length;

		/** The mesh of the box */
		std::vector<Triangle> mesh;

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "shape_box"; }

		/** The default constructor */
		Box(void);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "box"; }
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
#endif // BOX_INCLUDED
