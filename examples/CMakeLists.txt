add_subdirectory(assets)

include_directories(./)

set(TARGETS "")
list(APPEND TARGETS CHP)

foreach(target ${TARGETS})
  file(GLOB_RECURSE target_sources ${target}/**.c ${target}/**.cpp)
  add_executable(${target} ${target_sources})
  target_link_libraries(${target} purrr)
  target_include_directories(${target} PRIVATE ${target}/)
  set_property(TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()