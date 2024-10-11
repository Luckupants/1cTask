cmake_minimum_required(VERSION 3.9)

project(suggestor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(suggestor main.cpp trie.hpp)