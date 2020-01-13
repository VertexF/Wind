cmake_minimum_required(VERSION 3.14.0)

project(Wind 
		VERSION 0.0.0
		LANGUAGES CXX
		DESCRIPTION "Another game engine")

#Sets ups the module path to work with the a custom on so the defualt CMake modules work.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

add_subdirectory(src)
add_subdirectory(src/Physics/include)
add_subdirectory(src/Physics/CollisionSystem)
add_subdirectory(src/Graphics)
add_subdirectory(src/Components)

find_package(SDL2 REQUIRED)
target_include_directories(Wind ${SDL_INCLUDE_DIR})