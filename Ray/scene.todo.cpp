#include <cmath>
#include <Util/exceptions.h>
#include "scene.h"

using namespace Ray;
using namespace Util;

///////////
// Scene //
///////////
Point3D Scene::Reflect(Point3D v, Point3D n) {
	//////////////////
	// Reflect here //
	//////////////////
	return (v - 2 * v.dot(n) * n).unit();
}

bool Scene::Refract(Point3D v, Point3D n, double ir, Point3D& refract) {
	//////////////////
	// Refract here //
	//////////////////
	double i_factor = 1 / ir;
	// if v.dot(n) > 0, then we're leaving the object
	if (v.dot(n) > 0) {
		i_factor = ir; // air index is 1, so just ir
		n = -n; // flip n so the rest of the calculation still works
	}
	const double cos_t = fabs(v.dot(n));
	const double radicand = 1 - (i_factor * i_factor) * (1 - (cos_t * cos_t));
	if (radicand < 0) return false;
	const double cos_r = sqrt(radicand);
	refract = (i_factor * v + (i_factor * cos_t - cos_r) * n).unit();
	return true;
}

Point3D Scene::getColor(Ray3D ray, int rDepth, Point3D cLimit, unsigned int lightSamples) {
	////////////////////////////////////////////////
	// Get the color associated with the ray here //
	////////////////////////////////////////////////
	Point3D I;
	if (!rDepth || (cLimit[0] > 1 && cLimit[1] > 1 && cLimit[2] > 1)) return I;
	RayShapeIntersectionInfo iInfo;
	const double d = this->intersect(ray, iInfo);
	if (isinf(d)) return I;

	// compute color
	Point3D emissive_contrib = iInfo.material->emissive;
	Point3D surface_contrib;
	const auto lights = _globalData.lights;
	Point3D ambient_sum;
	for (const auto light : lights) {
		ambient_sum = ambient_sum + light->getAmbient(ray, iInfo);
	}
	for (const auto light : lights) {
		Point3D ambient = iInfo.material->ambient * ambient_sum;
		Point3D diffuse = light->getDiffuse(ray, iInfo);
		Point3D specular = light->getSpecular(ray, iInfo);
		Point3D shadow = light->transparency(iInfo, *this, cLimit, lightSamples);
		if (iInfo.material->tex) {
			const double u = iInfo.material->tex->_image.width() * iInfo.texture[0];
			const double v = iInfo.material->tex->_image.height() * iInfo.texture[1];
			const Image::Pixel32 pix = iInfo.material->tex->_image.bilinearSample(Point2D(u, v));
			const Point3D tex(pix.r / 255., pix.g / 255., pix.b / 255.);
			emissive_contrib *= tex;
			ambient *= tex;
			diffuse *= tex;
			specular *= tex;
		}
		surface_contrib = surface_contrib + ambient + (diffuse + specular) * shadow;
	}

	Point3D reflect_contrib;
	if (ray.direction.dot(iInfo.normal) < 0) {
		Ray3D reflect;
		reflect.direction = Reflect(ray.direction, iInfo.normal);
		reflect.position = iInfo.position + reflect.direction * Epsilon;
		const Point3D specularity = iInfo.material->specular;
		reflect_contrib = getColor(reflect, rDepth - 1, cLimit / specularity, lightSamples) * specularity;
	}

	Point3D refract_contrib;
	Ray3D refract;
	if (Refract(ray.direction, iInfo.normal, iInfo.material->ir, refract.direction)) {
		refract.position = iInfo.position + refract.direction * Epsilon;
		const Point3D transparency = iInfo.material->transparent;
		refract_contrib = getColor(refract, rDepth - 1, cLimit / transparency, lightSamples) * transparency;
	}

	I = emissive_contrib + surface_contrib + reflect_contrib + refract_contrib;
	I[0] = std::clamp(I[0], 0., 1.);
	I[1] = std::clamp(I[1], 0., 1.);
	I[2] = std::clamp(I[2], 0., 1.);
	return I;
}

//////////////
// Material //
//////////////
void Material::drawOpenGL(GLSLProgram* glslProgram) const {
#ifdef NEW_SHADER_CODE
	if (glslProgram) {
		static bool firstTime = true;
		glslProgram->setUniform<3>("material_emissive", &emissive[0], firstTime);
		glslProgram->setUniform<3>("material_ambient", &ambient[0], firstTime);
		glslProgram->setUniform<3>("material_diffuse", &diffuse[0], firstTime);
		glslProgram->setUniform<3>("material_specular", &specular[0], firstTime);
		glslProgram->setUniform("material_specular_shininess", specularFallOff, firstTime);
		firstTime = false;
	}
#endif // NEW_SHADER_CODE
	//////////////////////////////
	// Do OpenGL rendering here //
	//////////////////////////////
	// RGBA values
	const float emissive_param[4] = {emissive[0], emissive[1], emissive[2], 1.};
	const float ambient_param[4] = {ambient[0], ambient[1], ambient[2], 1.};
	const float diffuse_param[4] = {diffuse[0], diffuse[1], diffuse[2], 1.};
	const float specular_param[4] = {specular[0], specular[1], specular[2], 1.};

	glMaterialfv(GL_FRONT, GL_EMISSION, emissive_param);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_param);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_param);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_param);
	glMaterialf(GL_FRONT, GL_SHININESS, specularFallOff);

	if (tex) {
		glBindTexture(GL_TEXTURE_2D, tex->_openGLHandle);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glEnable(GL_TEXTURE_2D);
	}
	else glDisable(GL_TEXTURE_2D);

	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}

/////////////
// Texture //
/////////////
void Texture::initOpenGL(void) {
	///////////////////////////////////
	// Do OpenGL texture set-up here //
	/////////////////////////////////////
	std::vector<unsigned char> flattened_img;
	const int height = _image.height();
	const int width = _image.width();
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			const Image::Pixel32 pix = _image(i, j);
			flattened_img.push_back(pix.r);
			flattened_img.push_back(pix.g);
			flattened_img.push_back(pix.b);
			flattened_img.push_back(pix.a);
		}
	glGenTextures(1, &_openGLHandle);
	glBindTexture(GL_TEXTURE_2D, _openGLHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flattened_img.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	// Sanity check to make sure that OpenGL state is good
	ASSERT_OPEN_GL_STATE();
}
