cmake_minimum_required(VERSION 3.7)
#set(CMAKE_VERBOSE_MAKEFILE on)


# Common Config
project(CK3ToEU4)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/CK3ToEU4/)
set(TEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/CK3ToEU4Tests)
set(CONVERTER_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release-Linux)
set(TEST_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release-Tests)

add_compile_options("-std=c++2a")
add_compile_options("-g")

include_directories(commonItems)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(googletest/googletest)
include_directories(googletest/googletest/include)
include_directories(googletest/googlemock)
include_directories(googletest/googlemock/include)


# Converter
set(MAIN_SOURCE ${MAIN_SOURCE} "${PROJECT_SOURCE_DIR}/main.cpp")
set(CONVERTER_SOURCES ${CONVERTER_SOURCES} "${PROJECT_SOURCE_DIR}/CK3ToEU4.cpp")
set(CONVERTER_VERSION_SOURCES ${CONVERTER_VERSION_SOURCES} "${PROJECT_SOURCE_DIR}/ConverterVersion/ConverterVersion.cpp")
set(CONVERTER_VERSION_SOURCES ${CONVERTER_VERSION_SOURCES} "${PROJECT_SOURCE_DIR}/ConverterVersion/outConverterVersion.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Log.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/Parser.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/ParserHelpers.cpp")
set(COMMON_SOURCES ${COMMON_SOURCES} "../commonItems/StringUtils.cpp")

add_executable(
	CK3ToEU4Converter
	${MAIN_SOURCE}
	${CONVERTER_SOURCES}
	${CONVERTER_VERSION_SOURCES}
	${COMMON_SOURCES}
)
set_target_properties( CK3ToEU4Converter
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CONVERTER_OUTPUT_DIRECTORY}
)
file(MAKE_DIRECTORY ${CONVERTER_OUTPUT_DIRECTORY}/DataFiles)
configure_file("DataFiles/version.txt" "${CONVERTER_OUTPUT_DIRECTORY}/DataFiles/version.txt" COPYONLY)


# Tests
set(GMOCK_SOURCES "../googletest/googletest/src/gtest_main.cc")
set(GMOCK_SOURCES ${GMOCK_SOURCES} "../googletest/googletest/src/gtest-all.cc")
set(GMOCK_SOURCES ${GMOCK_SOURCES} "../googletest/googlemock/src/gmock-all.cc")
set(CONVERTER_VERSION_TESTS_SOURCES ${CONVERTER_VERSION_TESTS_SOURCES} "${TEST_SOURCE_DIR}/ConverterVersionTests/ConverterVersionTests.cpp")

add_executable(
	CK3ToEU4ConverterTests
	${CONVERTER_SOURCES}
	${CONVERTER_VERSION_SOURCES}
	${CONVERTER_VERSION_TESTS_SOURCES}
	${COMMON_SOURCES}
	${GMOCK_SOURCES}
)
set_target_properties( CK3ToEU4ConverterTests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}
)

target_link_libraries(CK3ToEU4ConverterTests pthread)