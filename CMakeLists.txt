# Invocations
#   cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#   cmake -DCMAKE_BUILD_TYPE=Release ..

# TODO Poke at this
cmake_minimum_required(VERSION 3.14)
project(treebird
  VERSION 0.7
  DESCRIPTION "A very lightweight and beautiful Pleroma frontend"
  LANGUAGES C)

# Import local build if it exists.
# This is a current solution unless distributions provide their own packages
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/mastodont-c")
  include(mastodont-c/cmake/mastodont-config.cmake)
else()
  find_package(MASTODONT REQUIRED)
endif()

add_executable(treebird ${SRC_FILES})

# Useful
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/os_hacks.cmake)
# List helper
include(cmake/append_and_def.cmake)
find_package(PkgConfig REQUIRED)
# Link stuff
find_package(CURL REQUIRED) # Required for mastodont-c (hack)

# For some reason the arguments from Perl are stored as a full string
# Blame CMake(?), not me...
find_package(PerlLibs REQUIRED)

# It returns in CMake terms, "the command", but we want ("the" "command").
#   `target_compile_options' wants this.
# I'm still learning CMake, don't know why it stores lists like this, but it's
#   whatever
string(REPLACE " " ";" _PERL_EXTRA_C_FLAGS ${PERL_EXTRA_C_FLAGS})
# We could also use this somehow:
#    separate_arguments(_PERL_EXTRA_C_FLAGS UNIX_COMMAND
#      "${PERL_EXTRA_C_FLAGS}")

# Useful for GNU Emacs or VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup compile options
target_compile_options(treebird PUBLIC
  -Wall -Wextra -std=c99
  -Wshadow -Wcast-align -Wstrict-prototypes
  ${_PERL_EXTRA_C_FLAGS}
  $<$<CONFIG:DEBUG>:-Og>
  $<$<CONFIG:DEBUG>:-g>
  $<$<CONFIG:RELEASE>:-O2>
)

# hack is on me
target_link_options(treebird PUBLIC
  # -lmastodont
)


# Read CMakeLists.txt in test file
#add_subdirectory(test)

set(SRC_FILES
  src/main.c
  src/types.c
  src/file-to-c/main.c
  src/cookie.c
  src/easprintf.c
  src/error.c
  src/memory.c
  src/helpers.c
  src/key.c
  src/local_config.c
  src/local_config_set.c
  src/query.c
  src/string.c
  src/string_helpers.c
  src/request.c
  src/session.c
  src/path.c
  src/mime.c
)

# Pages
append_and_def(treebird PUBLIC ${SRC_FILES}
  src/about.c
  src/account.c
  src/applications.c
  src/attachments.c
  src/base_page.c
  src/conversations.c
  src/emoji.c
  src/emoji_reaction.c
  src/global_cache.c
  src/global_perl.c
  src/hashtag.c
  src/http.c
  src/index.c
  src/lists.c
  # src/login.c
  # src/memory_page.c
  # src/notifications.c
  # src/page_config.c
  # src/scrobble.c
  # src/search.c
  # src/status.c
  # src/timeline.c
)

target_sources(treebird PUBLIC ${SRC_FILES})

target_include_directories(treebird PUBLIC
  ${PERL_INCLUDE_DIRS}
  ${MASTODONT_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS})

# Build! Note the mastodont hack, this _should_ change soon.
target_link_libraries(treebird PUBLIC
  ${PERL_LIBRARIES}
  ${CURL_LIBRARIES}

  ${MASTODONT_LIBRARIES})
