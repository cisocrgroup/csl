project(csl)

cmake_minimum_required(VERSION 2.4)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
SET(CPACK_PACKAGE_VENDOR "Ulrich Reffle")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "8")
#SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_SOURCE_GENERATOR "TGZ")
# IF(WIN32 AND NOT UNIX)
#   # There is a bug in NSI that does not handle full unix paths properly. Make
#   # sure there is at least one set of four (4) backlasshes.
#   SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#   SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#   SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
#   SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
#   SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
#   SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
#   SET(CPACK_NSIS_MODIFY_PATH ON)
# ELSE(WIN32 AND NOT UNIX)
#   SET(CPACK_STRIP_FILES "bin/MyExecutable")
#   SET(CPACK_SOURCE_STRIP_FILES "")
# ENDIF(WIN32 AND NOT UNIX)
# SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
INCLUDE(CPack)



# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


IF( CMAKE_COMPILER_IS_GNUCXX )
SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -Wall -Wno-unused " )
SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3 " )
#SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -ggdb " )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )

#SET(CMAKE_VERBOSE_MAKEFILE true)

SET( COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} CSL_ROOTDIR='"${csl_SOURCE_DIR}/"' )

ENABLE_TESTING()

#SET( USER_SOFTWARE_DIRS /mounts/Users/student/uli/soft )

include_directories (${csl_SOURCE_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
FIND_PACKAGE( CPPUNIT )

IF( CPPUNIT_FOUND )
 include_directories( ${CPPUNIT_INCLUDE_DIR} )
ENDIF( CPPUNIT_FOUND )


SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(csl)
add_subdirectory(tools)
