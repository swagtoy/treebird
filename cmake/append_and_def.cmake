# For ease of development, you can enable and disable pages. This is useful if
# the code doesn't compile correctly, for which you can do an `#ifdef' with the
# C Preprocessor

function(append_and_def TARGET SCOPE NAME)
  math(EXPR LEN "${ARGC}-1")
  foreach(I RANGE 3 ${LEN})
    list(GET ARGV ${I} filename)
    # Convert name partto uppercase for the definition
    get_filename_component(name ${filename} NAME_WE)
    string(TOUPPER ${name} file_up)

    get_target_property(
      targ_comp
      ${TARGET}
      COMPILE_DEFINITIONS
    )

    if ("${targ_comp}" STREQUAL "targ_comp-NOTFOUND")
      set_target_properties(
        ${TARGET}
        PROPERTIES COMPILE_DEFINITIONS "${file_up}"
      )
    else()
      set_target_properties(
        ${TARGET}
        PROPERTIES COMPILE_DEFINITIONS "${targ_comp};${file_up}"
      )
    endif()
    
    # Will return copy of files
    list(APPEND out_list ${filename})
  endforeach()

  # Return
  set(${NAME} ${out_list} PARENT_SCOPE)
endfunction()
