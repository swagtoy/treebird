# For ease of development, you can enable and disable pages. This is useful if
# the code doesn't compile correctly, for which you can do an `#ifdef' with the
# C Preprocessor

function(append_and_def NAME)
  foreach(X IN ITEMS ARGV)
    list(APPEND NAME X)
  endforeach()
endfunction()
