cmake_minimum_required(VERSION 3.14.0)

project(Wind 
		VERSION 0.0.0
		LANGUAGES CXX
		DESCRIPTION "Another game engine")

add_subdirectory(src)
add_subdirectory(src/Physics/include)
add_subdirectory(src/Physics/CollisionSystem)
add_subdirectory(src/Graphics)
add_subdirectory(src/Components)

find_package(SDL2 REQUIRED)
