# Invocations
#   cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#   cmake -DCMAKE_BUILD_TYPE=Release ..

cmake_minimum_required(VERSION 3.14)
project(treebird
  VERSION 0.7
  DESCRIPTION "A very lightweight and beautiful (subjective) Pleroma frontend"
  LANGUAGES C)

include(cmake/helpers.cmake)
find_package(FCGI REQUIRED)
find_package(PERL REQUIRED)

# Useful for GNU Emacs or VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup compile options
add_compile_options(
  -Wall -Wextra -std=c99
  -Wshadow -Wcast-align -Wstrict-prototypes
  $<$<CONFIG:DEBUG>:-Og>
  $<$<CONFIG:DEBUG>:-g>
  $<$<CONFIG:RELEASE>:-O2>
  )

include_directories(include)

# Read CMakeLists.txt in test file
add_subdirectory(test)

set_and_def(SRC_FILES
  
)

# Compile with X11
if(X11_FOUND)
  set(SRC_FILES ${SRC_FILES}
    src/x11/treebird_x11_context.c
    src/x11/treebird_x11_window.c
  )
  add_definitions(-DTREEBIRD_USE_X11_BACKEND)
endif(X11_FOUND)

add_executable(treebird SHARED ${SRC_FILES})
target_include_directories(treebird PUBLIC ${X11_xcb_INCLUDE_PATH})
# Build! Note the mastodont hack, this _should_ change soon.
target_link_libraries(treebird PUBLIC ${_LIB} mastodont-c/libmastodont.a)
