cmake_minimum_required(VERSION 3.5)
project(Apollo)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

# Compiler Debug info in Release mode
if (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

# install
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${Apollo_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${Apollo_BINARY_DIR}/bin/Release)
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install)
set(INSTALL_DIR ${PROJECT_BINARY_DIR}/install)

# cmake
include(cmake/msvs.cmake) # visual studio
add_definitions("/wd4819 /wd4251 /wd4996")

# OpenMP
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(OPENALROOT ${CMAKE_SOURCE_DIR}/3rdparty/openal)
include_directories(${OPENALROOT}/include)
set(OPENALLIB_DIR ${OPENALROOT}/libs)
link_directories(${OPENALLIB_DIR})
set(OPENAL_LIBS OpenAL32)

add_subdirectory(test)