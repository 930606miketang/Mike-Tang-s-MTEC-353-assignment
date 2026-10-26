cmake_minimum_required(VERSION 3.15)
project(MIDI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(midi LabExercise2/docs/midi.cpp)

if(APPLE)
    add_executable(synth synth.cpp)
    target_link_libraries(synth PRIVATE
        "-framework AudioToolbox"
        "-framework CoreAudio"
    )
endif()