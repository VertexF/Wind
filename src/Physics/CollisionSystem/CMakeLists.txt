add_library(Collision_Lib STATIC
						collision_broad.h collision_broad.cpp
						collision_narrow.h collision_narrow.cpp
						contact.h contact.cpp
						Geometry.h Geometry.cpp)

target_link_libraries(Wind PUBLIC Collision_Lib)