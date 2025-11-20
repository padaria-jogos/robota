# PROJETO ROBOTA
cmake_minimum_required(VERSION 3.26)
project(robota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# dependencies
find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# auto find source files
set(SOURCE_DIRS
        ${PROJECT_SOURCE_DIR}/Source
)

file(GLOB_RECURSE SRC_FILES
        CONFIGURE_DEPENDS
        ${SOURCE_DIRS}/*.cpp
        ${SOURCE_DIRS}/*.h
        ${SOURCE_DIRS}/*.tpp
)

# executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# includes
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIRS})

# libs
target_link_libraries(${PROJECT_NAME} PRIVATE
        GLEW::GLEW
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        OpenGL::GL
)
