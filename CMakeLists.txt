# Copyright 2021 Saroj Kumar.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 90)

project(ShroonLogger)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Target CPU: ${CMAKE_SYSTEM_PROCESSOR}")

set(ShroonIncludeDir "${CMAKE_CURRENT_LIST_DIR}/include")

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.h")

file(MAKE_DIRECTORY "${ShroonIncludeDir}/Shroon/Logger/")
file(COPY ${HEADER_FILES} DESTINATION "${ShroonIncludeDir}/Shroon/Logger/")

add_library(ShroonLogger INTERFACE)

target_include_directories(ShroonLogger INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/src/"
    "${CMAKE_CURRENT_LIST_DIR}/external/ShroonUtils/include/"
)

install(
    TARGETS ShroonLogger EXPORT ShroonLoggerTargets
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/ShroonLoggerConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

export(
    EXPORT ShroonLoggerTargets
    NAMESPACE ShroonLogger::
    FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/ShroonLoggerTargets.cmake"
)

if (${SLOG_BUILD_EXAMPLE})
    message(STATUS "Build example: ON")

    add_executable(Example "example/main.c")

    set_target_properties(Example PROPERTIES
        VERSION 1.0.0
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/example/"
    )

    target_include_directories(Example PUBLIC "${ShroonIncludeDir}")
    target_link_libraries(Example PUBLIC ShroonLogger m)
else()
    message(STATUS "Build example: OFF")
endif()

if (${SLOG_BUILD_DOCS})
    message(STATUS "Build docs: ON")

    add_custom_target(
      docs ALL
      COMMAND "doxygen"
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      COMMENT "Generating API documentation with Doxygen"
    )

else()
    message(STATUS "Build docs: OFF")
endif()

unset(ShroonIncludeDir)
