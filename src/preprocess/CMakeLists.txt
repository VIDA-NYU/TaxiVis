cmake_minimum_required(VERSION 3.10)
project(preprocess)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_FILE_OFSET_BITS=64)

# Find Boost
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.42 COMPONENTS iostreams filesystem timer REQUIRED)

# Find Qt6
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt" CACHE PATH "Qt installation path")
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# unif96_to_bin - converts old uniform 96-byte format to binary Trip format
add_executable(unif96_to_bin unif96_to_bin.cpp)
target_link_libraries(unif96_to_bin ${Boost_LIBRARIES})

# sampling - spatial sampling with census tract geometry
add_executable(sampling sampling.cpp)
target_link_libraries(sampling Qt6::Core Qt6::Gui Qt6::Widgets ${Boost_LIBRARIES})

# csv2Binary - converts single CSV file to binary Trip format
add_executable(csv2Binary csv2Binary.cpp)
target_link_libraries(csv2Binary Qt6::Core ${Boost_LIBRARIES})

# newFormatCsv2Binary - converts newer CSV format to binary Trip format
add_executable(newFormatCsv2Binary newFormatCsv2Binary.cpp)
target_link_libraries(newFormatCsv2Binary Qt6::Core ${Boost_LIBRARIES})

# multiCsv2Binary - processes multiple CSV files listed in index file
add_executable(multiCsv2Binary multiCsv2Binary.cpp)
target_link_libraries(multiCsv2Binary Qt6::Core ${Boost_LIBRARIES})

# testQuery - tests KdTrip query functionality
add_executable(testQuery testQuery.cpp)
target_link_libraries(testQuery Qt6::Core ${Boost_LIBRARIES})

# build_kdtrip - builds KD-tree spatial index from binary Trip data
add_executable(build_kdtrip build_kdtrip.cpp)
target_link_libraries(build_kdtrip ${Boost_LIBRARIES})

if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_executable(kdtrip_check ../../tests/kdtrip_check.cpp)
    target_include_directories(kdtrip_check PRIVATE ../TaxiVis)
    target_link_libraries(kdtrip_check ${Boost_LIBRARIES})
    add_test(NAME kdtrip_regression
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/kdtrip_regression.py
            $<TARGET_FILE:build_kdtrip> $<TARGET_FILE:kdtrip_check>
            ${CMAKE_CURRENT_SOURCE_DIR}/../../data/sample_merged_1.kdtrip)
    set_tests_properties(kdtrip_regression PROPERTIES
        ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1")
endif()
