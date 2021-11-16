#ifndef GROUP_INCLUDED
#define GROUP_INCLUDED
#include <vector>
#include <unordered_map>
#include <Util/geometry.h>
#include "shape.h"

namespace Ray {
	/** This abstract class represents a Shape with an affine transformation associated to it */
	class AffineShape : public Shape {
	protected:
		/** The shape to be transformed */
		Shape* _shape;
	public:
		/** The default constructor */
		AffineShape(void);

		/** This method returns the transformation associated with the list. */
		virtual Util::Matrix4D getMatrix(void) const = 0;

		/** This method returns the inverse of the transformation associated with the list. */
		virtual Util::Matrix4D getInverseMatrix(void) const = 0;

		/** This method returns the transformation that acts on the surface normals. */
		virtual Util::Matrix3D getNormalMatrix(void) const = 0;

		///////////////////
		// Shape methods //
		///////////////////
	public:
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		bool isInside(Util::Point3D p) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		size_t primitiveNum(void) const override;
	};


	/** This class derived from AffineShape represents a shape in the scene graph whose transformation matrix is constant. */
	class StaticAffineShape : public AffineShape {
		/** The static transformation associated to the shape */
		Util::Matrix4D _localTransform;

		/** The inverse of the static transformation associated to the shape */
		Util::Matrix4D _inverseTransform;

		/** The static normal transformation associated to the shape */
		Util::Matrix3D _normalTransform;
	public:
		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "static_affine"; }

		/** The default constructor */
		StaticAffineShape(void);

		/** This method initializes the transform with the prescribed matrix.*/
		void set(Util::Matrix4D m);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "static affine"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;

		/////////////////////////
		// AffineShape methods //
		/////////////////////////
	public:
		Util::Matrix4D getMatrix(void) const override;
		Util::Matrix4D getInverseMatrix(void) const override;
		Util::Matrix3D getNormalMatrix(void) const override;
	};

	/** This class derived from AffineShape represents a shape in the scene graph whose transformation matrix is prameterized. */
	class DynamicAffineShape : public AffineShape {
		/** The name of the parameter associated with the dynamic transformation */
		std::string _paramName;

		/** A pointer to the matrix storing the current transformation  */
		const Util::Matrix4D* _matrix;
	public:
		/** The default constructor */
		DynamicAffineShape(void);

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "dynamic_affine"; }

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "dynamic affine"; }
		void init(const class LocalSceneData& data) override;

		/////////////////////////
		// AffineShape methods //
		/////////////////////////
	public:
		Util::Matrix4D getMatrix(void) const override;
		Util::Matrix4D getInverseMatrix(void) const override;
		Util::Matrix3D getNormalMatrix(void) const override;
	};

	/** This class represents the difference of two shapes. */
	class Difference : public Shape {
		/** The shapes whose differences we are representing */
		Shape *_shape0, *_shape1;

	public:
		/** The default constructor */
		Difference(Shape* shape0 = nullptr, Shape* shape1 = nullptr);

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return "shape_difference"; }

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "difference"; }
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

	/** This class can be used for sorting shapes based on the intersections of their bounding volumes with a given ray.*/
	class ShapeBoundingBoxHit {
	public:
		/** The time along the ray to the point of intersection */
		double t;

		/** The shape that was intersected */
		const Shape* shape;

		/** This is a static method for sorting the hits by the order in which they occured.
		*** For example after the bounding volumes have been intersected and the distance and shapes have been written into
		*** an array of RayShapes, the array can be sorted by calling std::sort. */
		static bool Compare(const ShapeBoundingBoxHit& v1, const ShapeBoundingBoxHit& v2);
	};

	/** This class represents a node in the scene graph containing one or more shapes */
	class ShapeList : public Shape {
		friend class Union;
		friend class Intersection;

		/** This static method returns the directive header describing the shape. */
		static std::string _DirectiveHeader(void) { return "shape_list"; }
	public:
		/** The set of shape factoendDirectiveries */
		static std::unordered_map<std::string, Util::BaseFactory<Shape>*> ShapeFactories;

		/** This static method returns the directive describing the shape. */
		static std::string Directive(void) { return _DirectiveHeader() + std::string("_begin"); }

		/** The shapes that are associated to the node */
		std::vector<Shape*> shapes;

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "shape list"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		bool isInside(Util::Point3D p) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		void addTrianglesOpenGL(std::vector<class TriangleIndex>& triangles) override;
		size_t primitiveNum(void) const override;
	};

	/** This class represents a node which stores a triangle list. It's children can only be Triangles or TrivialShapeLists.*/
	class TriangleList : public Shape {
		friend class Scene;

#ifdef NEW_SHADER_CODE
		/** The OpenGL vertex array identifier */
		GLuint _vertexArrayID = 0;
#endif // NEW_SHADER_CODE

		/** The OpenGL vertex buffer identifier */
		GLuint _vertexBufferID = 0;

		/** The OpenGL element buffer identifier */
		GLuint _elementBufferID = 0;

		/** The number of triangles in the list */
		unsigned int _tNum;

		/** The number of vertices in the list */
		unsigned int _vNum;

		/** The list of vertices */
		const class Vertex* _vertices;

		/** The list of shapes */
		ShapeList _shapeList;

		/** The index of the material associated with the box */
		int _materialIndex;

		/** The material associated to all triangles within the list */
		const class Material* _material;
	public:
		/** This static method returns the directive header describing the shape. */
		static std::string Directive(void) { return "shape_triangles"; }

		/** The default constructor */
		TriangleList(void);

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "triangles"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		bool isInside(Util::Point3D p) const override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		void addTrianglesOpenGL(std::vector<TriangleIndex>& triangles) override;
		size_t primitiveNum(void) const override;
	};

	/** This class represents a node which stores the union of a set of shapes*/
	class Union : public Shape {
		/** The list of shapes */
		ShapeList _shapeList;
	public:
		/** This static method returns the directive header describing the shape. */
		static std::string Directive(void) { return "shape_union"; }

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "union"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		bool isInside(Util::Point3D p) const override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		size_t primitiveNum(void) const override;
	};


	/** This class represents a node which stores the intersection of a set of shapes*/
	class Intersection : public Shape {
		/** The list of shapes */
		ShapeList _shapeList;

	public:
		/** This static method returns the directive header describing the shape. */
		static std::string Directive(void) { return "shape_intersection"; }

		///////////////////
		// Shape methods //
		///////////////////
	private:
		void _write(std::ostream& stream) const override;
		void _read(std::istream& stream) override;
	public:
		std::string name(void) const override { return "intersection"; }
		void init(const class LocalSceneData& data) override;
		void initOpenGL(void) override;
		void updateBoundingBox(void) override;
		bool isInside(Util::Point3D p) const override;
		double intersect(Util::Ray3D ray, class RayShapeIntersectionInfo& iInfo,
		                 Util::BoundingBox1D range = Util::BoundingBox1D(Util::Epsilon, Util::Infinity),
		                 std::function<bool (double)> validityFunction = [](double t) { return true; }) const override;
		void drawOpenGL(GLSLProgram* glslProgram) const override;
		size_t primitiveNum(void) const override;
	};
}
#endif // GROUP_INCLUDED
