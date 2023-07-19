cmake_minimum_required(VERSION 3.25)
project(TODO)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(include)
include_directories(${GTKMM_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/main.cpp
        include/sqlite.h src/sqlite.cpp
        include/gui.h src/gui.cpp)
target_link_libraries(${PROJECT_NAME} sqlite3 ${GTKMM_LIBRARIES})
