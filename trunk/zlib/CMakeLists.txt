file (GLOB_RECURSE CC_ZLIB_FILES . *.c)

if (WIN32)
  add_library (z STATIC ${CC_ZLIB_FILES} win32/zlib.def)
endif ()
