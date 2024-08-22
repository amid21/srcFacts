# @file CMakeLists.txt
# 
# CMake files for the srcFacts project

# Can be run by any version of CMake >= this
cmake_minimum_required(VERSION 3.20)

# project info
project(srcFacts)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Use Release build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Source files for the main program srcFacts
set(SOURCE srcFacts.cpp refillBuffer.cpp XMLParser.cpp)

# srcFact application
add_executable(srcFacts ${SOURCE})

# cmake .. -DTRACE=
if(TRACE)
    message("TRACE is ${TRACE}")
    target_compile_definitions(srcFacts PUBLIC TRACE)
endif()

# Turn on warnings
if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # standard warnings
    add_compile_options(-Wall)
endif()

# Extract the demo input srcML file
file(ARCHIVE_EXTRACT INPUT ${CMAKE_SOURCE_DIR}/demo.xml.zip)

# Demo run command
add_custom_target(run
        COMMENT "Run demo"
        COMMAND $<TARGET_FILE:srcFacts> < demo.xml
        DEPENDS srcFacts
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Source files for xmlstats
set(XMLSTATS_SOURCE xmlstats.cpp XMLParser.cpp refillBuffer.cpp xml_parser.cpp)

# xmlstats application
add_executable(xmlstats ${XMLSTATS_SOURCE})

# xmlstats run command
add_custom_target(runxmlstats
        COMMENT "Run xmlstats"
        COMMAND $<TARGET_FILE:xmlstats> < demo.xml
        DEPENDS xmlstats
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Source files for identity
set(XMLSTATS_SOURCE identity.cpp XMLParser.cpp refillBuffer.cpp xml_parser.cpp)

# identity application
add_executable(identity ${XMLSTATS_SOURCE})

# identity run command
add_custom_target(runidentity
        COMMENT "Run identity"
        COMMAND $<TARGET_FILE:identity> < demo.xml > democopy.xml
        DEPENDS identity
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
