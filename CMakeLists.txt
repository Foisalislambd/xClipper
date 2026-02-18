cmake_minimum_required(VERSION 3.16)

project(LixClipper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt (Try Qt6 first, then Qt5)
find_package(Qt6 COMPONENTS Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/ClipboardManager.cpp
    src/ClipboardManager.h
    src/HistoryWindow.cpp
    src/HistoryWindow.h
)

add_executable(LixClipper ${SOURCES})

# Link libraries
if (Qt6_FOUND)
    target_link_libraries(LixClipper PRIVATE Qt6::Widgets)
else()
    target_link_libraries(LixClipper PRIVATE Qt5::Widgets)
endif()
