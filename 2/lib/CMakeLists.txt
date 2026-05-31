add_library(model)

target_include_directories(model PRIVATE .)
target_sources(model PRIVATE ./board.cpp ./wire.cpp)


