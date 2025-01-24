cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(
  "Itoa-Performance"
  VERSION "${PRODUCT_VERSION}"
  LANGUAGES CXX
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install/")

file(GLOB_RECURSE HEADER_01 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE HEADER_02 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE HEADER_03 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/fmt/*.h")
file(GLOB_RECURSE HEADER_04 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/msinttypes/*.h")
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(
  "Itoa-Performance" 
  ${SOURCE}
  ${HEADERS_01}
  ${HEADERS_02}
  ${HEADERS_03}
  ${HEADERS_04}
  "./src/main.cpp"
)

target_compile_options(
	"Itoa-Performance" PUBLIC
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:/fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wpedantic>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wpedantic>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wextra>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wall>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Wall>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

target_link_options(
	"Itoa-Performance" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
)

target_compile_definitions(
	"Itoa-Performance" PUBLIC 
	"BASE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../\""
	"JSONIFIER_COMMIT=\"${JSONIFIER_COMMIT_HASH}\""
	"SIMDJSON_COMMIT=\"${SIMDJSON_COMMIT_HASH}\""
	"GLAZE_COMMIT=\"${GLAZE_COMMIT_HASH}\""
	"OPERATING_SYSTEM_NAME=\"${CMAKE_SYSTEM_NAME}\""
	"OPERATING_SYSTEM_VERSION=\"${CMAKE_SYSTEM_VERSION}\""
	"COMPILER_ID=\"${CMAKE_CXX_COMPILER_ID}\""
	"COMPILER_VERSION=\"${CMAKE_CXX_COMPILER_VERSION}\""
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:Itoa-Performance>"
		DESTINATION "bin"
		OPTIONAL
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:Itoa-Performance>"
	DESTINATION "bin"
	OPTIONAL
)
