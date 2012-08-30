
cmake_minimum_required(VERSION 2.6)

project (cmandel)


set (LIBRARY_SOURCES mandel.cpp)
set (MAIN_SOURCES main.cpp)
set (LIBRARY_LIBS m)
set (MAIN_LIBS mandel )
set (HEADERS mandel.h)

option (USE_OPENMP "Use OpenMP for parallel computing" ON)
if (USE_OPENMP)
    include (FindOpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else (OPENMP_FOUND)
        message (SEND_ERROR "OpenMP is not supported by your compiler. Disable OpenMP or change compiler")
    endif (OPENMP_FOUND)
endif (USE_OPENMP)

option (STATIC "Build statically to run in-place" ON)

if (STATIC)
    add_library (mandel STATIC ${LIBRARY_SOURCES})
else (STATIC)
    add_library (mandel SHARED ${LIBRARY_SOURCES})
    install (TARGETS mandel DESTINATION lib${LIB_SUFFIX})
endif (STATIC)

target_link_libraries (mandel ${LIBRARY_LIBS})
install (FILES ${HEADERS} DESTINATION include)

option (BUILD_CLI "Build command-line interface" ON)
if (BUILD_CLI)
    message (STATUS "Looking for GD")
    find_program ( GD gdlib-config )
    if ( ${GD} STREQUAL "GD-NOTFOUND" )
        message (SEND_ERROR "GD library required for command-line tool is not found")
    endif ( ${GD} STREQUAL "GD-NOTFOUND" )
    execute_process ( COMMAND gdlib-config --features OUTPUT_VARIABLE GD_FEATURES )
    if ( NOT ${GD_FEATURES} MATCHES ".+GD_PNG.+"  )
        message (SEND_ERROR "GD library does not support PNG. Please recompile it with PNG support.")
    endif ( NOT ${GD_FEATURES} MATCHES ".+GD_PNG.+"  )

    set (MAIN_SOURCES ${MAIN_SOURCES} gdinterface.cpp)
    set (MAIN_LIBS ${MAIN_LIBS} gd)
    message (STATUS "Looking for GD - found")

    option (BUILD_NETWORK_MASTER "Build network rendering master node")
    if (BUILD_NETWORK_MASTER)
        set (MAIN_SOURCES ${MAIN_SOURCES} server.cpp)
    endif (BUILD_NETWORK_MASTER)

    add_executable (mandelbrot ${MAIN_SOURCES})
    target_link_libraries (mandelbrot ${MAIN_LIBS})
    install (TARGETS mandelbrot DESTINATION bin)
endif (BUILD_CLI)

option (BUILD_GUI "Build Qt-based mandelbrot viewer")
if (BUILD_GUI)
    add_subdirectory (qbrot)
endif (BUILD_GUI)

option (BUILD_NETWORK_NODE "Build network rendering worker node")
if (BUILD_NETWORK_NODE)
    set (NODE_SOURCES node.cpp)
    set (NODE_LIBS mandel)
    add_executable (mandelbrot_node ${NODE_SOURCES})
    target_link_libraries (mandelbrot_node ${NODE_LIBS})
    install (TARGETS mandelbrot_node DESTINATION bin)
endif (BUILD_NETWORK_NODE)

configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)
include_directories ( "${PROJECT_BINARY_DIR}" )
