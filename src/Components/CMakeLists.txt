add_library(Component_Lib STATIC
						Block.h Block.cpp
						CompRegistration.h CompRegistration.cpp
						LevelGeometry.h LevelGeometry.cpp
						Plane.h Plane.cpp
						Player.h Player.cpp)

target_link_libraries(Wind PUBLIC Component_Lib)