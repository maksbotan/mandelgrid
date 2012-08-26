
cmake_minimum_required(VERSION 2.6)

project (cmandel)

include (CheckSymbolExists)

set (LIBRARY_SOURCES mandel.cpp)
set (MAIN_SOURCES main.cpp)
set (LIBRARY_LIBS m)
set (MAIN_LIBS mandel )
set (HEADERS mandel.h)

option (ENABLE_GD "Enable png output with libgd")

if (ENABLE_GD)
    message (STATUS "Looking for GD")
    find_program ( GD gdlib-config )
    if ( ${GD} STREQUAL "GD-NOTFOUND" )
        message (SEND_ERROR "GD library not found, but support for it requested. Please install libgd.")
    endif ( ${GD} STREQUAL "GD-NOTFOUND" )
    execute_process ( COMMAND gdlib-config --features OUTPUT_VARIABLE GD_FEATURES )
    if ( NOT ${GD_FEATURES} MATCHES ".+GD_PNG.+"  )
        message (SEND_ERROR "GD library does not support PNG. Please recompile it with PNG support.")
    endif ( NOT ${GD_FEATURES} MATCHES ".+GD_PNG.+"  )

    set (LIBRARY_SOURCES ${LIBRARY_SOURCES} gdinterface.cpp)
    set (LIBRARY_LIBS ${LIBRARY_LIBS} gd)
    set (HEADERS ${HEADERS} gdinterface.h)
    set (HAVE_GD 1)
    message (STATUS "Looking for GD - found")
endif (ENABLE_GD)

CHECK_SYMBOL_EXISTS (clock "time.h" HAVE_CLOCK)

configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)
include_directories ( "${PROJECT_BINARY_DIR}" )

option (STATIC "Build statically to run in-place" ON)

if (STATIC)
    add_library (mandel STATIC ${LIBRARY_SOURCES})
else (STATIC)
    add_library (mandel SHARED ${LIBRARY_SOURCES})
endif (STATIC)

target_link_libraries (mandel ${LIBRARY_LIBS})

add_executable (mandelbrot ${MAIN_SOURCES})
target_link_libraries (mandelbrot ${MAIN_LIBS})

install (TARGETS mandelbrot DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
if (NOT STATIC)
    install (TARGETS mandel DESTINATION lib${LIB_SUFFIX})
endif (NOT STATIC)
