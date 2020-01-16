add_library(Core_Lib STATIC
						Core.h Core.cpp
						Body.h Body.cpp
						ForceGen.h ForceGen.cpp
						Geometry.h Geometry.cpp
						Links.h Links.cpp
						particle.h particle.cpp
						pfgen.h pfgen.cpp
						pcontact.h pcontact.cpp
						plinks.h plinks.cpp
						Polygon.h Polygon.cpp
						precision.h
						pworld.h pworld.cpp
						Random.h Random.cpp
						Wind.h
						world.h world.cpp)

target_link_libraries(Wind PRIVATE Core_Lib)