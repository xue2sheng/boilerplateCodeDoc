# code under test
set(LIB_PATH "${CMAKE_BINARY_DIR}/src")

# boost
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_TEST_DYN_LINK")
find_package( Boost 1.54.0 REQUIRED COMPONENTS unit_test_framework )
if(Boost_FOUND)

 ### copy data needed by tests on target directory, just to make it easier run those test cases
 add_custom_target(${TEST_NAME}_data ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../json/ ${CMAKE_CURRENT_BINARY_DIR} DEPENDS ${LIB_STATIC_NAME})

 file(GLOB SOURCES_MAIN main.cpp)
 include_directories(../include)
 link_directories(${LIB_PATH} ${STATIC_LIB_PATH})
 add_executable(${TEST_NAME} ${SOURCES_MAIN})
 add_dependencies(${TEST_NAME} ${LIB_STATIC_NAME} ${TEST_NAME}_data)
 target_link_libraries(${TEST_NAME} ${LIB_STATIC_NAME} ${STATIC_LIB_LINKER} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}) 
 add_test(UnitTest ${TEST_NAME})

 install(TARGETS ${TEST_NAME} RUNTIME DESTINATION ${TEST_INSTALL_DIR})

else()
  message(STATUS "Missing required Boost UNIT TEST framework library. Test building skipped")
endif(Boost_FOUND)

