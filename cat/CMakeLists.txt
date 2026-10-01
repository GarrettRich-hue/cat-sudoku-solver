add_library(catlib CatSolver.cpp)

target_include_directories(catlib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(catlib PUBLIC satlib)
