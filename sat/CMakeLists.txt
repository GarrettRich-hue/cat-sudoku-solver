add_library(satlib CNF.cpp Solver.cpp Formula.cpp)

target_include_directories(satlib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})
