#pragma once
#include <iostream>
#include <string>
#include <vector>

struct FilterDescription {
    std::string name;
    std::vector<std::string> parameters;
};

struct ParseResults {
    std::string in_path;
    std::string out_path;
    std::vector<FilterDescription> filter_descriptions;
};

class ArgParser {
public:
    static ParseResults Parse(int argc, const char *argv[]);
};