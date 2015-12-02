cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Set DataAccess build type.")
project(preprocess)

add_definitions(-D_FILE_OFSET_BITS=64)

find_path(BOOST_ROOT boost)

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.42 COMPONENTS system iostreams filesystem timer REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(unif96_to_bin unif96_to_bin.cpp) 

# Find Qt and related files
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(sampling sampling.cpp) 
target_link_libraries(sampling ${QT_LIBRARIES} ${Boost_LIBRARIES})

add_executable(csv2Binary csv2Binary.cpp)
target_link_libraries(csv2Binary ${QT_LIBRARIES} ${Boost_LIBRARIES})

add_executable(newFormatCsv2Binary newFormatCsv2Binary.cpp)
target_link_libraries(newFormatCsv2Binary ${QT_LIBRARIES} ${Boost_LIBRARIES})

add_executable(multiCsv2Binary multiCsv2Binary.cpp)
target_link_libraries(multiCsv2Binary ${QT_LIBRARIES} ${Boost_LIBRARIES})

add_executable(testQuery testQuery.cpp)
target_link_libraries(testQuery ${QT_LIBRARIES} ${Boost_LIBRARIES})

add_executable(build_kdtrip build_kdtrip.cpp) 
target_link_libraries(build_kdtrip ${Boost_LIBRARIES} ${QT_LIBRARIES} ${Boost_LIBRARIES})
