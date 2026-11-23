cmake_minimum_required(VERSION 3.22)
project(LabExercise4 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(JUCE CONFIG REQUIRED)

juce_add_console_app(LabExercise4
    PRODUCT_NAME "LabExercise4"
    NEEDS_CURL FALSE
    NEEDS_WEB_BROWSER FALSE)

target_sources(LabExercise4
    PRIVATE
        src/Main.cpp)

target_compile_definitions(LabExercise4
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LabExercise4,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LabExercise4,JUCE_VERSION>")

target_link_libraries(LabExercise4
    PRIVATE
        juce::juce_audio_formats
        juce::juce_audio_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)