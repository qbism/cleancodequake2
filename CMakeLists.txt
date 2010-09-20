# First cut at a CMake build script. Get the latest version at www.cmake.org
#
# TODO:
# * Figure out how to give it optimization flags

# Specify minimum version of CMake required
cmake_minimum_required (VERSION 2.8)

# Project name
project (CleanCodeQuake2)

# Look for header files in these directories
include_directories (include shared sircl/include)

# Build the sircl library
add_subdirectory (sircl)

# Build and include zlib (on Windows)
if (WIN32)
  include_directories (zlib)
  add_subdirectory (zlib)
endif ()

# Source files
file (GLOB CC_SHARED_FILES shared/*.cpp)
file (GLOB CC_SOURCE_FILES source/*/*.cpp)

# Set the platform-specific library name and use platform-specific files.
if (WIN32)
  set (CC_LIB_NAME gamex86)
  file (GLOB CC_PLATFORM_FILES source/Platform/windows/*)
elseif (UNIX)
  # Get machine hardware name (arch), force 386 (if applicable), strip newlines, and store in ARCH
  execute_process (COMMAND uname -m COMMAND sed s/i.86/i386/ COMMAND tr -d \n OUTPUT_VARIABLE ARCH)
  set (CC_LIB_NAME game${ARCH})
  file (GLOB CC_PLATFORM_FILES source/Platform/unix/*)
else ()
  message (FATAL "Unknown platform")
endif ()

# Specify the source files for the game library
add_library (${CC_LIB_NAME} SHARED ${CC_SHARED_FILES} ${CC_SOURCE_FILES} ${CC_PLATFORM_FILES})

# Specify that the game library links with sircl and zlib
set (CC_LINK_LIBS sircl z)
# Specify additional libraries required on Windows
if (WIN32)
  set (CC_LINK_LIBS ${CC_LINK_LIBS} Wininet Winmm)
endif ()
target_link_libraries (${CC_LIB_NAME} ${CC_LINK_LIBS})

# If host is Unix-like, remove "lib" prefix from the library's file name
if (UNIX)
  add_custom_command (TARGET ${CC_LIB_NAME} POST_BUILD COMMAND mv lib${CC_LIB_NAME}.so ${CC_LIB_NAME}.so)
endif ()

if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -fomit-frame-pointer -fno-strict-aliasing")
endif()
