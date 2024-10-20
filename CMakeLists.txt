cmake_minimum_required(VERSION 3.28)
project(test_sdl2)
find_package(OpenGL)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2_ttf SDL2_image)