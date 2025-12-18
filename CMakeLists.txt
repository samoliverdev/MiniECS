cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

#----------------ECS------------------
project(ECS)
file(GLOB_RECURSE ECS_SOURCE_FILES 
    ECS/Source/*.cpp
)
add_library(ECS STATIC ${ECS_SOURCE_FILES})

#----------------PerformanceTestVsEntt------------------
project(PerformanceTestVsEntt)
file(GLOB_RECURSE PerformanceTestVsEntt_SOURCE_FILES 
    PerformanceTestVsEntt/Source/*.cpp
)
add_executable(PerformanceTestVsEntt ${PerformanceTestVsEntt_SOURCE_FILES})
target_include_directories(PerformanceTestVsEntt PUBLIC ECS/Source/)
target_link_libraries(PerformanceTestVsEntt ECS)

#----------------Tests------------------
if(On) 
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)# For Windows: Prevent overriding the parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

project(Tests)
file(GLOB_RECURSE TESTS_SOURCE_FILES Tests/Source/*.cpp)
add_executable(Tests ${TESTS_SOURCE_FILES})
target_include_directories(Tests PUBLIC ECS/Source/)
target_link_libraries(Tests ECS)
target_link_libraries(Tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(Tests)

endif()