#include "parser.h"

ParseResults ArgParser::Parse(int argc, const char *argv[]) {
    if (argc == 2) {
        throw std::runtime_error("Error! Enter the name of the input AND output files and the list of filters.");
    }
    std::string in_path;
    std::string out_path;
    std::vector<FilterDescription> filter_descriptions;
    int n = 0;
    //./image_processor input.bmp /tmp/output.bmp -crop 800 600 -gs -blur 0.5
    int i = 0;
    std::string filt_name;
    std::vector<std::string> parameters;
    while (i < argc) {
        std::string s = argv[i];
        if (n == 1) {
            in_path = s;
        } else if (n == 2) {
            out_path = s;
        } else if (s[0] == '-') {
            if (n != 3) {
                FilterDescription new_filt;
                new_filt.name = filt_name;
                new_filt.parameters = parameters;
                filter_descriptions.push_back(new_filt);
                parameters = {};
            }
            filt_name = s;
        } else if (n > 0) {
            parameters.push_back(s);
        }
        ++i;
        ++n;
    }
    if (argc == 3) {
        std::cout << "No filters to apply.";
        return {in_path, out_path, {}};
    }
    FilterDescription new_filt;
    new_filt.name = filt_name;
    new_filt.parameters = parameters;
    filter_descriptions.push_back(new_filt);
    return {in_path, out_path, filter_descriptions};
}