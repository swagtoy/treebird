# Invocations
#   cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#   cmake -DCMAKE_BUILD_TYPE=Release ..

# TODO Poke at this
cmake_minimum_required(VERSION 3.14)
project(treebird
  VERSION 0.7
  DESCRIPTION "A very lightweight and beautiful (subjective) Pleroma frontend"
  LANGUAGES C)

include(cmake/os_hacks.cmake)
# List helper
include(cmake/append_and_def.cmake)
find_package(PkgConfig REQUIRED)
# Link stuff
pkg_check_modules(CURL libcurl REQUIRED)
find_package(PerlLibs REQUIRED)

# Useful for GNU Emacs or VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup compile options
add_compile_options(
  -Wall -Wextra -std=c99
  -Wshadow -Wcast-align -Wstrict-prototypes
  ${PERL_EXTRA_C_FLAGS}
  $<$<CONFIG:DEBUG>:-Og>
  $<$<CONFIG:DEBUG>:-g>
  $<$<CONFIG:RELEASE>:-O2>
)

# hack is on me
add_link_options(
  -lmastodont
  ${CURL_LDFLAGS_OTHER}
)

include_directories(include)

# Read CMakeLists.txt in test file
#add_subdirectory(test)

set(SRC_FILES
  src/file-to-c/main.c
  )

append_and_def(SRC_FILES
  src/about.c
  src/account.c
  src/applications.c
  src/attachments.c
  src/base_page.c
  src/conversations.c
  src/cookie.c
  src/easprintf.c
  src/emoji.c
  src/emoji_reaction.c
  src/error.c
  src/global_cache.c
  src/global_perl.c
  src/hashtag.c
  src/helpers.c
  src/http.c
  src/index.c
  src/key.c
  src/lists.c
  src/local_config.c
  src/local_config_set.c
  src/login.c
  src/main.c
  src/memory.c
  src/memory_page.c
  src/mime.c
  src/notifications.c
  src/page_config.c
  src/path.c
  src/query.c
  src/request.c
  src/scrobble.c
  src/search.c
  src/session.c
  src/status.c
  src/string.c
  src/string_helpers.c
  src/timeline.c
  src/types.c
)

# Compile with X11
if(X11_FOUND)
  set(SRC_FILES ${SRC_FILES}
    src/x11/treebird_x11_context.c
    src/x11/treebird_x11_window.c
  )
  add_definitions(-DTREEBIRD_USE_X11_BACKEND)
endif(X11_FOUND)

add_executable(treebird ${SRC_FILES})
target_include_directories(treebird PUBLIC
  ${PERL_INCLUDE_PATH}
  ${CURL_INCLUDE_PATH})

# Build! Note the mastodont hack, this _should_ change soon.
target_link_libraries(treebird PUBLIC
  ${PERL_LIBRARIES}
  ${CURL_LIBRARIES}
  mastodont-c/libmastodont.a)
