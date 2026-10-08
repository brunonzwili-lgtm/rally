# Emscripten toolchain for SuperTuxKart
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-emscripten.cmake -G Ninja ..
#
# NOTE: no compiler flags are set here. CMake includes its own
# Modules/Platform/Emscripten.cmake at project() time, and that module runs an
# unquoted if(${CMAKE_C_FLAGS} MATCHES MEMORY64) which errors out on any
# non-empty CMAKE_C_FLAGS. The -s* Emscripten settings therefore live in the
# top-level CMakeLists.txt (inside the USE_EMSCRIPTEN block) where they are
# added after project() has run.

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)

# Locate the Emscripten tree for reference by the project file
if(DEFINED ENV{EMSDK})
    set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
elseif(DEFINED ENV{EMSCRIPTEN})
    set(EMSCRIPTEN_ROOT $ENV{EMSCRIPTEN})
else()
    find_program(EMCC emcc)
    if(EMCC)
        get_filename_component(EMSCRIPTEN_ROOT ${EMCC} DIRECTORY)
    else()
        message(FATAL_ERROR "Emscripten not found. Set EMSDK or EMSCRIPTEN env var, or install emcc in PATH.")
    endif()
endif()
message(STATUS "Emscripten root: ${EMSCRIPTEN_ROOT}")

find_program(STK_WEB_NINJA ninja)
if(STK_WEB_NINJA)
    set(CMAKE_MAKE_PROGRAM "${STK_WEB_NINJA}" CACHE FILEPATH "Build tool")
endif()

# Output layout
set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/web")