cmake_minimum_required(VERSION 3.20)

project(CppDiplomProject)

add_compile_definitions(_WIN32_WINNT=0x0A01)

add_subdirectory(Spider)

#add_subdirectory(HttpServer)

#add_subdirectory(ConfigParser)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/Config
	)