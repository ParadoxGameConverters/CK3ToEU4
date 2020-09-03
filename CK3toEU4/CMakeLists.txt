cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE on)

project(CK2ToEU4)
#SET(CMAKE_CXX_COMPILER g++-9)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Source)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_compile_options("-lstdc++fs")
add_compile_options("-O2")
add_compile_options("-pthread")
add_link_options(-no-pie)

include_directories("../commonItems")
include_directories("../ZipLib")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(../ZipLib [binary_dir])

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CONVERTER_VERSION_SOURCES "${PROJECT_SOURCE_DIR}/ConverterVersion/*.cpp")
file(GLOB CK3WORLD_SOURCES "${PROJECT_SOURCE_DIR}/CK3World/*.cpp")
set(COMMON_SOURCES "../commonItems/CommonFunctions.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Color.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/GameVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Date.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/LinuxUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/OSCommonLayer.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/StringUtils.cpp")

add_executable(CK3toEU4 
	${MAIN_SOURCES}
	${CONVERTER_VERSION_SOURCES}
	${COMMON_SOURCES}
	${CK3WORLD_SOURCES}
)

target_link_libraries(CK3toEU4 LINK_PUBLIC ZIPLIB stdc++fs)

add_custom_command(TARGET CK3toEU4 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND chmod u+x Copy_Files.sh)
add_custom_command(TARGET CK3toEU4 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ./Copy_Files.sh)
