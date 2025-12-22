cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

#if(MSVC)
    add_compile_options(/permissive- /Zc:preprocessor /Zc:__cplusplus)
#endif()

#----------------ECS------------------
project(ECS)
file(GLOB_RECURSE ECS_SOURCE_FILES 
    ECS/Source/*.cpp
)
add_library(ECS STATIC ${ECS_SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
    taskflow
    GIT_REPOSITORY https://github.com/taskflow/taskflow.git
    GIT_TAG v3.7.0 
)
FetchContent_MakeAvailable(taskflow)
target_include_directories(ECS PRIVATE ${taskflow_SOURCE_DIR}/)

#----------------Benchmarks------------------
if(On) 
project(Benchmarks)
file(GLOB_RECURSE Benchmarks_SOURCE_FILES 
    Benchmarks/Source/*.cpp
)
add_executable(Benchmarks ${Benchmarks_SOURCE_FILES})
target_include_directories(Benchmarks PUBLIC ECS/Source/)
target_link_libraries(Benchmarks ECS)

include(FetchContent)
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v4.1.4   # ← strongly recommended to pin a version
)
FetchContent_MakeAvailable(flecs)
target_include_directories(Benchmarks PRIVATE ${flecs_SOURCE_DIR}/include)
target_link_libraries(Benchmarks flecs_static)

FetchContent_Declare(
    taskflow
    GIT_REPOSITORY https://github.com/taskflow/taskflow.git
)
FetchContent_MakeAvailable(taskflow)
target_include_directories(Benchmarks PRIVATE ${taskflow_SOURCE_DIR}/)

endif()

#----------------Sample------------------
project(Sample)
file(GLOB_RECURSE Sample_SOURCE_FILES 
    Sample/Source/*.cpp
)
add_executable(Sample ${Sample_SOURCE_FILES})
target_include_directories(Sample PUBLIC ECS/Source/)
target_link_libraries(Sample ECS)

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