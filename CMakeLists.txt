cmake_minimum_required(VERSION 4.1)
project(lab5)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/googletest)
enable_testing()

add_executable(lab5 src/main.cpp
        cli/interface.h
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
        include/Physics/MapReduceEngine.h)

include_directories(third_party/googletest/include)

add_executable(tests
        tests/test_dynamic_array.cpp
        tests/test_linked_list.cpp
        tests/test_sequence.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests gtest gtest_main)

add_test(NAME Lab5Tests COMMAND tests)

target_include_directories(lab5 PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Base
        ${CMAKE_SOURCE_DIR}/include/Common
        ${CMAKE_SOURCE_DIR}/include/Sequences
        ${CMAKE_SOURCE_DIR}/include/Specialized
)
