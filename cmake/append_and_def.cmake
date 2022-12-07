# For ease of development, you can enable and disable pages. This is useful if
# the code doesn't compile correctly, for which you can do an `#ifdef' with the
# C Preprocessor

macro(append_and_def TARGET SCOPE NAME ...)
  
  foreach(X IN ITEMS ${ARGN})
    # Convert to uppercase for the definition
    get_filename_component(TEMP ${X} NAME_WE)
    string(TOUPPER _TEMP ${TEMP})

    get_target_property(
      targ_comp
      ${TARGET}
      INTERFACE_COMPILE_DEFINITIONS)
    message(${_TEMP})
    target_compile_definitions(${TARGET} ${SCOPE}
      ${targ_comp} -DCMP_ENABLE_${_TEMP})
    list(APPEND X ${_TEMP})
  endforeach()
endmacro()
