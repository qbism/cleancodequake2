file (GLOB_RECURSE CC_SIRCL_FILES src *.cpp)

if (WIN32)
  add_library (sircl STATIC ${CC_SIRCL_FILES})
  target_link_libraries (sircl WS2_32)
elseif (UNIX)
  add_library (sircl SHARED ${CC_SIRCL_FILES})
endif ()
