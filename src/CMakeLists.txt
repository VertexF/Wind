add_library(Application_Lib STATIC
						 Application.h Application.cpp
						 Camera.h Camera.cpp
						 Game.h Game.cpp
						 Timer.h Timer.h)

find_package(SDL2 REQUIRED)

add_library(SDL2 STATIC IMPORTED)
find_library(SDL2_LIBRARY_PATH SDL2 HINTS "${CMAKE_CURRENT_LIST_DIR}/../../")
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${SDL2_LIBRARY_PATH}")

message(STATUS "${CMAKE_CURRENT_LIST_DIR}/../../")

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Wind main.cpp)

target_link_libraries(Wind PUBLIC Application_Lib ${SDL2_LIBRARIES})