include(GoogleTest)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(
    TestArgParser
    ${PROJECT_SOURCE_DIR}/src/ArgParser.h
    ${PROJECT_SOURCE_DIR}/src/ArgParser.cpp
    ArgParser.test.cpp)
target_link_libraries(TestArgParser GTest::Main)
gtest_discover_tests(TestArgParser)
