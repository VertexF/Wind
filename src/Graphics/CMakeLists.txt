add_library(Graphics_Lib STATIC
						include/ColourRGBA.h
						include/FontRect.h
						include/TexCoord.h
						include/TextureVertex2D.h
						include/VerPos2D.h
						Font.h Font.cpp
						Mesh.h Mesh.cpp
						OBJLoader.h OBJLoader.cpp
						ShaderProgram.h ShaderProgram.cpp
						ShaderProgram3D.h ShaderProgram3D.cpp
						ShaderProgram2D.h ShaderProgram2D.cpp
						SpriteSheet.h SpriteSheet.cpp
						Texture.h Texture.cpp)

target_link_libraries(Wind PUBLIC Graphics_Lib)