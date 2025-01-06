add_subdirectory(shaders)

add_custom_target(copy ALL)

file(GLOB_RECURSE copy_children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/**)

foreach(file ${copy_children})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    add_custom_command(
      TARGET copy PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E
        make_directory ${CMAKE_CURRENT_BINARY_DIR}/${file}
    )
  else()
    get_filename_component(filename ${file} NAME)
    string(COMPARE NOTEQUAL ${filename} "CMakeLists.txt" proceed)
    if (${proceed})
      add_custom_command(
        TARGET copy PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
          copy ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${CMAKE_CURRENT_BINARY_DIR}/${file}
      )
    endif()
  endif()
endforeach()