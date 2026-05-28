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
        include/Physics/Space/SpaceObject.h
        include/Physics/Space/CelestialBody.h
        include/Physics/Space/Spacecraft.h
        include/Physics/Space/SpaceSimulation.h
        include/Physics/Space/MapReduceEngine.h
        ui/SimulationSpaceGame.h
        include/Physics/Space/AstronomySimulation.h
        ui/SimulationAstronomy.h
        include/Common/Observation.h
        include/Physics/MassSpringSystem.h
        ui/SimulationMassSpring.h
        include/Common/Particle.h
        include/Common/RandomGenerator.h
        include/Physics/BrownianMotion.h
        ui/SimulationBrownianMotion.h
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
        tests/test_space_physics.cpp
        "tests/test_astronomy_simulation.cpp"
        tests/test_mass_spring_simulation.cpp
        tests/test_brownian_motion.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests gtest gtest_main)

add_test(NAME Lab5Tests COMMAND tests)

function(setup_windows_runtime_dlls target_name)
    if(WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${target_name}>
                $<TARGET_FILE_DIR:${target_name}>
                COMMAND_EXPAND_LISTS
        )
    endif()
endfunction()

setup_windows_runtime_dlls(lab5)
setup_windows_runtime_dlls(tests)