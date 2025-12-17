cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(3dCapClient)

include(FetchContent)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(3dCapClient ${cpp_src})

if(NOT MSVC)
    target_compile_options(3dCapClient PRIVATE -Wall -Wextra -pedantic)
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(3dCapClient PRIVATE /wd4244)
endif()
set_property(TARGET 3dCapClient PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_features(3dCapClient PUBLIC cxx_std_23)
target_include_directories(3dCapClient PRIVATE src)

set(EIGEN_BUILD_CMAKE_PACKAGE TRUE)
FetchContent_Declare(
    Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 5.0.1
    SYSTEM
)
FetchContent_MakeAvailable(Eigen3)
target_link_libraries(3dCapClient PUBLIC Eigen3::Eigen)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)
target_link_libraries(3dCapClient PUBLIC SFML::Audio SFML::Graphics)

find_package(OpenGL REQUIRED)
target_link_libraries(3dCapClient PUBLIC OpenGL::GLU)

install(TARGETS 3dCapClient DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
