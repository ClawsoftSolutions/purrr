cmake_minimum_required(VERSION 3.14)

project(purrr)

find_package(Vulkan REQUIRED)

if (NOT TARGET glfw)
  add_subdirectory(deps/glfw)
endif()

file(GLOB_RECURSE SOURCES "src/**.c" "include/**.h")
add_library(purrr STATIC ${SOURCES})
target_link_libraries(purrr glfw Vulkan::Vulkan)
target_include_directories(purrr PUBLIC include/)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(example/)
endif()