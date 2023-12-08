cmake_minimum_required(VERSION 3.20)

project(CppDiplomProject)



# TODO: Define by your own needs:
set(OPENSSL_ROOT_DIR "C:\\Program Files\\OpenSSL-Win64")

set(BOOST_ROOT "C:\\boost")

set(libpqxx_DIR "C:\\libpqxx\\libpqxx-7.7.4")


# TODO: If you are running on Windows below 10, change this:

add_compile_definitions(_WIN32_WINNT=0x0A01)



# Boost:
find_package(Boost 1.80.0) #REQUIRED COMPONENTS system thread locale)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost")
endif()


# PQXX
set(PostgreSQL_ADDITIONAL_VERSIONS "15")

add_subdirectory(${libpqxx_DIR} libpqxx-build) 


# OpenSSL
find_package(OpenSSL REQUIRED)



add_subdirectory(Spider)

#add_subdirectory(ConfigParser)

#add_subdirectory(http_server)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/Config
	)