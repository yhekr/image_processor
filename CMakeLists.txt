cmake_minimum_required(VERSION 3.17)
project(image_processor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(image_processor
    image_processor.cpp
        lib/parser/parser.cpp
        lib/BMP/BMP.cpp
        lib/filters/filters.cpp
        lib/controller/controller.cpp
        )

add_catch(test_image_processor
        image_processor.cpp
        lib/parser/parser.cpp
        lib/BMP/BMP.cpp
        lib/filters/filters.cpp
        lib/controller/controller.cpp
        test.cpp
        )
