cmake_minimum_required(VERSION 3.22)

# Define project name, version, and C++ language requirement
project(MidiEventSystem VERSION 1.0.0 LANGUAGES C CXX)

# Enforce C++17 standard for modern template and memory features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include JUCE from the repository root
# Alternatively, if JUCE is installed globally, use: find_package(JUCE CONFIG REQUIRED)
add_subdirectory(../JUCE ${CMAKE_BINARY_DIR}/JUCE)

# Create a console application target using JUCE's CMake helper
juce_add_console_app(MidiEventSystem
    PRODUCT_NAME "MidiEventSystem"
)

# Specify the source files for the target
target_sources(MidiEventSystem PRIVATE
    src/Main.cpp
)

# Disable unnecessary external dependencies for a lightweight console build
target_compile_definitions(MidiEventSystem PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_STANDALONE_APPLICATION=1
)

# Link strictly the core JUCE module required for juce::String and juce::Array
target_link_libraries(MidiEventSystem PRIVATE
    juce::juce_core
)