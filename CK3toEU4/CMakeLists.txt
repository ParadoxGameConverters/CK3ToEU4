cmake_minimum_required(VERSION 3.5)

project(CK3toEU4)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Source)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBDIR /usr/local/lib)
set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} LIBDIR)
set(LD_RUN_PATH ${LD_RUN_PATH} LIBDIR)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=8")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath -Wl,LIBDIR")

add_compile_options("-lstdc++fs")
add_compile_options("-O2")
add_compile_options("-pthread")
add_link_options(-no-pie)

include_directories("../commonItems")
include_directories("/usr/local/include/ImageMagick-7/")
include_directories("${PROJECT_SOURCE_DIR}")
link_directories(${CMAKE_SOURCE_DIR}/Resources /usr/local/lib)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CONFIGURATION_SOURCES "${PROJECT_SOURCE_DIR}/Configuration/*.cpp")
file(GLOB CK3WORLD_SOURCES "${PROJECT_SOURCE_DIR}/CK3World/*.cpp")
file(GLOB CK3WORLD_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/CK3World/*/*.cpp")
file(GLOB EU4WORLD_SOURCES "${PROJECT_SOURCE_DIR}/EU4World/*.cpp")
file(GLOB EU4WORLD_SUBDIR_SOURCES "${PROJECT_SOURCE_DIR}/EU4World/*/*.cpp")
file(GLOB MAPPERS_SOURCES "${PROJECT_SOURCE_DIR}/Mappers/*/*.cpp")

set(COMMON_SOURCES "../commonItems/CommonFunctions.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/BulkParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Color.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ConvenientParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ConverterVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Date.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/GameVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/LinuxUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModLoader.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/OSCommonLayer.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/StringUtils.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModFilesystem.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModLoader.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ModLoader/ModParser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/external/zip/src/zip.c")

add_executable(CK3toEU4 
	${MAIN_SOURCES}
	${CONFIGURATION_SOURCES}
	${CK3WORLD_SOURCES}
	${CK3WORLD_SUBDIR_SOURCES}
	${EU4WORLD_SOURCES}
	${EU4WORLD_SUBDIR_SOURCES}
	${MAPPERS_SOURCES}
	${COMMON_SOURCES}
)

target_link_libraries(CK3toEU4 LINK_PUBLIC rakaly stdc++fs MagickWand-7.Q8 MagickCore-7.Q8 Magick++-7.Q8)

add_custom_command(TARGET CK3toEU4 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND git rev-parse HEAD > Release-Linux/commit_id.txt)
add_custom_command(TARGET CK3toEU4 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND chmod u+x Copy_Files.sh)
add_custom_command(TARGET CK3toEU4 POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ./Copy_Files.sh)

set_target_properties(CK3toEU4 PROPERTIES OUTPUT_NAME "CK3ToEU4Converter")
