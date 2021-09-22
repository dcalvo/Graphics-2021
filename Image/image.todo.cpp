#include "image.h"
#include <algorithm>
#include <iomanip>
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
	auto dithered_image = Image32(*this);
	// dither
	for (int y = 0; y < this->_height; y++) {
		for (int x = 0; x < this->_width; x++) {
			Pixel32 pixel = dithered_image(x, y);
			// errors
			const int r_e = pixel.r - pixel.r / factor * (255 / quantized_ceiling);
			const int g_e = pixel.g - pixel.g / factor * (255 / quantized_ceiling);
			const int b_e = pixel.b - pixel.b / factor * (255 / quantized_ceiling);

			// quantize
			pixel.r = pixel.r / factor * (255 / quantized_ceiling);
			pixel.g = pixel.g / factor * (255 / quantized_ceiling);
			pixel.b = pixel.b / factor * (255 / quantized_ceiling);
			dithered_image(x, y) = pixel;

			// distribute error
			if (x + 1 < this->_width) {
				auto pixel = dithered_image(x + 1, y);
				// right
				pixel.r = std::clamp(static_cast<int>(pixel.r + r_e * (7 / 16.0)), 0, 255);
				pixel.g = std::clamp(static_cast<int>(pixel.g + g_e * (7 / 16.0)), 0, 255);
				pixel.b = std::clamp(static_cast<int>(pixel.b + b_e * (7 / 16.0)), 0, 255);
				dithered_image(x + 1, y) = pixel;
			}
			if (y + 1 < this->_height) {
				if (x > 0) {
					auto pixel = dithered_image(x - 1, y + 1);
					// down left
					pixel.r = std::clamp(static_cast<int>(pixel.r + r_e * (3 / 16.0)), 0, 255);
					pixel.g = std::clamp(static_cast<int>(pixel.g + g_e * (3 / 16.0)), 0, 255);
					pixel.b = std::clamp(static_cast<int>(pixel.b + b_e * (3 / 16.0)), 0, 255);
					dithered_image(x - 1, y + 1) = pixel;
				}
				auto pixel = dithered_image(x, y + 1);
				// down
				pixel.r = std::clamp(static_cast<int>(pixel.r + r_e * (5 / 16.0)), 0, 255);
				pixel.g = std::clamp(static_cast<int>(pixel.g + g_e * (5 / 16.0)), 0, 255);
				pixel.b = std::clamp(static_cast<int>(pixel.b + b_e * (5 / 16.0)), 0, 255);
				dithered_image(x, y + 1) = pixel;
				if (x + 1 < this->_width) {
					auto pixel = dithered_image(x + 1, y + 1);
					// down right
					pixel.r = std::clamp(static_cast<int>(pixel.r + r_e * (1 / 16.0)), 0, 255);
					pixel.g = std::clamp(static_cast<int>(pixel.g + g_e * (1 / 16.0)), 0, 255);
					pixel.b = std::clamp(static_cast<int>(pixel.b + b_e * (1 / 16.0)), 0, 255);
					dithered_image(x + 1, y + 1) = pixel;
				}
			}

		}
	}
	return dithered_image;
}

Image32 Image32::blur3X3(void) const {
	// technically, this matrix is flipped both vertically and horizontally so (i, j) is (j, i),
	// but it doesn't really matter since it's a gaussian filter
	constexpr double kernel[3][3] =
	{
		{1 / 16.0, 2 / 16.0, 1 / 16.0},
		{2 / 16.0, 4 / 16.0, 2 / 16.0},
		{1 / 16.0, 2 / 16.0, 1 / 16.0}
	};
	constexpr int k_rows = 3, k_cols = 3;
	auto blurred_image = Image32();
	blurred_image.setSize(this->_width, this->_height);
	// blur
	for (int y = 0; y < this->_height; y++) {
		for (int x = 0; x < this->_width; x++) {
			double accumulator_r = 0;
			double accumulator_g = 0;
			double accumulator_b = 0;
			// convolve
			for (int i = 0; i < k_rows; i++) {
				const int ii = k_rows - 1 - i; // reverse index (j, i) -> (i, j)
				for (int j = 0; j < k_cols; j++) {
					const int jj = k_cols - 1 - j; // reverse index (j, i) -> (i, j)

					// overlay filter on top of input
					const int yy = y + (k_cols / 2 - jj);
					const int xx = x + (k_rows / 2 - ii);

					if (yy >= 0 && yy < this->_height && xx >= 0 && xx < this->_width) {
						const auto pixel = (*this)(xx, yy);
						accumulator_r += pixel.r * kernel[ii][jj];
						accumulator_g += pixel.g * kernel[ii][jj];
						accumulator_b += pixel.b * kernel[ii][jj];
					} // else, it's a black pixel (zero padding)
				}
			}
			blurred_image(x, y).r = std::clamp(static_cast<int>(accumulator_r), 0, 255);
			blurred_image(x, y).g = std::clamp(static_cast<int>(accumulator_g), 0, 255);
			blurred_image(x, y).b = std::clamp(static_cast<int>(accumulator_b), 0, 255);
		}
	}
	return blurred_image;
}

Image32 Image32::edgeDetect3X3(void) const {
	constexpr double kernel[3][3] =
	{
		{-1 / 8.0, -1 / 8.0, -1 / 8.0},
		{-1 / 8.0, 1.0, -1 / 8.0},
		{-1 / 8.0, -1 / 8.0, -1 / 8.0}
	};
	constexpr int k_rows = 3, k_cols = 3;
	auto edge_image = Image32();
	edge_image.setSize(this->_width, this->_height);
	// blur
	for (int y = 0; y < this->_height; y++) {
		for (int x = 0; x < this->_width; x++) {
			double accumulator_r = 0;
			double accumulator_g = 0;
			double accumulator_b = 0;
			// convolve
			for (int i = 0; i < k_rows; i++) {
				const int ii = k_rows - 1 - i; // reverse index (j, i) -> (i, j)
				for (int j = 0; j < k_cols; j++) {
					const int jj = k_cols - 1 - j; // reverse index (j, i) -> (i, j)

					// overlay filter on top of input
					const int yy = y + (k_cols / 2 - jj);
					const int xx = x + (k_rows / 2 - ii);

					if (yy >= 0 && yy < this->_height && xx >= 0 && xx < this->_width) {
						const auto pixel = (*this)(xx, yy);
						accumulator_r += pixel.r * kernel[ii][jj];
						accumulator_g += pixel.g * kernel[ii][jj];
						accumulator_b += pixel.b * kernel[ii][jj];
					} // else, it's a black pixel (zero padding)
				}
			}
			edge_image(x, y).r = std::clamp(static_cast<int>(accumulator_r), 0, 255);
			edge_image(x, y).g = std::clamp(static_cast<int>(accumulator_g), 0, 255);
			edge_image(x, y).b = std::clamp(static_cast<int>(accumulator_b), 0, 255);
		}
	}
	edge_image.brighten(10);
	return edge_image;
}

Image32 Image32::scaleNearest(double scaleFactor) const {
	auto scaled_image = Image32();
	scaled_image.setSize(this->_width * scaleFactor, this->_height * scaleFactor);
	for (int i = 0; i < scaled_image._width * scaled_image._height; i++) {
		const int x = i % scaled_image._width;
		const int y = i / scaled_image._width;
		const double source_x = x / scaleFactor;
		const double source_y = y / scaleFactor;
		const Pixel32 pixel = nearestSample(Point2D(source_x, source_y));
		scaled_image._pixels[i] = pixel;
	}
	return scaled_image;
}

Image32 Image32::scaleBilinear(double scaleFactor) const {
	auto scaled_image = Image32();
	scaled_image.setSize(this->_width * scaleFactor, this->_height * scaleFactor);
	for (int i = 0; i < scaled_image._width * scaled_image._height; i++) {
		const int x = i % scaled_image._width;
		const int y = i / scaled_image._width;
		const double source_x = x / scaleFactor;
		const double source_y = y / scaleFactor;
		const Pixel32 pixel = bilinearSample(Point2D(source_x, source_y));
		scaled_image._pixels[i] = pixel;
	}
	return scaled_image;
}

Image32 Image32::scaleGaussian(double scaleFactor) const {
	const double variance = pow(1 / scaleFactor, 2);
	const double radius = 2 / scaleFactor;
	auto scaled_image = Image32();
	scaled_image.setSize(this->_width * scaleFactor, this->_height * scaleFactor);
	for (int i = 0; i < scaled_image._width * scaled_image._height; i++) {
		const int x = i % scaled_image._width;
		const int y = i / scaled_image._width;
		const double source_x = x / scaleFactor;
		const double source_y = y / scaleFactor;
		const Pixel32 pixel = gaussianSample(Point2D(source_x, source_y), variance, radius);
		scaled_image._pixels[i] = pixel;
	}
	return scaled_image;
}

Image32 Image32::rotateNearest(double angle) const {
	const double cos_angle = cos(angle * Pi / 180), sin_angle = sin(angle * Pi / 180);
	auto rotated_image = Image32();
	rotated_image.setSize(this->_width, this->_height);
	const int center_x = (this->_width - 1) / 2, center_y = (this->_height - 1) / 2;
	for (int i = 0; i < rotated_image._width * rotated_image._height; i++) {
		const int x = i % rotated_image._width;
		const int y = i / rotated_image._width;
		const double source_x = (x - center_x) * cos_angle - (y - center_y) * sin_angle + center_x;
		const double source_y = (x - center_x) * sin_angle + (y - center_y) * cos_angle + center_y;
		const Pixel32 pixel = nearestSample(Point2D(source_x, source_y));
		rotated_image._pixels[i] = pixel;
	}
	return rotated_image;
}

Image32 Image32::rotateBilinear(double angle) const {
	const double cos_angle = cos(angle * Pi / 180), sin_angle = sin(angle * Pi / 180);
	auto rotated_image = Image32();
	rotated_image.setSize(this->_width, this->_height);
	const int center_x = (this->_width - 1) / 2, center_y = (this->_height - 1) / 2;
	for (int i = 0; i < rotated_image._width * rotated_image._height; i++) {
		const int x = i % rotated_image._width;
		const int y = i / rotated_image._width;
		const double source_x = (x - center_x) * cos_angle - (y - center_y) * sin_angle + center_x;
		const double source_y = (x - center_x) * sin_angle + (y - center_y) * cos_angle + center_y;
		const Pixel32 pixel = bilinearSample(Point2D(source_x, source_y));
		rotated_image._pixels[i] = pixel;
	}
	return rotated_image;
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
	// 0,0 is upper left
	const int left_x = x1 < x2 ? x1 : x2;
	const int right_x = x1 > x2 ? x1 : x2;
	const int top_y = y1 < y2 ? y1 : y2;
	const int bot_y = y1 > y2 ? y1 : y2;
	const int cropped_width = right_x - left_x;
	const int cropped_height = bot_y - top_y;
	if (left_x < 0 || top_y < 0 || right_x > this->_width || bot_y > this->_height)
		THROW("corner outside of image");
	const auto cropped_image = new Image32();
	cropped_image->setSize(cropped_width, cropped_height);
	int i = 0;
	for (int y = 0; y < this->_height; y++) {
		if (y < top_y || y >= bot_y) continue;
		for (int x = 0; x < this->_width; x++) {
			if (x < left_x || x >= right_x) continue ;
			cropped_image->_pixels[i++] = this->_pixels[x + this->_width * y];
		}
	}
	return *cropped_image;
}

Pixel32 Image32::nearestSample(Point2D p) const {
	const int nearest_x = floor(p[0] + 0.5);
	const int nearest_y = floor(p[1] + 0.5);
	if (nearest_x >= 0 && nearest_x < this->_width && nearest_y >= 0 && nearest_y < this->_height)
		return (*this)(nearest_x, nearest_y);
	return Pixel32();
}

Pixel32 Image32::bilinearSample(Point2D p) const {
	const int u1 = floor(p[0]), u2 = u1 + 1;
	const int v1 = floor(p[1]), v2 = v1 + 1;
	if (u1 < 0 || u1 >= this->_width || v1 < 0 || v1 >= this->_height)
		return Pixel32();
	const double du = p[0] - u1;
	// (u1, v1) is known to be safe, unsure about the others
	const Pixel32 src_u1_v1 = (*this)(u1, v1);
	const Pixel32 src_u2_v1 = u2 >= 0 && u2 < this->_width ? (*this)(u2, v1) : src_u1_v1;
	const double a_r = src_u1_v1.r * (1 - du) + src_u2_v1.r * du;
	const double a_g = src_u1_v1.g * (1 - du) + src_u2_v1.g * du;
	const double a_b = src_u1_v1.b * (1 - du) + src_u2_v1.b * du;
	const Pixel32 src_u1_v2 = v2 >= 0 && v2 < this->_height ? (*this)(u1, v2) : src_u1_v1;
	const Pixel32 src_u2_v2 = u2 >= 0 && u2 < this->_width && v2 >= 0 && v2 < this->_height
		                          ? (*this)(u2, v2)
		                          : src_u1_v1;
	const double b_r = src_u1_v2.r * (1 - du) + src_u2_v2.r * du;
	const double b_g = src_u1_v2.g * (1 - du) + src_u2_v2.g * du;
	const double b_b = src_u1_v2.b * (1 - du) + src_u2_v2.b * du;
	const double dv = p[1] - v1;
	auto dst = Pixel32();
	dst.r = std::clamp(static_cast<int>(a_r * (1 - dv) + b_r * dv), 0, 255);
	dst.g = std::clamp(static_cast<int>(a_g * (1 - dv) + b_g * dv), 0, 255);
	dst.b = std::clamp(static_cast<int>(a_b * (1 - dv) + b_b * dv), 0, 255);
	return dst;
}

Pixel32 Image32::gaussianSample(Point2D p, double variance, double radius) const {
	const double sigma = sqrt(variance);
	std::vector kernel(2 * radius + 1, std::vector<double>(2 * radius + 1));
	double sum = 0;
	// create filter
	for (int row = 0; row < kernel.size(); row++) {
		for (int col = 0; col < kernel[row].size(); col++) {
			const double a = (row - radius) / sigma;
			const double b = (col - radius) / sigma;
			const double n = std::exp(-0.5 * a * a) * std::exp(-0.5 * b * b);
			kernel[row][col] = n;
			sum += n;
		}
	}
	//// normalize
	//for (auto& row : kernel) {
	//	for (double& col : row) {
	//		col /= sum;
	//	}
	//}
	// sample
	double accumulator_r = 0;
	double accumulator_g = 0;
	double accumulator_b = 0;
	const double x = p[0];
	const double y = p[1];
	sum = 0;
	// convolve
	for (int i = 0; i < kernel.size(); i++) {
		const int ii = kernel.size() - 1 - i; // reverse index (j, i) -> (i, j)
		for (int j = 0; j < kernel[i].size(); j++) {
			const int jj = kernel[i].size() - 1 - j; // reverse index (j, i) -> (i, j)

			// overlay filter on top of input
			// use the nearest pixel at each sample location
			const int yy = floor(y + (kernel[i].size() / 2 - jj) + 0.5);
			const int xx = floor(x + (kernel.size() / 2 - ii) + 0.5);

			if (yy >= 0 && yy < this->_height && xx >= 0 && xx < this->_width) {
				const auto pixel = (*this)(xx, yy);
				accumulator_r += pixel.r * kernel[ii][jj];
				accumulator_g += pixel.g * kernel[ii][jj];
				accumulator_b += pixel.b * kernel[ii][jj];
				sum += kernel[ii][jj];
			} // else, it's a black pixel (zero padding)
		}
	}
	auto pixel = Pixel32();
	pixel.r = accumulator_r / sum;
	pixel.g = accumulator_g / sum;
	pixel.b = accumulator_b / sum;
	return pixel;
}
