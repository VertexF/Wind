add_library(Application_Lib STATIC
						 Application.h Application.cpp
						 Camera.h Camera.cpp
						 Game.h Game.cpp
						 Timer.h Timer.h)
						 
target_link_libraries(Wind PUBLIC Application_Lib)