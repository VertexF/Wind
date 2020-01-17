cmake_minimum_required(VERSION 3.14.0)

project(Wind 
		VERSION 0.0.0
		LANGUAGES CXX
		DESCRIPTION "Another game engine")
		
#Sets ups the module path to work with the a custom on so the defualt CMake modules work.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}../../../glew-2.1.0/")

set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}../../../SDL2-2.0.10/include")
set(SDL2_LIBRARY "${CMAKE_SOURCE_DIR}../../../SDL2-2.0.10/lib/x64/SDL2.lib")
set(SDL2MAIN_LIBRARY "${CMAKE_SOURCE_DIR}../../../SDL2-2.0.10/lib/x64/SDL2main.lib")
set(IL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}../../../DevIL Windows SDK/include")
set(IL_LIBRARIES "${CMAKE_SOURCE_DIR}../../../DevIL Windows SDK/lib/x64/Release/DevIL.lib")
set(ILU_LIBRARIES "${CMAKE_SOURCE_DIR}../../../DevIL Windows SDK/lib/x64/Release/ILU.lib")
set(ILUT_LIBRARIES "${CMAKE_SOURCE_DIR}../../../DevIL Windows SDK/lib/x64/Release/ILUT.lib")

find_package(SDL2 REQUIRED COMPONENTS main PATH SDL2_LIB)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${IL_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(src/Physics/include)
add_subdirectory(src/Physics/CollisionSystem)
add_subdirectory(src/Graphics)
add_subdirectory(src/Components)

add_executable(Wind src/main.cpp)

target_link_libraries(Wind PRIVATE Application_Lib Core_Lib Collision_Lib Graphics_Lib Component_Lib)
target_link_libraries(Wind PRIVATE SDL2::Core SDL2::Main GLEW::GLEW ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES})