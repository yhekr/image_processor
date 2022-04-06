#include "image_processor.h"

int main(int argc, const char *argv[]) {
    //home/iya/cpp-base-hse-2022/projects/image_processor/examples/example.bmp /home/iya/cpp-base-hse-2022/projects/image_processor/examples/out.bmp -crop 500 500 -gs -neg -sharp -edge 5 -blur 1 -brightness 1 -frame 50 -pixelation 50
    if (argc == 1) {
        std::cout << "Enter the name of the input and output files and the list of filters.";
        return 0;
    }
    ParseResults parse_results = ArgParser::Parse(argc, argv);
    Controller controller(parse_results);
    controller.ApplyFilters();
    return 0;
}
