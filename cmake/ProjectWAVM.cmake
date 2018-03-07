if(ProjectWAVMIncluded)
	return()
endif()

set(ProjectWAVMIncluded TRUE)

include(ExternalProject)

if(MSVC)
	set(build_command BUILD_COMMAND cmake --build <BINARY_DIR> --config Release)
	set(install_command INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install)
endif()

set(prefix ${CMAKE_BINARY_DIR}/deps)
set(source_dir ${CMAKE_BINARY_DIR}/src/WAVM)
set(binary_dir ${CMAKE_BINARY_DIR}/src/WAVM-build)

set(wavm_include_dir ${source_dir}/Include)

set(wavm_binaries
	${binary_dir}/bin/Assemble
	${binary_dir}/bin/Disassemble
	${binary_dir}/bin/Test
	${binary_dir}/bin/wavm
)
set(wavm_libraries
	${binary_dir}/Source/Platform/${CMAKE_SHARED_LIBRARY_PREFIX}Platform${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/Logging/${CMAKE_SHARED_LIBRARY_PREFIX}Logging${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/IR/${CMAKE_SHARED_LIBRARY_PREFIX}IR${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/WASM/${CMAKE_SHARED_LIBRARY_PREFIX}WASM${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/WAST/${CMAKE_SHARED_LIBRARY_PREFIX}WAST${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/Runtime/${CMAKE_SHARED_LIBRARY_PREFIX}Runtime${CMAKE_SHARED_LIBRARY_SUFFIX}
	${binary_dir}/Source/Emscripten${CMAKE_SHARED_LIBRARY_PREFIX}Emscripten${CMAKE_SHARED_LIBRARY_SUFFIX}
)

ExternalProject_Add(WAVM
	PREFIX ${prefix}
	GIT_REPOSITORY https://github.com/AndrewScheidecker/WAVM.git
	GIT_TAG master
	SOURCE_DIR ${source_dir}
	BINARY_DIR ${binary_dir}
	CMAKE_ARGS
	 -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	 -DCMAKE_BUILD_TYPE=RELEASE
	${build_command}
	BUILD_BYPRODUCTS ${wavm_binaries} ${wavm_libraries}
)

add_library(WAVM::Platform SHARED IMPORTED)
add_library(WAVM::Logging SHARED IMPORTED)
add_library(WAVM::IR SHARED IMPORTED)
add_library(WAVM::WASM SHARED IMPORTED)
add_library(WAVM::WAST SHARED IMPORTED)
add_library(WAVM::Runtime SHARED IMPORTED)
add_library(WAVM::Emscripten SHARED IMPORTED)

set_target_properties(
	WAVM::Platform
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/Platform/${CMAKE_SHARED_LIBRARY_PREFIX}Platform${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::Logging
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/Logging/${CMAKE_SHARED_LIBRARY_PREFIX}Logging${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::IR
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/IR/${CMAKE_SHARED_LIBRARY_PREFIX}IR${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::WASM
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/WASM/${CMAKE_SHARED_LIBRARY_PREFIX}WASM${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::WAST
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/WAST/${CMAKE_SHARED_LIBRARY_PREFIX}WAST${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::Runtime
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/Runtime/${CMAKE_SHARED_LIBRARY_PREFIX}Runtime${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)
set_target_properties(
	WAVM::Emscripten
	PROPERTIES
	IMPORTED_CONFIGURATIONS Release
	IMPORTED_LOCATION_RELEASE ${binary_dir}/Source/Emscripten/${CMAKE_SHARED_LIBRARY_PREFIX}Emscripten${CMAKE_SHARED_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${wavm_include_dir}
)

add_dependencies(WAVM::Platform Platform)
add_dependencies(WAVM::Logging Logging)
add_dependencies(WAVM::IR IR)
add_dependencies(WAVM::WASM WASM)
add_dependencies(WAVM::WAST WAST)
add_dependencies(WAVM::Runtime Runtime)
add_dependencies(WAVM::Emscripten Emscripten)
