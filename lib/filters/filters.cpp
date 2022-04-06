#include <vector>
#include <stdlib.h>
#include "filters.h"
#include <math.h>

BMP Crop::Apply(const BMP &bmp) {
    BMP new_bmp(width_, height_);
    for (size_t i = 0; i < width_; ++i) {
        for (size_t j = 0; j < height_; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(i, j);
            new_bmp.set_pixel(i, j, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    return new_bmp;
}

BMP Grayscale::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP new_bmp(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(i, j);
            int new_col = 0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue;
            new_bmp.set_pixel(i, j, new_col, new_col, new_col, 0);
        }
    }
    return new_bmp;
}

BMP Negative::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP new_bmp(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(i, j);
            new_bmp.set_pixel(i, j, 255 - pixel.blue, 255 - pixel.green, 255 - pixel.red, 0);
        }
    }
    return new_bmp;
}

RedGreenBlue FilterWithMatrix::ApplyMatrix(const std::vector<std::vector<int>>& matrix, int x, int y, BMP bmp) {
    std::vector<std::vector<RedGreenBlue>> vec_pixels;
    int width = bmp.bmp_info_header.width;
    int height = bmp.bmp_info_header.height;
    for (int i = -1; i < 2; ++i) {
        std::vector<RedGreenBlue> line_pixels;
        for (int j = -1; j < 2; ++j) {
            line_pixels.push_back(bmp.get_pixel(std::min(std::max(0, x + i), width - 1), std::min(std::max(0, y + i), height - 1)));
        }
        vec_pixels.push_back(line_pixels);
    }
    RedGreenBlue new_pixel_color;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            new_pixel_color.red += vec_pixels[i][j].red * matrix[i][j];
            new_pixel_color.green += vec_pixels[i][j].green * matrix[i][j];
            new_pixel_color.blue += vec_pixels[i][j].blue * matrix[i][j];
        }
    }
    new_pixel_color.red = std::min(255, std::max(0, new_pixel_color.red));
    new_pixel_color.green = std::min(255, std::max(0, new_pixel_color.green));
    new_pixel_color.blue = std::min(255, std::max(0, new_pixel_color.blue));
    return new_pixel_color;
}

BMP Sharpening::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP new_bmp(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            std::vector<std::vector<int>> matrix = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
            RedGreenBlue pixel = ApplyMatrix(matrix, i, j, bmp);
            new_bmp.set_pixel(i, j, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    return new_bmp;
}

BMP EdgeDetection::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    Grayscale grayscale;
    BMP new_bmp = grayscale.Apply(bmp);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            std::vector<std::vector<int>> matrix = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
            RedGreenBlue pixel = ApplyMatrix(matrix, i, j, bmp);
            if (pixel.blue > threshold_) {
                new_bmp.set_pixel(i, j, 0, 0, 0, 0);
            } else {
                new_bmp.set_pixel(i, j, 255, 255, 255, 0);
            }
        }
    }
    return new_bmp;
}

BMP GaussianBlur::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP blur_horizontal(width, height);
    BMP new_bmp(width, height);
    for (size_t x0 = 0; x0 < width; ++x0) {
        for (size_t y0 = 0; y0 < height; ++y0) {
            RedGreenBlue pixel;
            for (size_t x = 0; x < width; ++x) {
                RedGreenBlue pixel_x_y0 = bmp.get_pixel(x, y0);
                pixel.red += pixel_x_y0.red * exp(-((x0 - x) * (x0 - x) / (2 * sigma_ * sigma_)));
                pixel.green += pixel_x_y0.green * exp(-((x0 - x) * (x0 - x) / (2 * sigma_ * sigma_)));
                pixel.blue += pixel_x_y0.blue * exp(-((x0 - x) * (x0 - x) / (2 * sigma_ * sigma_)));
            }
            pixel.red *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            pixel.green *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            pixel.blue *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            blur_horizontal.set_pixel(x0, y0, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    for (size_t x0 = 0; x0 < width; ++x0) {
        for (size_t y0 = 0; y0 < height; ++y0) {
            RedGreenBlue pixel;
            for (size_t y = 0; y < height; ++y) {
                RedGreenBlue pixel_x0_y = blur_horizontal.get_pixel(x0, y);
                pixel.red += pixel_x0_y.red * exp(-((y0 - y) * (y0 - y) / (2 * sigma_ * sigma_)));
                pixel.green += pixel_x0_y.green * exp(-((y0 - y) * (y0 - y) / (2 * sigma_ * sigma_)));
                pixel.blue += pixel_x0_y.blue * exp(-((y0 - y) * (y0 - y) / (2 * sigma_ * sigma_)));
            }
            pixel.red *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            pixel.green *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            pixel.blue *= (1 / sqrt(2 * M_PI * sigma_ * sigma_));
            new_bmp.set_pixel(x0, y0, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    return new_bmp;
}

BMP Brightness::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP new_bmp(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(i, j);
            new_bmp.set_pixel(i, j, pixel.blue * coef_, pixel.green * coef_, pixel.red * coef_, 0);
        }
    }
    return new_bmp;
}

BMP AddFrame::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    BMP new_bmp(width + 2 * thickness_, height + 2 * thickness_);
    for (size_t i = thickness_; i < width + thickness_; ++i) {
        for (size_t j = thickness_; j < height + thickness_; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(i - thickness_, j - thickness_);
            new_bmp.set_pixel(i, j, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    return new_bmp;
}

BMP Pixelation::Apply(const BMP &bmp) {
    size_t width = bmp.bmp_info_header.width;
    size_t height = bmp.bmp_info_header.height;
    size_t width_of_pixel = width / numb_of_pixels_;
    size_t height_of_pixel = height / numb_of_pixels_;
    BMP new_bmp(width_of_pixel * numb_of_pixels_, height_of_pixel * numb_of_pixels_);
    for (size_t i = 0; i < width_of_pixel * numb_of_pixels_; ++i) {
        for (size_t j = 0; j < height_of_pixel * numb_of_pixels_; ++j) {
            RedGreenBlue pixel = bmp.get_pixel(int(i / width_of_pixel) * width_of_pixel, int(j / width_of_pixel) * width_of_pixel);
            new_bmp.set_pixel(i, j, pixel.blue, pixel.green, pixel.red, 0);
        }
    }
    return new_bmp;
}

