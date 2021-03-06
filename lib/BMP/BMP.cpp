#include "BMP.h"

BMP::BMP(const std::string& fname) {
    read(fname);
}

void BMP::read(const std::string& fname) {
    std::ifstream inp(fname);
    inp.read((char *)&file_header, sizeof(file_header));
    if (file_header.file_type != 0x4D42) {
        throw std::runtime_error("Error! Unrecognized file format.");
    }
    inp.read((char *)&bmp_info_header, sizeof(bmp_info_header));

    // The BMPColorHeader is used only for transparent images

    // Jump to the pixel data location
    inp.seekg(file_header.offset_data, inp.beg);

    // Adjust the header fields for output.
    // Some editors will put extra info in the image file, we only save the headers and the data.
    bmp_info_header.size = sizeof(BMPInfoHeader);
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = file_header.offset_data;

    if (bmp_info_header.height < 0) {
        throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
    }

    data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

    // Here we check if we need to take into account row padding
    row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
    uint32_t new_stride = make_stride_aligned(4);
    std::vector<uint8_t> padding_row(new_stride - row_stride);

    for (int y = 0; y < bmp_info_header.height; ++y) {
        inp.read((char *)(data.data() + row_stride * y), row_stride);
        inp.read((char *)padding_row.data(), padding_row.size());
    }
    file_header.file_size +=
        static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());

}

BMP::BMP(int32_t width, int32_t height, bool has_alpha) {
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("The image width and height must be positive numbers.");
    }

    bmp_info_header.width = width;
    bmp_info_header.height = height;
    bmp_info_header.size = sizeof(BMPInfoHeader);
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    bmp_info_header.bit_count = 24;
    bmp_info_header.compression = 0;
    row_stride = width * 3;
    data.resize(row_stride * height);

    uint32_t new_stride = make_stride_aligned(4);
    file_header.file_size = file_header.offset_data + static_cast<uint32_t>(data.size()) +
                            bmp_info_header.height * (new_stride - row_stride);
}

void BMP::write(const std::string& fname) {
    std::ofstream of{fname, std::ios_base::binary};
    uint32_t new_stride = make_stride_aligned(4);
    std::vector<uint8_t> padding_row(new_stride - row_stride);

    write_headers(of);

    for (int y = 0; y < bmp_info_header.height; ++y) {
        of.write((const char *)(data.data() + row_stride * y), row_stride);
        of.write((const char *)padding_row.data(), padding_row.size());
    }
}

void BMP::fill_region(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
    if (x0 + w > (uint32_t)bmp_info_header.width || y0 + h > (uint32_t)bmp_info_header.height) {
        throw std::runtime_error("The region does not fit in the image!");
    }

    uint32_t channels = bmp_info_header.bit_count / 8;
    for (uint32_t y = y0; y < y0 + h; ++y) {
        for (uint32_t x = x0; x < x0 + w; ++x) {
            data[channels * (y * bmp_info_header.width + x) + 0] = B;
            data[channels * (y * bmp_info_header.width + x) + 1] = G;
            data[channels * (y * bmp_info_header.width + x) + 2] = R;
            if (channels == 4) {
                data[channels * (y * bmp_info_header.width + x) + 3] = A;
            }
        }
    }
}

void BMP::set_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
    if (x0 >= (uint32_t)bmp_info_header.width || y0 >= (uint32_t)bmp_info_header.height || x0 < 0 || y0 < 0) {
        throw std::runtime_error("The point is outside the image boundaries!");
    }

    uint32_t channels = bmp_info_header.bit_count / 8;
    data[channels * (y0 * bmp_info_header.width + x0) + 0] = B;
    data[channels * (y0 * bmp_info_header.width + x0) + 1] = G;
    data[channels * (y0 * bmp_info_header.width + x0) + 2] = R;
    if (channels == 4) {
        data[channels * (y0 * bmp_info_header.width + x0) + 3] = A;
    }
}

RedGreenBlue BMP::get_pixel(uint32_t x0, uint32_t y0) const {
    if (x0 >= (uint32_t)bmp_info_header.width || y0 >= (uint32_t)bmp_info_header.height || x0 < 0 || y0 < 0) {
        throw std::runtime_error("The point is outside the image boundaries!");
    }

    uint32_t channels = bmp_info_header.bit_count / 8;
    int B = data[channels * (y0 * bmp_info_header.width + x0) + 0];
    int G = data[channels * (y0 * bmp_info_header.width + x0) + 1];
    int R = data[channels * (y0 * bmp_info_header.width + x0) + 2];
    RedGreenBlue pixel;
    pixel.blue = B;
    pixel.red = R;
    pixel.green = G;
    return pixel;
}

void BMP::write_headers(std::ofstream &of) {
    of.write((const char *)&file_header, sizeof(file_header));
    of.write((const char *)&bmp_info_header, sizeof(bmp_info_header));
    if (bmp_info_header.bit_count == 32) {
        of.write((const char *)&bmp_color_header, sizeof(bmp_color_header));
    }
}

void BMP::write_headers_and_data(std::ofstream &of) {
    write_headers(of);
    of.write((const char *)data.data(), data.size());
}

// Add 1 to the row_stride until it is divisible with align_stride
uint32_t BMP::make_stride_aligned(uint32_t align_stride) {
    uint32_t new_stride = row_stride;
    while (new_stride % align_stride != 0) {
        new_stride++;
    }
    return new_stride;
}

// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void BMP::check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
        throw std::runtime_error(
            "Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
    }
    if (expected_color_header.color_space_type != bmp_color_header.color_space_type) {
        throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
    }
}
