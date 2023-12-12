cmake_minimum_required(VERSION 3.26)
set(CMAKE_VERBOSE_MAKEFILE on)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "/Users/zakerden1234/Downloads/SFML-2.6.1/lib")

include_directories("/Users/zakerden1234/Downloads/SFML-2.6.1/include")

link_directories("/Users/zakerden1234/Downloads/SFML-2.6.1/lib")

add_executable(Minesweeper main.cpp)

target_link_libraries(Minesweeper sfml-graphics sfml-window sfml-system)
