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
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++1z -g -stdlib=libc++")
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
set(TOOL_NAME "${LOCAL_CMAKE_PROJECT_NAME}_tool")
set(TOOL_INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/json/schema.json")
set(TOOL_CPP_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LOCAL_CMAKE_PROJECT_NAME}.h")
set(TOOL_HTML_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LOCAL_CMAKE_PROJECT_NAME}.html")

###############################################
# html & cpp code generation by external tool #
###############################################
function(locate_boilerplate_tool TOOL_NAME TOOL_DIR TOOL_SOURCE_DIR TOOL_INCLUDE_DIR TOOL_BINARY)
 set(TOOL_NAME_VAR "${TOOL_NAME}")
 set(TOOL_DIR_VAR "${TOOL_DIR}")
 set(TOOL_SOURCE_DIR_VAR "${TOOL_SOURCE_DIR}")
 set(TOOL_INCLUDE_DIR_VAR "${TOOL_INCLUDE_DIR}")
 find_program(TOOL_BINARY_VAR NAME ${TOOL_NAME_VAR} HINTS ${TOOL_DIR_VAR})
 if(NOT TOOL_BINARY_VAR)
    # time to build it in a very dirty way but fast!
    file(GLOB TOOL_SOURCE_FILES ${TOOL_SOURCE_DIR_VAR}/*.cpp)
    if(EXISTS ${TOOL_DIR_VAR})
      message(STATUS "Using tool directory: ${TOOL_DIR_VAR}")
    else()
      message(STATUS "Creating tool at directory: ${TOOL_DIR_VAR}")
      file(MAKE_DIRECTORY ${TOOL_DIR_VAR})
    endif()
    set(EXECUTE_COMMAND ${CMAKE_CXX_COMPILER} -v
	-o ${TOOL_DIR_VAR}/${TOOL_NAME_VAR}
	-I${TOOL_INCLUDE_DIR} -I${TOOL_SOURCE_DIR}/.
	${TOOL_SOURCE_FILES})
    execute_process(COMMAND ${EXECUTE_COMMAND} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} TIMEOUT 5
	RESULT_VARIABLE TOOL_BINARY_RESULT OUTPUT_VARIABLE TOOL_BINARY_OUTPUT ERROR_VARIABLE TOOL_BINARY_ERROR)
    find_program(TOOL_BINARY_VAR NAME ${TOOL_NAME_VAR} HINTS ${TOOL_DIR_VAR})
 endif()
 message(STATUS "Tool: ${TOOL_BINARY_VAR}")
 if(NOT TOOL_BINARY_VAR)
      message(STATUS "Tool name: ${TOOL_NAME_VAR}")
      message(STATUS "Tool dir: ${TOOL_DIR_VAR}")
      message(STATUS "Tool source dir: ${TOOL_SOURCE_DIR_VAR}")
      message(STATUS "Tool include dir: ${TOOL_INCLUDE_DIR_VAR}")
      message(STATUS "Tool binary generation: result<${TOOL_BINARY_RESULT}> output<${TOOL_BINARY_OUTPUT}> error<${TOOL_BINARY_ERROR}>")
      message(FATAL_ERROR "Unable to find or build ${TOOL_NAME_VAR} required to generate needed code")
 endif()
 set(${TOOL_BINARY} "${TOOL_BINARY_VAR}" PARENT_SCOPE)
endfunction(locate_boilerplate_tool TOOL_NAME TOOL_DIR TOOL_BINARY)

## tool directory
if(EXISTS ${TOOL_DIR})
    message(STATUS "Tool directory: ${TOOL_DIR}")
else(EXISTS ${TOOL_DIR})
 if(EXISTS $ENV{JSONSCHEMA2CPP})
    set(TOOL_DIR "$ENV{JSONSCHEMA2CPP}")
 else()
    set(TOOL_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
 endif(EXISTS $ENV{JSONSCHEMA2CPP})
endif(EXISTS ${TOOL_DIR})

## tool binary
locate_boilerplate_tool(${TOOL_NAME} ${TOOL_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include" TOOL_BINARY)

## Now that tool is found or created, high time to use it!
execute_process(COMMAND ${TOOL_BINARY} ${TOOL_INPUT_FILE} ${TOOL_HTML_OUTPUT_FILE} ${TOOL_CPP_OUTPUT_FILE} TIMEOUT 5
    RESULT_VARIABLE TOOL_BINARY_RESULT OUTPUT_VARIABLE TOOL_BINARY_OUTPUT ERROR_VARIABLE TOOL_BINARY_ERROR)
if(TOOL_BINARY_RESULT)
    message(FATAL_ERROR "Unable to generated C++/HTML code")
else()
    message(STATUS "Generated HTML: ${TOOL_HTML_OUTPUT_FILE}")
    message(STATUS "Generated C++: ${TOOL_CPP_OUTPUT_FILE}")
endif()

## Create a target to relaunch the tool later on
add_custom_target(launch${TOOL_NAME} ALL ${TOOL_BINARY} ${TOOL_INPUT_FILE} ${TOOL_HTML_OUTPUT_FILE} ${TOOL_CPP_OUTPUT_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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
