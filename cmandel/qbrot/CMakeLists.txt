
cmake_minimum_required (VERSION 2.6)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR}/qbrot)

set (HEADERS mainwindow.h
             mandelbrotshower.h)
set (SOURCES main.cpp
             mainwindow.cpp
             mandelbrotshower.cpp)
set (FORMS   mainwindow.ui)

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UI_SOURCES ${FORMS})
    add_executable (qbrot ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries (qbrot mandel ${QT_LIBRARIES})

install (TARGETS qbrot DESTINATION bin)
