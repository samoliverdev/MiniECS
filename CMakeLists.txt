cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

#if(MSVC)
    add_compile_options(/permissive- /Zc:preprocessor /Zc:__cplusplus)
#endif()

#----------------Taskflow------------------
include(FetchContent)
FetchContent_Declare(
    taskflow
    GIT_REPOSITORY https://github.com/taskflow/taskflow.git
    GIT_TAG v3.7.0 
)
FetchContent_MakeAvailable(taskflow)

#----------------ECS------------------
project(ECS)
file(GLOB_RECURSE ECS_SOURCE_FILES 
    ECS/Source/*.cpp
)
option(ECS_BUILD_SHARED "Build ECS as a shared library" ON)
option(ECS_USE_DLL_SAFE_COMPONENT_IDS "Use exported component ID fields across DLL boundaries" OFF)
option(ECS_TEST_GET_ID "Use cached type-name IDs across DLL boundaries" ON)
if(ECS_BUILD_SHARED)
    add_library(ECS SHARED ${ECS_SOURCE_FILES})
    target_compile_definitions(ECS PRIVATE ECS_BUILD_DLL)
    target_compile_definitions(ECS INTERFACE ECS_USE_DLL)
else()
    add_library(ECS STATIC ${ECS_SOURCE_FILES})
endif()
if(ECS_TEST_GET_ID)
    target_compile_definitions(ECS PUBLIC TestGetId)
elseif(ECS_USE_DLL_SAFE_COMPONENT_IDS)
    target_compile_definitions(ECS PUBLIC UseDLLSafe)
endif()

target_include_directories(ECS PUBLIC ECS/Source/ ${taskflow_SOURCE_DIR}/)

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

target_include_directories(Benchmarks PRIVATE ${taskflow_SOURCE_DIR}/)

endif()

#----------------Sample------------------
project(Sample)
set(Sample_SOURCE_FILES Sample/Source/Main.cpp)
add_executable(Sample ${Sample_SOURCE_FILES})
target_include_directories(Sample PUBLIC ECS/Source/)
target_link_libraries(Sample ECS)

target_include_directories(Sample PRIVATE ${taskflow_SOURCE_DIR}/)

if(ECS_BUILD_SHARED AND (ECS_USE_DLL_SAFE_COMPONENT_IDS OR ECS_TEST_GET_ID))
    add_library(SampleEngine SHARED Sample/Source/DllSampleEngine.cpp)
    target_include_directories(SampleEngine PUBLIC Sample/Source/ ECS/Source/)
    target_compile_definitions(SampleEngine PRIVATE SAMPLE_ENGINE_BUILD_DLL)
    target_link_libraries(SampleEngine PUBLIC ECS)

    add_executable(DllComponentSample Sample/Source/DllSampleMain.cpp)
    target_link_libraries(DllComponentSample PRIVATE SampleEngine ECS)
endif()

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

target_include_directories(Tests PRIVATE ${taskflow_SOURCE_DIR}/)

endif()
