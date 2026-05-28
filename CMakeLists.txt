cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_VERSION_MINIMUM "3.5")

project(lab5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
)

set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOC OFF CACHE INTERNAL "")
set(SFML_BUILD_TESTS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(sfml)

add_subdirectory(third_party/googletest)
enable_testing()

add_executable(lab5
        src/main.cpp
        include/Sequences/ListSequence.h
        include/Base/BinarySearchTree.h
        include/Base/DynamicArray.h
        include/Common/Option.h
        include/Common/Pair.h
        include/Sequences/Algorithms.h
        include/Sequences/ArraySequenceBuilder.h
        include/Sequences/SequenceEnumerator.h
        include/Sequences/SequenceFunctions.h
        include/Common/Vector3D.h
        include/Physics/SpaceObject.h
        include/Physics/CelestialBody.h
        include/Physics/Spacecraft.h
        include/Physics/SpaceSimulation.h
        include/Physics/MapReduceEngine.h
        ui/SpaceGameUI.h
)

target_include_directories(lab5 PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Base
        ${CMAKE_SOURCE_DIR}/include/Common
        ${CMAKE_SOURCE_DIR}/include/Sequences
        ${CMAKE_SOURCE_DIR}/include/Specialized
)

target_link_libraries(lab5 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)

add_executable(tests
        tests/test_dynamic_array.cpp
        tests/test_linked_list.cpp
        tests/test_sequence.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests gtest gtest_main)

add_test(NAME Lab5Tests COMMAND tests)

if(WIN32)
    add_custom_command(TARGET lab5 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:lab5>
            $<TARGET_FILE_DIR:lab5>
            COMMAND_EXPAND_LISTS
    )
endif()