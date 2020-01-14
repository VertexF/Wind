cmake_minimum_required(VERSION 3.14.0)

project(Wind 
		VERSION 0.0.0
		LANGUAGES CXX
		DESCRIPTION "Another game engine")

#Sets ups the module path to work with the a custom on so the defualt CMake modules work.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

set(SDL2_INCLUDE_DIR "C:/SDL2-2.0.10/include")
set(SDL2_LIBRARY "C:/SDL2-2.0.10/lib/x86")

find_package(SDL2 REQUIRED COMPONENTS main PATH SDL2_LIB)
#find_library(SDL2_LIBRARY NAMES SDL2.lib SDL2main.lib)
#find_file(SDL2_INCLUDE_DIR SDL2 "C:/SDL2-2.0.10/include")
#target_link_directories(Application_Lib ${SDL2_INCLUDE_DIR})


include_directories(Wind ${SDL2_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(src/Physics/include)
add_subdirectory(src/Physics/CollisionSystem)
add_subdirectory(src/Graphics)
add_subdirectory(src/Components)

target_link_libraries(Wind PRIVATE ${SDL2_LIB})
