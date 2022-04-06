#include "controller.h"

void Controller::ApplyFilters() {
    BMP bmp(parse_results_.in_path);
    auto filters = parse_results_.filter_descriptions;
    for (auto filter: filters) {
        if (filter.name == "-crop") {
            auto param = filter.parameters;
            Crop crop(std::stoi(param[0]), std::stoi(param[1]));
            bmp = crop.Apply(bmp);
        } else if (filter.name == "-gs") {
            Grayscale grayscale;
            bmp = grayscale.Apply(bmp);
        } else if (filter.name == "-neg") {
            Negative negative;
            bmp = negative.Apply(bmp);
        } else if (filter.name == "-sharp") {
            Sharpening sharpening;
            bmp = sharpening.Apply(bmp);
        } else if (filter.name == "-edge") {
            auto param = filter.parameters;
            EdgeDetection edge_detection(std::stof(param[0]));
            bmp = edge_detection.Apply(bmp);
        } else if (filter.name == "-blur") {
            auto param = filter.parameters;
            GaussianBlur gaussian_blur(std::stof(param[0]));
            bmp = gaussian_blur.Apply(bmp);
        } else if (filter.name == "-brightness") {
            auto param = filter.parameters;
            Brightness brightness(std::stof(param[0]));
            bmp = brightness.Apply(bmp);
        } else if (filter.name == "-frame") {
            auto param = filter.parameters;
            AddFrame add_frame(std::stoi(param[0]));
            bmp = add_frame.Apply(bmp);
        } else if (filter.name == "-pixelation") {
            auto param = filter.parameters;
            Pixelation pixelation(std::stoi(param[0]));
            bmp = pixelation.Apply(bmp);
        }
    }
    bmp.write(parse_results_.out_path);
}