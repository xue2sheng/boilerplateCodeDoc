################
# Version info #
################
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/Version.txt" HUMAN_VERSION) 
string(STRIP "${HUMAN_VERSION}" HUMAN_VERSION)
message(STATUS "${LOCAL_CMAKE_PROJECT_NAME} ${HUMAN_VERSION}")

############
# Git info #
############

# Add GIT project name
execute_process(
  COMMAND git config --local remote.origin.url
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_PROJECT_NAME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Add GIT branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Add Git hashcode LONG
execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH_LONG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Add Git hashcode SHORT
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# extra environment info
site_name(CMAKE_HOSTNAME)
set(GIT_SYSTEM "$ENV{USER}@${CMAKE_HOSTNAME}")
execute_process(COMMAND "date"  OUTPUT_VARIABLE DATETIME OUTPUT_STRIP_TRAILING_WHITESPACE)

string(STRIP "${HUMAN_VERSION} ${GIT_PROJECT_NAME} ${GIT_BRANCH} ${GIT_COMMIT_HASH} [${DATETIME}] ${GIT_SYSTEM}" VERSION_INFO)
string(STRIP "${HUMAN_VERSION}_${GIT_COMMIT_HASH}" DOXYGEN_PROJECT_NUMBER)

## some info
message(STATUS "VERSION_INFO: ${VERSION_INFO}")
message(STATUS "DOXYGEN_PROJECT_NUMBER: ${DOXYGEN_PROJECT_NUMBER}")

#####################
### Documentation ###
#####################

set(DOXYGEN_PROJECT_NAME "${LOCAL_CMAKE_PROJECT_NAME}")
set(DOXYGEN_INCLUDE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DOXYGEN_SRC_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(DOXYGEN_MAINPAGE_INPUT "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc${LOCAL_CMAKE_PROJECT_NAME} ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY 
			${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

############################
# version generated header #
############################
set(VERSION_VAR_NAME "${LOCAL_CMAKE_PROJECT_NAME}_VERSION")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h @ONLY)

####################################
# build/install/pack instructions  #
####################################

### Look for faster Ninja buid system in stead of common Unix Makefiles ###
message(STATUS "Current generator: \"${CMAKE_GENERATOR}\"")
find_program(NINJA "ninja")
if(NINJA)
  if("${CMAKE_GENERATOR}" MATCHES "Ninja")
  else()
    message(STATUS "Detected faster \"Ninja\" generator")
    message(STATUS "Invoke again from 'build' folder: rm -rf * && cmake .. -G \"Ninja\"") 
  endif()
endif()

### This can be placed in a specif CMake config file for each machine and use "find_package" ###
if(APPLE)
   set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
   project(${LOCAL_CMAKE_PROJECT_NAME} CXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++14 -g -stdlib=libc++")
   set(STATIC_LIB_LINKER "-lc++ -lc++abi") # not really static
   set(STATIC_LIB_PATH "") # not really static
elseif(UNIX)
  find_program(LSB_RELEASE lsb_release)
  execute_process(COMMAND ${LSB_RELEASE} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  if("${LSB_RELEASE_ID_SHORT}" MATCHES "Debian") # Supposed be at home so default g++ is already version 6
   project(${LOCAL_CMAKE_PROJECT_NAME} CXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++1z -g")
   set(STATIC_LIB_PATH "/usr/lib/gcc/x86_64-linux-gnu/6/") # echo | g++-6 -E -Wp,-v -
  else() # Supposed be at work, opensuse, where default g++ is old 4.x
   set(CMAKE_CXX_COMPILER "/usr/bin/g++-6")
   project(${LOCAL_CMAKE_PROJECT_NAME} CXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++1z -g")
   set(STATIC_LIB_PATH "/usr/lib64/gcc/x86_64-suse-linux/6/") # echo | g++-6 -E -Wp,-v -
  endif("${LSB_RELEASE_ID_SHORT}" MATCHES "Debian")
  set(STATIC_LIB_LINKER "-static-libgcc -static-libstdc++")
endif(APPLE)

message(STATUS "${LOCAL_CMAKE_PROJECT_NAME} C++ flags:    ${CMAKE_COMPILER_FLAGS}")
message(STATUS "${LOCAL_CMAKE_PROJECT_NAME} C++ flags:    ${CMAKE_CXX_FLAGS}")
message(STATUS "${LOCAL_CMAKE_PROJECT_NAME} linker flags: ${CMAKE_LINKER_FLAGS}")

### Code ###
set(LIB_STATIC_NAME "${LOCAL_CMAKE_PROJECT_NAME}_static")
set(${LOCAL_CMAKE_PROJECT_NAME}_STATIC_LIB "${CMAKE_CURRENT_BINARY_DIR}/${LOCAL_CMAKE_PROJECT_NAME}/src/lib${LIB_STATIC_NAME}.a")
set(${LOCAL_CMAKE_PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LOCAL_CMAKE_PROJECT_NAME}/include")
set(LIB_SHARED_NAME "${LOCAL_CMAKE_PROJECT_NAME}")
set(TEST_NAME "${LOCAL_CMAKE_PROJECT_NAME}_test")

### Install ###
if(DEFINED ENV{INSTALL_PREFIX})
  set(INTERNAL_INSTALL_PREFIX "${INSTALL_PREFIX}")
else()
    set(INTERNAL_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()
set(HEADERS_INSTALL_DIR "${INTERNAL_INSTALL_PREFIX}/include")
set(LIB_INSTALL_DIR "${INTERNAL_INSTALL_PREFIX}/lib")
set(TEST_INSTALL_DIR "${INTERNAL_INSTALL_PREFIX}/test")

### Package ###
if("${LOCAL_CMAKE_PROJECT_NAME}" STREQUAL "${CMAKE_PROJECT_NAME}")
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SET_DESTDIR ON)
  set(CPACK_PACKAGE_VERSION "${DOXYGEN_PROJECT_NUMBER}")
  set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/html" html)
  set(CPACK_PACKAGE_INSTALL_PREFIX "")
  include(CPack)
endif()
