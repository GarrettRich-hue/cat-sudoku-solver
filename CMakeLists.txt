cmake_minimum_required(VERSION 4.1)
project(catsudokusolver)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE satlib)
add_subdirectory(sat)

target_link_libraries(${PROJECT_NAME} PRIVATE catlib)
add_subdirectory(cat)
