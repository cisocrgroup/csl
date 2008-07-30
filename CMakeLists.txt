project(csl)

#SET(CMAKE_CXX_FLAGS -ggdb)
SET(CMAKE_CXX_FLAGS -O3)
#SET(CMAKE_VERBOSE_MAKEFILE true)

ADD_DEFINITIONS( -DCSL_ROOTDIR='"${csl_SOURCE_DIR}/"' )

ENABLE_TESTING()

SET( USER_SOFTWARE_DIRS /mounts/Users/student/uli/soft )

include_directories (${csl_SOURCE_DIR})

SET( CPPUNIT_DIR ${CMAKE_SOURCE_DIR} )
FIND_PACKAGE( CPPUNIT )

include_directories( ${CPPUNIT_INCLUDE_DIR} )


SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(csl)
add_subdirectory(tools)
