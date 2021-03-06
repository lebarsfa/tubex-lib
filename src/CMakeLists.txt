# ==================================================================
#  Codac - cmake configuration file
# ==================================================================

add_subdirectory(core)
add_subdirectory(robotics)
add_subdirectory(3rd) # 3rd party (todo: make a clean dependency)

# todo: specify minimum version of IBEX (in Requires field):


# Generating PKG file
# ===================

set(CODAC_PKG_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/codac.pc)

set(CODAC_PKG_CONFIG_CFLAGS "-I\${includedir}/ibex -I\${includedir}/codac -I\${includedir}/codac-rob -I\${includedir}/codac-pyibex")
set(CODAC_PKG_CONFIG_LIBS "-L\${libdir} -lcodac -lcodac-rob -lcodac-pyibex")

if(WITH_CAPD)
  set(CODAC_PKG_CONFIG_CFLAGS "${CODAC_PKG_CONFIG_CFLAGS} -I\${includedir}/codac-capd")
  set(CODAC_PKG_CONFIG_LIBS "${CODAC_PKG_CONFIG_LIBS} -lcodac-capd")
endif()

set(CODAC_PKG_CONFIG_LIBS "${CODAC_PKG_CONFIG_LIBS} -lcodac") # Seems to be needed

file(GENERATE OUTPUT ${CODAC_PKG_CONFIG_FILE}
              CONTENT "prefix=${CMAKE_INSTALL_PREFIX}
includedir=\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}
libdir=\${prefix}/${CMAKE_INSTALL_LIBDIR}

Name: codac
Description: ${PROJECT_DESCRIPTION}
Url: ${PROJECT_HOMEPAGE_URL}
Version: ${PROJECT_VERSION}
Requires: ibex
Cflags: ${CODAC_PKG_CONFIG_CFLAGS}
Libs: ${CODAC_PKG_CONFIG_LIBS}")

install(FILES ${CODAC_PKG_CONFIG_FILE} DESTINATION ${CMAKE_INSTALL_PKGCONFIG})


# Generating codac-config.cmake file
# ==================================

set(CODAC_CMAKE_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/codac-config.cmake)

file(WRITE ${CODAC_CMAKE_CONFIG_FILE}
           "# Try to find Codac
# This file has been generated by CMake

find_path(CODAC_INCLUDE_DIR codac.h
          PATH_SUFFIXES include/codac)
find_path(CODAC_ROB_INCLUDE_DIR codac-rob.h
          PATH_SUFFIXES include/codac-rob)
find_path(CODAC_PYIBEX_INCLUDE_DIR codac-pyibex.h
          PATH_SUFFIXES include/codac-pyibex)

find_library(CODAC_LIBRARY NAMES codac
             PATH_SUFFIXES lib)
find_library(CODAC_ROB_LIBRARY NAMES codac-rob
             PATH_SUFFIXES lib)
find_library(CODAC_PYIBEX_LIBRARY NAMES codac-pyibex
             PATH_SUFFIXES lib)

set(CODAC_VERSION ${PROJECT_VERSION})
set(CODAC_LIBRARIES \${CODAC_LIBRARY} \${CODAC_ROB_LIBRARY} \${CODAC_PYIBEX_LIBRARY})
set(CODAC_INCLUDE_DIRS \${CODAC_INCLUDE_DIR} \${CODAC_ROB_INCLUDE_DIR} \${CODAC_PYIBEX_INCLUDE_DIR})

set(CODAC_C_FLAGS \"${CMAKE_C_FLAGS}\")
set(CODAC_CXX_FLAGS \"${CMAKE_CXX_FLAGS}\")
")

if((CMAKE_BUILD_TYPE MATCHES Debug) AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
# If Codac is built in Debug, -pg (added by Ibex at the time of Codac 
# compilation) might be also necessary to be able to successfully build 
# programs in Release (otherwise it would be only added by Ibex if the program 
# is built in Debug)...
file(APPEND ${CODAC_CMAKE_CONFIG_FILE} "
set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS} -pg\")
")
endif()

if(WITH_CAPD)

  file(APPEND ${CODAC_CMAKE_CONFIG_FILE} "

  # Optional 3rd party:

  find_path(CODAC_CAPD_INCLUDE_DIR codac-capd.h
            PATH_SUFFIXES include/codac-capd)
  set(CODAC_INCLUDE_DIRS \${CODAC_INCLUDE_DIRS} \${CODAC_CAPD_INCLUDE_DIR})

  find_library(CODAC_CAPD_LIBRARY NAMES codac-capd
               PATH_SUFFIXES lib)
  set(CODAC_LIBRARIES \${CODAC_LIBRARIES} \${CODAC_CAPD_LIBRARY})
  ")

endif()

install(FILES ${CODAC_CMAKE_CONFIG_FILE} DESTINATION ${CMAKE_INSTALL_CMAKE})