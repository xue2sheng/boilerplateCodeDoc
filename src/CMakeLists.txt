# this is the "object library" target: compiles the sources only once
file(GLOB TOOL_SOURCES *.cpp)
set(LIB_SOURCES ${TOOL_SOURCES})
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tool.cpp)
set(LIB_OBJECT_NAME "${LOCAL_CMAKE_PROJECT_NAME}_object")
file(GLOB_RECURSE EXTRA_SOURCES *.h* ../include/*.h*) # making happy Qt-Creator project tab
include_directories(../include)
add_library(${LIB_OBJECT_NAME} OBJECT ${LIB_SOURCES} ${EXTRA_SOURCES})

# actual shared and static libraries built from the same object files
add_library(${LIB_STATIC_NAME} STATIC $<TARGET_OBJECTS:${LIB_OBJECT_NAME}>)
#add_library(${LIB_SHARED_NAME} SHARED $<TARGET_OBJECTS:${LIB_OBJECT_NAME}>)
#set_target_properties(${LIB_SHARED_NAME} PROPERTIES VERSION ${HUMAN_VERSION})

# tool binary
add_executable(${TOOL_NAME} ${TOOL_SOURCES})
add_dependencies(${TOOL_NAME} ${LIB_STATIC_NAME})
target_link_libraries(${TOOL_NAME} ${LIB_STATIC_NAME} ${STATIC_LIB_LINKER})

### Only if this the principal project ###
if("${LOCAL_CMAKE_PROJECT_NAME}" STREQUAL "${CMAKE_PROJECT_NAME}")

  ### Install binaries ###
  install(TARGETS ${LIB_STATIC_NAME} ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
  #install(TARGETS ${LIB_SHARED_NAME} LIBRARY DESTINATION ${LIB_INSTALL_DIR})
  install(TARGETS ${LOCAL_TOOL} RUNTIME DESTINATION ${LIB_INSTALL_DIR})

  ### Install headers ###
  file(GLOB HEADERS ../include/*.h)
  install(FILES ${HEADERS} DESTINATION ${HEADERS_INSTALL_DIR})

endif()

