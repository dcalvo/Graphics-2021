#include "image.h"
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <Util/exceptions.h>
#include <random>

using namespace Util;
using namespace Image;

/////////////
// Image32 //
/////////////
Image32 Image32::addRandomNoise(double noise) const {
	const int noise_interpolated = static_cast<int>(255 * noise);
	std::random_device rd_device;
	std::mt19937 gen(rd_device());
	const std::uniform_int_distribution<> distribution(-noise_interpolated, noise_interpolated);
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		pixel.r = std::clamp(pixel.r + distribution(gen), 0, 255);
		pixel.g = std::clamp(pixel.g + distribution(gen), 0, 255);
		pixel.b = std::clamp(pixel.b + distribution(gen), 0, 255);
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::brighten(double brightness) const {
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		pixel.r = std::clamp(static_cast<int>(pixel.r * brightness), 0, 255);
		pixel.g = std::clamp(static_cast<int>(pixel.g * brightness), 0, 255);
		pixel.b = std::clamp(static_cast<int>(pixel.b * brightness), 0, 255);
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::luminance(void) const {
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		const unsigned char luminance = std::clamp(static_cast<int>(pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11), 0,
		                                           255);
		pixel.r = pixel.g = pixel.b = luminance;
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::contrast(double contrast) const {
	double sum = 0;
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		sum += pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11;
	}
	const double avg_luminance = sum / (this->_width * this->_height);
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		pixel.r = std::clamp(static_cast<int>((pixel.r - avg_luminance) * contrast + avg_luminance), 0, 255);
		pixel.g = std::clamp(static_cast<int>((pixel.g - avg_luminance) * contrast + avg_luminance), 0, 255);
		pixel.b = std::clamp(static_cast<int>((pixel.b - avg_luminance) * contrast + avg_luminance), 0, 255);
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::saturate(double saturation) const {
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		const double luminance = pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11;
		pixel.r = std::clamp(static_cast<int>((pixel.r - luminance) * saturation + luminance), 0, 255);
		pixel.g = std::clamp(static_cast<int>((pixel.g - luminance) * saturation + luminance), 0, 255);
		pixel.b = std::clamp(static_cast<int>((pixel.b - luminance) * saturation + luminance), 0, 255);
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::quantize(int bits) const {
	const int num_colors = pow(2, bits);
	const int factor = 256 / num_colors;
	const int quantized_ceiling = 255 / factor;
	for (int i = 0; i < this->_width * this->_height; i++) {
		Pixel32 pixel = this->_pixels[i];
		pixel.r = std::clamp(pixel.r / factor * (255 / quantized_ceiling), 0, 255);
		pixel.g = std::clamp(pixel.g / factor * (255 / quantized_ceiling), 0, 255);
		pixel.b = std::clamp(pixel.b / factor * (255 / quantized_ceiling), 0, 255);
		this->_pixels[i] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::randomDither(int bits) const {
	Image32 noisy_image = this->addRandomNoise(1 / pow(2, bits));
	Image32 quantized_noisy_image = noisy_image.quantize(bits);
	return quantized_noisy_image;
}

Image32 Image32::orderedDither2X2(int bits) const {
	constexpr double matrix[2][2] = {
		{1 / 5.0, 3 / 5.0}, {4 / 5.0, 2 / 5.0}
	};
	const int factor = 255 / static_cast<int>(255 / (256 / pow(2, bits)));
	for (int index = 0; index < this->_width * this->_height; index++) {
		const int i = index % this->_width % 2;
		const int j = index / this->_height % 2;
		Pixel32 pixel = this->_pixels[index];
		double r = pixel.r / 256.0 * (pow(2, bits) - 1);
		double g = pixel.g / 256.0 * (pow(2, bits) - 1);
		double b = pixel.b / 256.0 * (pow(2, bits) - 1);
		r = r - floor(r) > matrix[i][j] ? ceil(r) : floor(r);
		g = g - floor(g) > matrix[i][j] ? ceil(g) : floor(g);
		b = b - floor(b) > matrix[i][j] ? ceil(b) : floor(b);
		pixel.r = std::clamp(static_cast<int>(r) * factor, 0, 255);
		pixel.g = std::clamp(static_cast<int>(g) * factor, 0, 255);
		pixel.b = std::clamp(static_cast<int>(b) * factor, 0, 255);
		this->_pixels[index] = pixel;
	}
	return Image32(*this);
}

Image32 Image32::floydSteinbergDither(int bits) const {
	const int num_colors = pow(2, bits);
	const int factor = 256 / num_colors;
	const int quantized_ceiling = 255 / factor;
	std::vector working_array(this->_width, std::vector<Pixel32>(this->_height));
	// transform 1d to 2d
	for (int i = 0; i < this->_width * this->_height; i++) {
		working_array[i % this->_width][i / this->_width] = this->_pixels[i];
	}
	// dither
	for (int y = 0; y < this->_height; y++) {
		for (int x = 0; x < this->_width; x++) {
			Pixel32 pixel = working_array[x][y];
			// errors
			const int r_e = pixel.r - pixel.r / factor * (255 / quantized_ceiling);
			const int g_e = pixel.g - pixel.g / factor * (255 / quantized_ceiling);
			const int b_e = pixel.b - pixel.b / factor * (255 / quantized_ceiling);

			// quantize
			pixel.r = pixel.r / factor * (255 / quantized_ceiling);
			pixel.g = pixel.g / factor * (255 / quantized_ceiling);
			pixel.b = pixel.b / factor * (255 / quantized_ceiling);
			working_array[x][y] = pixel;

			// distribute error
			if (x + 1 < this->_width) {
				// right
				working_array[x + 1][y].r = std::clamp(static_cast<int>(working_array[x + 1][y].r + r_e * (7 / 16.0)),
				                                       0, 255);
				working_array[x + 1][y].g = std::clamp(static_cast<int>(working_array[x + 1][y].g + g_e * (7 / 16.0)),
				                                       0, 255);
				working_array[x + 1][y].b = std::clamp(static_cast<int>(working_array[x + 1][y].b + b_e * (7 / 16.0)),
				                                       0, 255);
			}
			if (y + 1 < this->_height) {
				if (x > 0) {
					// down left
					working_array[x - 1][y + 1].r = std::clamp(
						static_cast<int>(working_array[x - 1][y + 1].r + r_e * (3 / 16.0)), 0, 255);
					working_array[x - 1][y + 1].g = std::clamp(
						static_cast<int>(working_array[x - 1][y + 1].g + g_e * (3 / 16.0)), 0, 255);
					working_array[x - 1][y + 1].b = std::clamp(
						static_cast<int>(working_array[x - 1][y + 1].b + b_e * (3 / 16.0)), 0, 255);
				}
				// down
				working_array[x][y + 1].r = std::clamp(static_cast<int>(working_array[x][y + 1].r + r_e * (5 / 16.0)),
				                                       0, 255);
				working_array[x][y + 1].g = std::clamp(static_cast<int>(working_array[x][y + 1].g + g_e * (5 / 16.0)),
				                                       0, 255);
				working_array[x][y + 1].b = std::clamp(static_cast<int>(working_array[x][y + 1].b + b_e * (5 / 16.0)),
				                                       0, 255);
				if (x + 1 < this->_width) {
					// down right
					working_array[x + 1][y + 1].r = std::clamp(
						static_cast<int>(working_array[x + 1][y + 1].r + r_e * (1 / 16.0)), 0, 255);
					working_array[x + 1][y + 1].g = std::clamp(
						static_cast<int>(working_array[x + 1][y + 1].g + g_e * (1 / 16.0)), 0, 255);
					working_array[x + 1][y + 1].b = std::clamp(
						static_cast<int>(working_array[x + 1][y + 1].b + b_e * (1 / 16.0)), 0, 255);
				}
			}

		}
	}
	// transform 2d to 1d
	for (int y = 0; y < this->_height; y++) {
		for (int x = 0; x < this->_width; x++) {
			this->_pixels[x + y * this->_width] = working_array[x][y];
		}
	}
	return Image32(*this);
}

Image32 Image32::blur3X3(void) const {
	//////////////////////
	// Do blurring here //
	//////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::edgeDetect3X3(void) const {
	////////////////////////////
	// Do edge detection here //
	////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::scaleNearest(double scaleFactor) const {
	/////////////////////////////////////////////////
	// Do scaling with nearest-point sampling here //
	/////////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::scaleBilinear(double scaleFactor) const {
	////////////////////////////////////////////
	// Do scaling with bilinear sampling here //
	////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::scaleGaussian(double scaleFactor) const {
	////////////////////////////////////////////
	// Do scaling with Gaussian sampling here //
	////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::rotateNearest(double angle) const {
	//////////////////////////////////////////////////
	// Do rotation with nearest-point sampling here //
	//////////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::rotateBilinear(double angle) const {
	/////////////////////////////////////////////
	// Do rotation with bilinear sampling here //
	/////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::rotateGaussian(double angle) const {
	/////////////////////////////////////////////
	// Do rotation with Gaussian sampling here //
	/////////////////////////////////////////////
	THROW("method undefined");
	return Image32();
}

void Image32::setAlpha(const Image32& matte) {
	///////////////////////////
	// Set alpha values here //
	///////////////////////////
	THROW("method undefined");
}

Image32 Image32::composite(const Image32& overlay) const {
	/////////////////////////
	// Do compositing here //
	/////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::CrossDissolve(const Image32& source, const Image32& destination, double blendWeight) {
	////////////////////////////
	// Do cross-dissolve here //
	////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::warp(const OrientedLineSegmentPairs& olsp) const {
	/////////////////////
	// Do warping here //
	/////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::funFilter(void) const {
	////////////////////////////
	// Do the fun-filter here //
	////////////////////////////
	THROW("method undefined");
	return Image32();
}

Image32 Image32::crop(int x1, int y1, int x2, int y2) const {
	//////////////////////
	// Do cropping here //
	//////////////////////
	THROW("method undefined");
	return Image32();
}

Pixel32 Image32::nearestSample(Point2D p) const {
	//////////////////////////////
	// Do nearest sampling here //
	//////////////////////////////
	THROW("method undefined");
	return Pixel32();
}

Pixel32 Image32::bilinearSample(Point2D p) const {
	///////////////////////////////
	// Do bilinear sampling here //
	///////////////////////////////
	THROW("method undefined");
	return Pixel32();
}

Pixel32 Image32::gaussianSample(Point2D p, double variance, double radius) const {
	///////////////////////////////
	// Do Gaussian sampling here //
	///////////////////////////////
	THROW("method undefined");
	return Pixel32();
}
