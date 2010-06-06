file (GLOB_RECURSE CC_SIRCL_FILES src *.cpp)

add_library (sircl SHARED ${CC_SIRCL_FILES})

