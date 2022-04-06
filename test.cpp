#include <catch.hpp>
#include "image_processor.h"

#include <cctype>

TEST_CASE("ImageProcessor") {
    //home/iya/cpp-base-hse-2022/projects/image_processor/examples/example.bmp /home/iya/cpp-base-hse-2022/projects/image_processor/examples/out.bmp -crop 500 500 -gs -neg -sharp -edge 5 -blur 1 -brightness 1 -frame 50 -pixelation 50
    const char* in_path = "/home/iya/cpp-base-hse-2022/projects/image_processor/examples/example.bmp";
    const char* out_path = "/home/iya/cpp-base-hse-2022/projects/image_processor/examples/out.bmp";
    const char*  prog_name = "/home/iya/cpp-base-hse-2022/cmake-build-debug/image_processor";

    const char* argv1[3] = {prog_name, in_path, out_path};
    ParseResults parse_results = ArgParser::Parse(3, argv1);
    REQUIRE(parse_results.in_path == in_path);
    REQUIRE(parse_results.out_path == out_path);
    REQUIRE(parse_results.filter_descriptions.empty());

    const char* argv2[4] = {prog_name, in_path, out_path, "-gs"};
    ParseResults parse_results2 = ArgParser::Parse(4, argv2);
    REQUIRE(parse_results2.filter_descriptions[0].name == "-gs");
    REQUIRE(parse_results2.filter_descriptions[0].parameters.empty());

    const char* argv3[7] = {prog_name, in_path, out_path, "-gs", "-crop", "500", "600"};
    ParseResults parse_results3 = ArgParser::Parse(7, argv3);
    REQUIRE(parse_results3.filter_descriptions[0].name == "-gs");
    REQUIRE(parse_results3.filter_descriptions[0].parameters.empty());
    REQUIRE(parse_results3.filter_descriptions[1].name == "-crop");
    REQUIRE(parse_results3.filter_descriptions[1].parameters[0] == "500");
    REQUIRE(parse_results3.filter_descriptions[1].parameters[1] == "600");

    const char* argv4[15] = {prog_name, in_path, out_path, "-gs", "-crop", "500", "600", "-neg", "-sharp", "-edge", "5", "-blur", "1", "-brightness", "1"};
    ParseResults parse_results4 = ArgParser::Parse(15, argv4);
    REQUIRE(parse_results4.filter_descriptions[0].name == "-gs");
    REQUIRE(parse_results4.filter_descriptions[0].parameters.empty());
    REQUIRE(parse_results4.filter_descriptions[1].name == "-crop");
    REQUIRE(parse_results4.filter_descriptions[1].parameters[0] == "500");
    REQUIRE(parse_results4.filter_descriptions[1].parameters[1] == "600");
    REQUIRE(parse_results4.filter_descriptions[2].name == "-neg");
    REQUIRE(parse_results4.filter_descriptions[2].parameters.empty());
    REQUIRE(parse_results4.filter_descriptions[3].name == "-sharp");
    REQUIRE(parse_results4.filter_descriptions[3].parameters.empty());
    REQUIRE(parse_results4.filter_descriptions[4].name == "-edge");
    REQUIRE(parse_results4.filter_descriptions[4].parameters[0] == "5");
    REQUIRE(parse_results4.filter_descriptions[5].name == "-blur");
    REQUIRE(parse_results4.filter_descriptions[5].parameters[0] == "1");
    REQUIRE(parse_results4.filter_descriptions[6].name == "-brightness");
    REQUIRE(parse_results4.filter_descriptions[6].parameters[0] == "1");
}
