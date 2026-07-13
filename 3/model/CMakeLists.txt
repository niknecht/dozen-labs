add_executable(subway)

target_include_directories(classes PRIVATE .)
target_sources(classes PRIVATE ./main.cpp ./command.cpp ./app_mode.cpp)

target_link_libraries(classes model)
