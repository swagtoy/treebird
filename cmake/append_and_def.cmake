# For ease of development, you can enable and disable pages. This is useful if
# the code doesn't compile correctly, for which you can do an `#ifdef' with the
# C Preprocessor

# ugly btw, just wanted to let you know
function(append_and_def TARGET SCOPE NAME ...)
  set(Y)
  foreach(I RANGE 3 ${ARGC})
    # Convert to uppercase for the definition
    list(GET ARGV ${I} I)
    math(EXPR I "${I} - 1")
    get_filename_component(I ${X} NAME_WE)
    string(TOUPPER _TEMP ${I})

    # get_target_property(
    #   targ_comp
    #   ${TARGET}
    #   INTERFACE_COMPILE_DEFINITIONS)

    # target_compile_definitions(${TARGET} ${SCOPE}
#      ${targ_comp} -DCMP_ENABLE_${_TEMP})
    
    list(APPEND Y ${_TEMP})
  endforeach()

  return(PROPAGATE ${Y} ${SCOPE} ${X})
endfunction()
