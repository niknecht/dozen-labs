include(GoogleTest)
find_package(GTest)

add_executable(tests)

target_sources(tests PRIVATE ./wire_test.cpp ./board_test.cpp)
target_include_directories(tests PRIVATE .)

gtest_discover_tests(tests)

target_link_libraries(tests PRIVATE model GTest::gmock_main)
